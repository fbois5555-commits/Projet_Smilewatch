with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

# Find TP8 instance in Alimentation_sch
pos = text.find('(property "Reference" "TP8"')
start = text.rfind('(symbol', 0, pos)
end = text.find('(symbol', pos)
print("=== TP8 in Alimentation_sch ===")
print(text[start:end])

# Find what wires connect to TP8 at (24.13, 118.11)
lines = text.splitlines()
print("\nWires near (24.13, 118.11):")
for i, l in enumerate(lines):
    if "24.13" in l and any(k in l for k in ["118.11", "113.03", "121.92", "127"]):
        print(f"Line {i}: {l}")
        # print context
        for j in range(max(0, i-5), min(len(lines), i+6)):
            print(f"  {lines[j]}")
        break
