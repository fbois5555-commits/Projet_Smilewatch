with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re

# find gr_circle or gr_arc on Edge.Cuts
edges = re.findall(r'\(gr_(?:circle|arc|line)[\s\S]*?\(layer "Edge\.Cuts"\)[\s\S]*?\)', pcb)
print(f"Edge.Cuts elements: {len(edges)}")
for e in edges:
    print(e)
