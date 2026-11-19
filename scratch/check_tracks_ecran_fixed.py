import re

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

# Find segments for target nets
pattern = r'\(segment\s+\(start\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(end\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(width\s+([\d\.-]+)\)\s+\(layer\s+\"([^\"]+)\"\)\s+\(net\s+\"([^\"]+)\"\)'

for m in re.finditer(pattern, pcb):
    x1, y1, x2, y2, w, layer, net = m.groups()
    if any(k in net for k in ['+3V8', 'Ecran', 'Q1']):
        print(f"Net: {net:20} Layer: {layer:10} Width: {w:5} mm  from ({x1}, {y1}) to ({x2}, {y2})")

# Find vias for target nets
pattern_via = r'\(via\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(size\s+([\d\.-]+)\)\s+\(drill\s+([\d\.-]+)\)\s+\(layers\s+\"([^\"]+)\"\s+\"([^\"]+)\"\)[\s\S]*?\(net\s+\"([^\"]+)\"\)'
for m in re.finditer(pattern_via, pcb):
    vx, vy, vs, vd, l1, l2, net = m.groups()
    if any(k in net for k in ['+3V8', 'Ecran', 'Q1']):
        print(f"VIA: {net:20} at ({vx}, {vy}) size={vs} drill={vd} {l1}->{l2}")
