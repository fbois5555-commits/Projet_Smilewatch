with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

# Look for Q3 and U4 connections
for sym in ["Q3", "U4"]:
    pos = text.find(f'(property "Reference" "{sym}"')
    if pos != -1:
        start = text.rfind('(symbol', 0, pos)
        end = text.find('\n\t)', pos)
        print(f"=== {sym} ===")
        print(text[start:end+4])

# Look for labels around U4 and Q3
labels = re.findall(r'\((?:label|global_label|hierarchical_label)\s+"([^"]+)"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)', text)
for name, x, y in labels:
    if any(k in name for k in ["GND", "VM", "CO", "DO", "BAT", "PWR"]):
        print(f"Label: {name} at ({x}, {y})")
