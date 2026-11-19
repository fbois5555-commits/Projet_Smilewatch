import re

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

# Let's inspect position of pads G10, G9, K9, H1 on U2
for pnum in ['G10', 'G9', 'G8', 'K9', 'H1', 'G7']:
    m = re.search(r'\(pad\s+\"' + pnum + r'\"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)[\s\S]*?\(net\s+\"([^\"]+)\"\)', pcb)
    if m:
        dx, dy, net = m.groups()
        # U2 is at (104.6025, 68.3825)
        # rotated 180 deg? Let's check U2 rotation:
        print(f"Pad {pnum:4}: offset ({dx}, {dy}) net={net}")
