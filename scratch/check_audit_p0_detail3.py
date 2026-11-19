import sys
sys.stdout.reconfigure(encoding='utf-8')

with open(r"C:\Users\fbois\.gemini\antigravity\brain\56ccdda9-7d51-42dd-9df6-ed271c6f6e19\Audit_Complet_Smilewatch_V2.md", "r", encoding="utf-8") as f:
    text = f.read()

pos = text.find("### 1.1")
if pos != -1:
    print(text[pos:pos+2500])
