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

init_req = {
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {
        "protocolVersion": "2024-11-05",
        "capabilities": {},
        "clientInfo": {"name": "test", "version": "1.0"}
    }
}

p.stdin.write(json.dumps(init_req) + "\n")
p.stdin.flush()

time.sleep(1)
line = p.stdout.readline()
print("Response:", line)

# List tools request
tools_req = {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "tools/list",
    "params": {}
}
p.stdin.write(json.dumps(tools_req) + "\n")
p.stdin.flush()

time.sleep(1)
line2 = p.stdout.readline()
data = json.loads(line2)
print("Tools count:", len(data.get("result", {}).get("tools", [])))
print("Tool names sample:", [t["name"] for t in data.get("result", {}).get("tools", [])[:10]])

p.terminate()
