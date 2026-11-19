with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re

# Find TP8 block
pos = pcb.find('(property "Reference" "TP8"')
start = pcb.rfind('(footprint', 0, pos)
end = pcb.find('\n\t(footprint', pos)
print("=== TP8 in PCB ===")
print(pcb[start:end])

# Check Edge.Cuts near TP8 (111.5, 89.5)
# In Edge.Cuts, the central cutout was from 96.5 to 114.7 at Y=72.5 to 86.85
# TP8 is at Y=89.535, which is south of Y=86.85 (below the Miyota cutout!)
print("Edge of Miyota cutout was at Y=86.86 mm.")
print("TP8 center is at Y=89.535 mm, with radius 3mm (pad from Y=86.535 to 92.535 mm).")
