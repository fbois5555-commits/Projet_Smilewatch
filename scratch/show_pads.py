with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb_text = f.read()

import re
pos = pcb_text.find('(property "Reference" "C22"')
start = pcb_text.rfind('(footprint', 0, pos)
end = pcb_text.find('\n\t(footprint', pos)
c22_block = pcb_text[start:end]

for p in re.finditer(r'\(pad\s+([^\n]+(?:\n[^\)]+)*\))', c22_block):
    print("C22 PAD:", p.group(0)[:150])

pos2 = pcb_text.find('(property "Reference" "TP2"')
start2 = pcb_text.rfind('(footprint', 0, pos2)
end2 = pcb_text.find('\n\t(footprint', pos2)
tp2_block = pcb_text[start2:end2]
for p in re.finditer(r'\(pad\s+([^\n]+(?:\n[^\)]+)*\))', tp2_block):
    print("TP2 PAD:", p.group(0)[:150])
