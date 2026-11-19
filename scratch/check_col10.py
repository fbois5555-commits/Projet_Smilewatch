import re

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

# Check all pads in column 10 (at dx = 1.72)
for row in ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K']:
    pnum = row + "10"
    m = re.search(r'\(pad\s+\"' + pnum + r'\"[\s\S]*?\(net\s+\"([^\"]+)\"\)', pcb)
    net = m.group(1) if m else "None"
    print(f"Pad {pnum:4}: {net}")
