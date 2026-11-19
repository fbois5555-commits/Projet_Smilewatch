import re
import os

sch_path = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch"
with open(sch_path, 'r', encoding='utf-8') as f:
    text = f.read()

refs = re.findall(r'\(property\s+"Reference"\s+"([^"]+)"', text)
print("Symbols in Alimentation_sch:", len(refs), sorted(list(set(refs))))

# Look for battery related labels/symbols
for line in text.splitlines():
    if any(k in line.upper() for k in ["BAT", "BT1", "C22", "TP2", "TP13", "LIR", "S1921", "AP9101", "BQ24040", "BQ51003"]):
        print(line[:120])
