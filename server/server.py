import socket
import os
from openai import OpenAI
from openai import RateLimitError
import uuid
import json
from dotenv import load_dotenv
import time
import re
# === Init ===
load_dotenv()
# Initialize OpenAI client
client = OpenAI(api_key=os.environ.get("OPENAI_API_KEY"))
AGENTS_FILE = "agents.json"
sessions = {}
MAX_TOKENS_LIMIT = 1000
MAX_INPUT_SIZE = 8192
# === Agent Handling ===
def load_agents():
    with open(AGENTS_FILE, "r", encoding="utf-8") as f:
        return json.load(f)
def get_all_agents_summary():
    agents = load_agents()
    return [
        {"id": a["id"], "name": a["name"], "description": a["description"]}
        for a in agents
    ]
def get_agent_details(agent_id):
    agents = load_agents()
    for agent in agents:
        if agent["id"] == agent_id:
            return {
                "name": agent["name"],
                "description": agent["description"],
                "base_prompt": agent["base_prompt"],
                "model": agent.get("model", "gpt-5.5")
            }
    return None

# === Output Sanitization ===
def sanitize_for_dos(text, max_len=1200):
    # Normalize Unicode smart quotes/dashes to plain ASCII
    replacements = {
        '\u2018': "'", '\u2019': "'",   # single curly quotes
        '\u201c': '"', '\u201d': '"',   # double curly quotes
        '\u2013': '-', '\u2014': '-',   # en dash, em dash
        '\u2026': '...',                # ellipsis
    }
    for uni_char, ascii_char in replacements.items():
        text = text.replace(uni_char, ascii_char)
    # Strip markdown bold/italic markers
    text = re.sub(r'\*+', '', text)
    # Strip markdown links, keep just the link text
    text = re.sub(r'\[([^\]]+)\]\([^\)]+\)', r'\1', text)
    # Strip markdown horizontal rule dividers (---, ***, ___ on their own line)
    text = re.sub(r'^\s*[-*_]{3,}\s*$', '', text, flags=re.MULTILINE)
    # Collapse resulting blank lines from removed dividers
    text = re.sub(r'\n{3,}', '\n\n', text)
    # Drop any remaining non-ASCII characters
    text = text.encode('ascii', errors='ignore').decode('ascii')
    # Truncate to a safe length for the DOS client
    return text.strip()[:max_len]
# === Command Handling ===
def handle_command(command_str):
    parts = [part.strip() for part in command_str.split(",")]
    command = parts[0]
    if command == "ListAgents":
        summaries = get_all_agents_summary()
        lines = [
            f"ID: {a['id']} | Name: {a['name']} | Desc: {a['description']}"
            for a in summaries
        ]
        return "00000000", "\n".join(lines)
    
    elif command == "TestConnection":
        try:
            response = client.chat.completions.create(
                model="gpt-5.5",
                messages=[{"role": "user", "content": "Hello"}],
                max_completion_tokens=1
            )
            return "00000000", "Online"
        except Exception as e:
            print(f"TestConnection failed: {e}") 
            return "00000000", "Offline"
    
    elif command == "StartChat":
        if len(parts) != 2:
            return "00000000", "Error: Missing agent ID"
        agent_id = parts[1]
        agent = get_agent_details(agent_id)
        if not agent:
            return "00000000", f"Error: Agent with ID '{agent_id}' not found"
        session_id = str(uuid.uuid4())[:8]
        sessions[session_id] = {
            "model": agent["model"],
            "messages": [{"role": "system", "content": agent["base_prompt"]}]
        }
        return session_id, f"Session started with Agent {agent_id}"
    elif command == "Chat":
        if len(parts) < 3:
            return "00000000", "Error: Missing session ID or message"
        session_id = parts[1]
        message = ",".join(parts[2:]).strip()
        if session_id not in sessions:
            return session_id, "Error: Session ID not found"
        
        session = sessions[session_id]
        session["messages"].append({"role": "user", "content": message})
        try:
            create_kwargs = {
                "model": session["model"],
                "messages": session["messages"],
                "max_completion_tokens": MAX_TOKENS_LIMIT
            }
            if session["model"] == "gpt-5-search-api":
                create_kwargs["web_search_options"] = {}
            response = client.chat.completions.create(**create_kwargs)
            reply = response.choices[0].message.content.strip()
            reply = sanitize_for_dos(reply)
        except RateLimitError as e:
            print(f"Chat rate limited (session {session_id}): {e}")
            # Remove the user message we appended since it never got a reply
            session["messages"].pop()
            reply = "The System is thinking hard and needs a moment. Please try again shortly."
            return session_id, reply
        except Exception as e:
            print(f"Chat error (session {session_id}): {e}")
            reply = "SYSTEM ERROR: Unable to process your request right now. Please try again shortly."
            session["messages"].pop()  # remove the user message so it's not stuck in a broken session
            return session_id, reply
        session["messages"].append({"role": "assistant", "content": reply})
        return session_id, reply
    elif command == "EndChat":
        if len(parts) != 2:
            return "00000000", "Error: Missing session ID"
        session_id = parts[1]
        if session_id in sessions:
            del sessions[session_id]
            return session_id, f"Session {session_id} ended"
        return session_id, "Error: Session ID not found"
    return "00000000", "Error: Unknown command"
# === TCP Server ===
def run_server():
    HOST = "0.0.0.0"
    PORT = 5000
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((HOST, PORT))
        server.listen(1)
        print(f"Server listening on port {PORT}")
        while True:
            client_conn, addr = server.accept()
            client_conn.settimeout(10)  # 10s socket timeout
            with client_conn:
                try:
              
                    print(f"Accepted connection from {addr}")
                    incoming = client_conn.recv(MAX_INPUT_SIZE).decode("ascii", errors="ignore").strip()
                    if len(incoming) > MAX_INPUT_SIZE:
                        print("Rejected input: too long")
                        client_conn.sendall(b"Error: Input too long")
                        continue
                    print(f"Received: {incoming}")
                    # Parse command and handle it
                    # Example: "ListAgents" or "StartChat,agent_id"
                    session_id, message = handle_command(incoming)
                    # Respond
                    full_response = f"{session_id}\n{message}"
                    try:
                        client_conn.sendall(full_response.encode("ascii", errors="replace"))
                        print("Sent response to client.\n")
                    except Exception as e:
                        print(f"Error sending response: {e}")
                except Exception as e:
                    print(f"Unexpected error with client {addr}: {e}")
                    try:
                        client_conn.sendall(b"00000000\nMESSAGE=Server error occurred")
                    except:
                        pass
                finally:
                    client_conn.close()
if __name__ == "__main__":
    run_server()
# def run_server_works_with_openai():
#     with socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0) as server:
#         server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
#         server.bind((HOST, PORT))
#         server.listen(1)
#         print("Server listening on port", PORT)
#         while True:
#             client_conn, addr = server.accept()
#             print(f"Accepted connection from {addr}")
#             try:
#                 incoming = client_conn.recv(1024).decode("utf-8", errors="ignore")
#                 print(f"Received: {incoming}")
#                 # Parse SESSIONID and MESSAGE fields from lines
#                 session_id, prompt = "", ""
#                 for line in incoming.strip().splitlines():
#                     if line.startswith("SESSIONID"):
#                         session_id = line.split("=", 1)[1].strip()
#                     elif line.startswith("MESSAGE"):
#                         prompt = line.split("=", 1)[1].strip()
#                 if not session_id or not prompt:
#                     raise ValueError("Missing SESSIONID or MESSAGE")
#                 response = client.chat.completions.create(
#                     model="gpt-5.5",
#                     messages=[
#                         {"role": "system", "content": "You are The System, an all-knowing assistant."},
#                         {"role": "user", "content": prompt}
#                     ],
#                     max_tokens=300
#                 )
#                 message = response.choices[0].message.content.strip()
#                 safe_message = message.replace("\n", " ")[:2000]  # truncate and clean
#                 full_response = f"SESSIONID = {session_id}\nMESSAGE = {safe_message}"
#                 client_conn.sendall(full_response.encode("utf-8", errors="ignore"))
#                 print("Sent response to client.\n")
#             except Exception as e:
#                 error_msg = f"{session_id}\nMESSAGE=Error: {str(e)}"
#                 client_conn.sendall(error_msg.encode())
#                 print(f"Error occurred: {e}")
#             finally:
#                 client_conn.close()
# accept, receive, send, repeat
# def run_server_orig():
#     with socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0) as server:
#         server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
#         server.bind((HOST, PORT))
#         server.listen(1)
#         print("Starting Communication Loop, Enter Ctrl+Break to Exit.")
#         while True:
#             #time.sleep(2); print("Wait before accept"); time.sleep(8); print("wait complete, accepting...")
#             client, addr = server.accept()
#             print(f"Accepted Client at {addr}")
#             # receive and send
#             response = client.recv(1024).decode()
#             print(f"Received: {response}")
#             #print("Wait before send."); time.sleep(6); print("wait complete, sending...")
#             client.sendall((response.split("\n")[0] +
#                             "\nMESSAGE=Python Server Prompt response. V2.")
#                             .encode())
#             print("Sent Response to Client!\n\nCurrent Communication Terminated, ready for next...")
#     print("Server closed")
