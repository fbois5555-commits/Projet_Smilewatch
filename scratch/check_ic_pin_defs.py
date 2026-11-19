with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

# find pin definitions of AP9101CK
idx = text.find('AP9101CK"')
idx_end = text.find('(symbol "', idx+20)
print("=== AP9101CK Symbol Def ===")
for line in text[idx:idx_end].splitlines():
    if "pin " in line or "name " in line:
        print(line)

# find pin definitions of DMN2028UFU-7
idx2 = text.find('DMN2028UFU-7"')
idx2_end = text.find('(symbol "', idx2+20)
print("=== DMN2028UFU-7 Symbol Def ===")
for line in text[idx2:idx2_end].splitlines():
    if "pin " in line or "name " in line:
        print(line)
