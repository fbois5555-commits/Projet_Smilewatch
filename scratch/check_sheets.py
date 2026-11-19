with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re
sheets = re.findall(r'\(sheet[\s\S]*?\(property "Sheetname" "([^"]+)"[\s\S]*?\(property "Sheetfile" "([^"]+)"', text)
print("Hierarchical sheets in Carte_V2.kicad_sch:")
for name, f in sheets:
    print(f"Sheet: {name:20s} -> File: {f}")
