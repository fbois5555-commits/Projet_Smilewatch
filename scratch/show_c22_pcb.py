with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb_text = f.read()

import re
pos = pcb_text.find('(property "Reference" "C22"')
if pos != -1:
    start = pcb_text.rfind('(footprint', 0, pos)
    end = pcb_text.find('\n  (footprint', pos)
    if end == -1: end = pos + 2000
    print(pcb_text[start:end])
