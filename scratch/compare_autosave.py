with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\_autosave-Alimentation_sch.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

for line in text.splitlines():
    if any(k in line for k in ["C22", "BT1", "S1921"]):
        print(line[:120])

# Also check UUID of C22 in both files
def get_c22_info(content):
    pos = content.find('(property "Reference" "C22"')
    if pos != -1:
        start = content.rfind('(symbol', 0, pos)
        end = content.find('\n\t)', pos)
        return content[start:end]
    return "Not found"

print("\n--- In _autosave ---")
print(get_c22_info(text)[:400])

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch", "r", encoding="utf-8") as f:
    orig = f.read()
print("\n--- In original ---")
print(get_c22_info(orig)[:400])
