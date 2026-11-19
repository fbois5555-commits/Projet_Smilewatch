import re

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

# Find pad G7 of U2
idx = pcb.find('(property "Reference" "U2"')
if idx != -1:
    start = pcb.rfind('(footprint', 0, idx)
    end = pcb.find('\n\t)', idx)
    fp = pcb[start:end]
    at_m = re.search(r'\(at\s+([\d\.-]+)\s+([\d\.-]+)', fp)
    print("U2 position:", at_m.groups() if at_m else "?")
    pad_m = re.search(r'\(pad\s+\"G7\"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)[\s\S]*?\(net\s+\"([^\"]+)\"\)', fp)
    if pad_m:
        print("Pad G7:", pad_m.groups())
    else:
        # search pad G7 block
        idx_g7 = fp.find('(pad "G7"')
        print("Pad G7 block:", fp[idx_g7:idx_g7+200])
