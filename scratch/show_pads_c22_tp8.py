with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re

for ref in ["C22", "TP8"]:
    pos = pcb.find(f'(property "Reference" "{ref}"')
    start = pcb.rfind('(footprint', 0, pos)
    end = pcb.find('\n\t(footprint', pos)
    block = pcb[start:end]
    p_idx = block.find('(pad "1"')
    print(f"=== {ref} PAD 1 ===")
    print(block[p_idx:p_idx+350])
