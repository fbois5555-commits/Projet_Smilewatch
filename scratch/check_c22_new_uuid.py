with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\_autosave-Alimentation_sch.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

for m in re.finditer(r'\(symbol[\s\S]*?\(property "Reference" "C22"[\s\S]*?\n\t\)', text):
    block = m.group(0)
    uuid_m = re.search(r'\(uuid "([^"]+)"\)', block)
    path_m = re.search(r'\(path "([^"]+)"', block)
    print("Autosave C22 uuid:", uuid_m.group(1) if uuid_m else "no uuid")
    print("Autosave C22 path:", path_m.group(1) if path_m else "no path")
