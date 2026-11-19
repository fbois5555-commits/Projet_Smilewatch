import math

# In S1921 footprint:
# C22 is at (99.057, 66.752, rot=22 deg)
# The Harwin S1921 spring contacts the battery from the side.
# Let's check where the battery cutout / circle is in Carte_V2.kicad_pcb or edge cuts
with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re
# check any silk or fab circles on B.SilkS or B.Fab or Edge.Cuts
circles = re.findall(r'\(gr_circle[\s\S]*?\n\t\)', pcb)
for c in circles:
    print("Circle:", c.strip()[:150])

# Check arcs
arcs = re.findall(r'\(gr_arc[\s\S]*?\n\t\)', pcb)
print(f"Total arcs: {len(arcs)}")
