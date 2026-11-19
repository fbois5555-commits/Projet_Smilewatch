with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

# Find power symbols near 24.13
for m in re.finditer(r'\(symbol[\s\S]*?\(at 24\.13 ([\d\.-]+)[\s\S]*?\n\t\)', text):
    block = m.group(0)
    ref = re.search(r'\(property "Reference" "([^"]+)"', block)
    val = re.search(r'\(property "Value" "([^"]+)"', block)
    at = re.search(r'\(at ([\d\.-]+) ([\d\.-]+)', block)
    print(f"Symbol at ({at.group(1)}, {at.group(2)}): {ref.group(1)} val={val.group(1) if val else '?'}")

# Let's inspect pins of U4 and Q3 in Alimentation_sch
for target in ["U4", "Q3"]:
    pos = text.find(f'(property "Reference" "{target}"')
    block = text[text.rfind('(symbol', 0, pos):text.find('\n\t)', pos)]
    at = re.search(r'\(at ([\d\.-]+) ([\d\.-]+)', block)
    print(f"\nInstance {target} at ({at.group(1)}, {at.group(2)})")
