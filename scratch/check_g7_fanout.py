import re

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

# Check vias near (105.12, 68.90) within 1.5mm
vias = re.findall(r'\(via\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(size\s+([\d\.-]+)\)\s+\(drill\s+([\d\.-]+)\)\s+\(layers\s+\"([^\"]+)\"\s+\"([^\"]+)\"\)[\s\S]*?\(net\s+\"([^\"]+)\"\)', pcb)

print("Vias near G7 (105.12, 68.90):")
for vx, vy, vs, vd, l1, l2, net in vias:
    vx, vy = float(vx), float(vy)
    if abs(vx - 105.12) < 1.5 and abs(vy - 68.90) < 1.5:
        print(f"  Via at ({vx}, {vy}) net={net}")

# Check tracks near G7
tracks = re.findall(r'\(segment\s+\(start\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(end\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(width\s+([\d\.-]+)\)\s+\(layer\s+\"([^\"]+)\"\)\s+\(net\s+\"([^\"]+)\"\)', pcb)
print("\nTracks near G7:")
for x1, y1, x2, y2, w, l, net in tracks:
    x1, y1, x2, y2 = map(float, [x1, y1, x2, y2])
    if (abs(x1 - 105.12) < 0.6 and abs(y1 - 68.90) < 0.6) or (abs(x2 - 105.12) < 0.6 and abs(y2 - 68.90) < 0.6):
        print(f"  Track from ({x1}, {y1}) to ({x2}, {y2}) net={net} layer={l}")
