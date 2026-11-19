import sys
sys.stdout.reconfigure(encoding='utf-8')

with open(r"C:\Users\fbois\.gemini\antigravity\brain\56ccdda9-7d51-42dd-9df6-ed271c6f6e19\Audit_Complet_Smilewatch_V2.md", "r", encoding="utf-8") as f:
    text = f.read()

import re
matches = [m.start() for m in re.finditer(r'P0-ELEC-1', text)]
for m in matches[1:]:
    print("--- MATCH ---")
    print(text[m-50:m+600])
