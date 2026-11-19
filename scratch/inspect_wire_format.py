sch_path = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch"
with open(sch_path, 'r', encoding='utf-8') as f:
    text = f.read()

import re
wire_matches = [line for line in text.splitlines() if line.strip().startswith('(wire')]
print("Wire lines count:", len(wire_matches))
for i, line in enumerate(text.splitlines()[:200]):
    if "(wire" in line:
        for j in range(i, min(i+10, len(text.splitlines()))):
            print(text.splitlines()[j])
        break
