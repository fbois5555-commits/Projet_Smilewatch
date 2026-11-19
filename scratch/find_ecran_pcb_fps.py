with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re

# Find footprints with sheetfile "Ecran.kicad_sch"
fp_matches = re.finditer(r'\(footprint\s+"([^"]+)"[\s\S]*?\(property\s+"Reference"\s+"([^"]+)"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)\s*([\d\.-]*)[\s\S]*?\(layer\s+"([^"]+)"\)[\s\S]*?\(sheetfile\s+"Ecran\.kicad_sch"\)', pcb)

print("Footprints from Ecran.kicad_sch:")
for m in fp_matches:
    print(f"Ref: {m.group(2):6s} | Layer: {m.group(6):6s} | at ({float(m.group(3)):.2f}, {float(m.group(4)):.2f}, rot={m.group(5)}) | Footprint: {m.group(1)}")
