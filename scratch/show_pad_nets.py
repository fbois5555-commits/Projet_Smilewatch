with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb_text = f.read()

import re

for ref in ["C22", "TP2", "TP8", "TP13"]:
    pos = pcb_text.find(f'(property "Reference" "{ref}"')
    start = pcb_text.rfind('(footprint', 0, pos)
    end = pcb_text.find('\n\t(footprint', pos)
    block = pcb_text[start:end]
    pad_match = re.search(r'\(pad\s+"1"[^\)]*\(net\s+(\d+)\s+"([^"]+)"\)', block)
    if pad_match:
        print(f"{ref}: net {pad_match.group(1)} = {pad_match.group(2)}")
    else:
        # search anywhere in pad
        p_idx = block.find('(pad "1"')
        print(f"{ref}: pad 1 text:\n", block[p_idx:p_idx+300])
