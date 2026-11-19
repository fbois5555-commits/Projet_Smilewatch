import re

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

# Pad K9 is at (103.2825, 66.6625)
# Check all tracks and pads within 1.5mm of K9 on all layers
tracks = re.findall(r'\(segment\s+\(start\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(end\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(width\s+([\d\.-]+)\)\s+\(layer\s+\"([^\"]+)\"\)\s+\(net\s+\"([^\"]+)\"\)', pcb)
vias = re.findall(r'\(via\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(size\s+([\d\.-]+)\)\s+\(drill\s+([\d\.-]+)\)\s+\(layers\s+\"([^\"]+)\"\s+\"([^\"]+)\"\)[\s\S]*?\(net\s+\"([^\"]+)\"\)', pcb)

print("Tracks near K9 (103.28, 66.66):")
for x1, y1, x2, y2, w, l, net in tracks:
    x1, y1, x2, y2 = map(float, [x1, y1, x2, y2])
    if (abs(x1 - 103.28) < 1.5 and abs(y1 - 66.66) < 1.5) or (abs(x2 - 103.28) < 1.5 and abs(y2 - 66.66) < 1.5):
        print(f"  {l:8} w={w} ({x1}, {y1}) -> ({x2}, {y2}) net={net}")

print("\nVias near K9:")
for vx, vy, vs, vd, l1, l2, net in vias:
    vx, vy = float(vx), float(vy)
    if abs(vx - 103.28) < 1.5 and abs(vy - 66.66) < 1.5:
        print(f"  Via at ({vx}, {vy}) net={net}")
