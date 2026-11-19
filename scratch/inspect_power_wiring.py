import re

sch_path = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch"
with open(sch_path, 'r', encoding='utf-8') as f:
    text = f.read()

# Let's see all wires and labels in range x in [10, 80], y in [100, 170] (C22 and U4 and Q3 area)
wire_pat = r'\(wire\s+\(pts\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\)[\s\S]*?\)'
wires = re.findall(wire_pat, text)

print(f"Total wires found: {len(wires)}")
for x1, y1, x2, y2 in wires:
    x1, y1, x2, y2 = float(x1), float(y1), float(x2), float(y2)
    if (10 <= x1 <= 160 and 100 <= y1 <= 170) or (10 <= x2 <= 160 and 100 <= y2 <= 170):
        print(f"Wire: ({x1:.2f}, {y1:.2f}) -> ({x2:.2f}, {y2:.2f})")

label_pat = r'\((?:label|global_label|hierarchical_label)\s+"([^"]+)"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)'
for l in re.finditer(label_pat, text):
    name, lx, ly = l.group(1), float(l.group(2)), float(l.group(3))
    print(f"Label: {name:20s} at ({lx:.2f}, {ly:.2f})")
