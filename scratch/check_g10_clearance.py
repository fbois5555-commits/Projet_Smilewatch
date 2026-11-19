import re

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

# U2 rotation:
idx = pcb.find('(property "Reference" "U2"')
start = pcb.rfind('(footprint', 0, idx)
fp_header = pcb[start:start+100]
print("FP header:", fp_header)
# In footprint, at 104.6025 68.3825 180
# If rotated 180: pad at offset (dx, dy) is at (104.6025 - dx, 68.3825 - dy)
# For G10: offset (1.72, 0.52) -> (104.6025 - 1.72, 68.3825 - 0.52) = (102.8825, 67.8625)
print("Pad G10 absolute pos: (102.8825, 67.8625)")

# Check if any tracks or obstacles near (102.88, 67.86) on F.Cu
tracks = re.findall(r'\(segment\s+\(start\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(end\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(width\s+([\d\.-]+)\)\s+\(layer\s+\"F\.Cu\"\)\s+\(net\s+\"([^\"]+)\"\)', pcb)

print("Tracks on F.Cu near G10:")
for x1, y1, x2, y2, w, net in tracks:
    x1, y1, x2, y2 = map(float, [x1, y1, x2, y2])
    if (abs(x1 - 102.88) < 1.0 and abs(y1 - 67.86) < 1.0) or (abs(x2 - 102.88) < 1.0 and abs(y2 - 67.86) < 1.0):
        print(f"  F.Cu track from ({x1}, {y1}) to ({x2}, {y2}) net={net}")
