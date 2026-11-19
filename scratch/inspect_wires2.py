import re

sch_path = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch"
with open(sch_path, 'r', encoding='utf-8') as f:
    text = f.read()

wire_blocks = re.findall(r'\(wire\s+\(pts\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s*\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\)', text)
print(f"Total wires parsed: {len(wire_blocks)}")

for x1, y1, x2, y2 in wire_blocks:
    x1, y1, x2, y2 = float(x1), float(y1), float(x2), float(y2)
    if (15 <= x1 <= 150 and 100 <= y1 <= 170) or (15 <= x2 <= 150 and 100 <= y2 <= 170):
        print(f"Wire: ({x1:.2f}, {y1:.2f}) -> ({x2:.2f}, {y2:.2f})")
