import re

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

tracks = re.findall(r'\(segment\s+\(start\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(end\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(width\s+([\d\.-]+)\)\s+\(layer\s+\"([^\"]+)\"\)\s+\(net\s+\"([^\"]+)\"\)', pcb)

print("Tracks around Column 10 (101 <= x <= 103, 66 <= y <= 70):")
for x1, y1, x2, y2, w, l, net in tracks:
    x1, y1, x2, y2 = map(float, [x1, y1, x2, y2])
    if 101 <= x1 <= 103 and 66 <= y1 <= 70 and l == "F.Cu":
        print(f"  F.Cu w={w} ({x1}, {y1}) -> ({x2}, {y2}) net={net}")
