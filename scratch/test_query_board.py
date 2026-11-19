import subprocess
import json
import time

p = subprocess.Popen(
    [r"C:\Users\fbois\.gemini\antigravity\mcp_bin\kipilot\kipilot-mcp-0.4.0-windows-x64\kipilot-mcp.exe"],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True
)

def send_recv(req):
    p.stdin.write(json.dumps(req) + "\n")
    p.stdin.flush()
    time.sleep(0.5)
    return json.loads(p.stdout.readline())

send_recv({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {
        "protocolVersion": "2024-11-05",
        "capabilities": {},
        "clientInfo": {"name": "test", "version": "1.0"}
    }
})

docs_res = send_recv({
    "jsonrpc": "2.0",
    "id": 2,
    "method": "tools/call",
    "params": {
        "name": "kicad_list_open_documents",
        "arguments": {}
    }
})

print("Open docs:", json.dumps(docs_res, indent=2))

board_res = send_recv({
    "jsonrpc": "2.0",
    "id": 3,
    "method": "tools/call",
    "params": {
        "name": "kicad_get_board_summary",
        "arguments": {}
    }
})
print("Board summary:", json.dumps(board_res, indent=2))

p.terminate()
