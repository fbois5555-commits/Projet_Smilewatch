import re

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

# U2 is at (104.6025, 68.3825) rot 180
# Let's find absolute coordinates of pads:
# K9, G10, J8, J7, H7, G8, G9, H5, H1
candidate_pins = ['K9', 'G10', 'J8', 'J7', 'H7', 'G8', 'G9', 'H5', 'H1']

tracks = re.findall(r'\(segment\s+\(start\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(end\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(width\s+([\d\.-]+)\)\s+\(layer\s+\"([^\"]+)\"\)\s+\(net\s+\"([^\"]+)\"\)', pcb)
vias = re.findall(r'\(via\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(size\s+([\d\.-]+)\)\s+\(drill\s+([\d\.-]+)\)\s+\(layers\s+\"([^\"]+)\"\s+\"([^\"]+)\"\)[\s\S]*?\(net\s+\"([^\"]+)\"\)', pcb)

print("=== CANDIDATE PINS LOCATION AND STATUS ===")
for pnum in candidate_pins:
    m = re.search(r'\(pad\s+\"' + pnum + r'\"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)[\s\S]*?\(net\s+\"([^\"]+)\"\)', pcb)
    if m:
        dx, dy, net = float(m.group(1)), float(m.group(2)), m.group(3)
        # rotated 180: (104.6025 - dx, 68.3825 - dy)
        ax = round(104.6025 - dx, 4)
        ay = round(68.3825 - dy, 4)
        
        # Count nearby tracks on F.Cu within 0.5mm
        nearby_fcu = 0
        for x1, y1, x2, y2, w, l, tnet in tracks:
            if l == "F.Cu":
                x1, y1, x2, y2 = map(float, [x1, y1, x2, y2])
                if abs(x1 - ax) < 0.35 and abs(y1 - ay) < 0.35:
                    nearby_fcu += 1
        
        # Check nearby vias within 0.5mm
        nearby_vias = 0
        for vx, vy, vs, vd, l1, l2, vnet in vias:
            vx, vy = float(vx), float(vy)
            if abs(vx - ax) < 0.35 and abs(vy - ay) < 0.35:
                nearby_vias += 1
                
        print(f"Pin {pnum:4} at ({ax:8.4f}, {ay:8.4f}) net={net:28} -> nearby F.Cu tracks: {nearby_fcu}, vias: {nearby_vias}")
