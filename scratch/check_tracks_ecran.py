import re

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

# Let's inspect tracks for nets: +3V8, /Ecran/VBAT, Net-(Q1-G), /Ecran/GPIO_EC, /Ecran/C2P, /Ecran/C2N, /Ecran/CIP, /Ecran/CIN, /Ecran/IREF
target_nets = ['+3V8', '/Ecran/VBAT', 'Net-(Q1-G)', '/Ecran/GPIO_EC', '/Ecran/C2P', '/Ecran/C2N', '/Ecran/CIP', '/Ecran/CIN', '/Ecran/IREF']

tracks = re.findall(r'\(segment\s+\(start\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(end\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(width\s+([\d\.-]+)\)\s+\(layer\s+\"([^\"]+)\"\)\s+\(net\s+(\d+)\)', pcb)

# Map net ids to names
net_map = {}
for m in re.finditer(r'\(net\s+(\d+)\s+\"([^\"]+)\"\)', pcb):
    net_map[m.group(1)] = m.group(2)

print("=== SEGMENTS FOR TARGET NETS ===")
for x1, y1, x2, y2, w, layer, net_id in tracks:
    net_name = net_map.get(net_id, "")
    if any(t in net_name for t in ['+3V8', 'Ecran', 'Q1']):
        print(f"Net {net_name:20} Layer {layer:10} Width {w:5} ({x1}, {y1}) -> ({x2}, {y2})")

# Also check vias on these nets
vias = re.findall(r'\(via\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(size\s+([\d\.-]+)\)\s+\(drill\s+([\d\.-]+)\)\s+\(layers\s+\"([^\"]+)\"\s+\"([^\"]+)\"\)[\s\S]*?\(net\s+(\d+)\)', pcb)
print("\n=== VIAS FOR TARGET NETS ===")
for vx, vy, vs, vd, l1, l2, net_id in vias:
    net_name = net_map.get(net_id, "")
    if any(t in net_name for t in ['+3V8', 'Ecran', 'Q1']):
        print(f"Via Net {net_name:20} at ({vx}, {vy}) size={vs} drill={vd} layers={l1}->{l2}")
