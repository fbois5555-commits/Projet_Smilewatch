with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re
import math

pad_x = 111.506
pad_y = 89.535
pad_r = 3.0 # 6mm diameter

print(f"Checking pad at ({pad_x}, {pad_y}), radius {pad_r} mm")

# Find tracks on B.Cu near pad
segments = re.findall(r'\(segment\s+\(start ([\d\.-]+) ([\d\.-]+)\)\s+\(end ([\d\.-]+) ([\d\.-]+)\)\s+\(width ([\d\.-]+)\)\s+\(layer "([^"]+)"\)\s+\(net "([^"]+)"\)', pcb)

for x1, y1, x2, y2, w, layer, net in segments:
    if layer == "B.Cu" and net != "GNDPWR":
        # check distance to (pad_x, pad_y)
        x1, y1, x2, y2, w = float(x1), float(y1), float(x2), float(y2), float(w)
        # distance from point to segment
        dx = x2 - x1
        dy = y2 - y1
        if dx == 0 and dy == 0:
            dist = math.hypot(pad_x - x1, pad_y - y1)
        else:
            t = max(0, min(1, ((pad_x - x1)*dx + (pad_y - y1)*dy) / (dx*dx + dy*dy)))
            proj_x = x1 + t*dx
            proj_y = y1 + t*dy
            dist = math.hypot(pad_x - proj_x, pad_y - proj_y)
        clearance = dist - pad_r - (w / 2.0)
        if clearance < 0.5:
            print(f"WARNING: Track on B.Cu net '{net}' too close! Clearance = {clearance:.3f} mm, pts: ({x1},{y1}) -> ({x2},{y2})")

# Check vias near pad not in GNDPWR
vias = re.findall(r'\(via\s+\(at ([\d\.-]+) ([\d\.-]+)\)\s+\(size ([\d\.-]+)\)\s+\(drill ([\d\.-]+)\)[^\)]*\(net "([^"]+)"\)', pcb)
for vx, vy, vs, vd, vnet in vias:
    vx, vy, vs = float(vx), float(vy), float(vs)
    if vnet != "GNDPWR":
        dist = math.hypot(pad_x - vx, pad_y - vy)
        clearance = dist - pad_r - (vs / 2.0)
        if clearance < 0.5:
            print(f"WARNING: Via net '{vnet}' at ({vx}, {vy}) too close! Clearance = {clearance:.3f} mm")

# Check other footprints pads on B.Cu
# find all pads on B.Cu
blocks = pcb.split('(footprint ')
for b in blocks[1:]:
    ref_m = re.search(r'\(property "Reference" "([^"]+)"', b)
    ref = ref_m.group(1) if ref_m else "?"
    if ref != "TP8":
        at_fp = re.search(r'\(at ([\d\.-]+) ([\d\.-]+)', b)
        if at_fp:
            fx, fy = float(at_fp.group(1)), float(at_fp.group(2))
            if math.hypot(pad_x - fx, pad_y - fy) < 8.0:
                print(f"Footprint {ref} nearby at ({fx}, {fy})")
