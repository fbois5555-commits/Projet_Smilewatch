import re

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

components = ['Q1', 'Q2', 'R13', 'R14', 'R15', 'C39', 'C20', 'C45', 'C42', 'C43', 'C44']

for ref in components:
    idx = pcb.find(f'(property "Reference" "{ref}"')
    if idx != -1:
        start = pcb.rfind('(footprint', 0, idx)
        end = pcb.find('\n\t)', idx)
        fp_block = pcb[start:end]
        at_m = re.search(r'\(at\s+([\d\.-]+)\s+([\d\.-]+)(?:\s+([\d\.-]+))?\)', fp_block)
        pos = at_m.groups() if at_m else ("?", "?", "?")
        layer_m = re.search(r'\(layer\s+\"([^\"]+)\"\)', fp_block)
        layer = layer_m.group(1) if layer_m else "?"
        pads = re.findall(r'\(pad\s+\"([^\"]+)\"[\s\S]*?\(net\s+\"([^\"]+)\"\)', fp_block)
        print(f"=== {ref} at ({pos[0]}, {pos[1]}) layer={layer} ===")
        for pnum, pnet in pads:
            print(f"  Pad {pnum}: net={pnet}")
