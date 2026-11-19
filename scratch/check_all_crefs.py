import os
import re

kicad_dir = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2"

for f in os.listdir(kicad_dir):
    if f.endswith(".kicad_sch"):
        path = os.path.join(kicad_dir, f)
        with open(path, "r", encoding="utf-8") as file:
            text = file.read()
        refs = re.findall(r'\(property\s+"Reference"\s+"([^"]+)"', text)
        c_refs = [r for r in refs if r.startswith("C")]
        print(f"{f}: {len(c_refs)} C-refs -> {sorted(set(c_refs))}")
