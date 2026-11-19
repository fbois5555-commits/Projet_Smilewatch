with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Ecran.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

for sym in ["FDN338P_1_1", "FDN335N_1_1"]:
    idx = text.find(f'(symbol "{sym}"')
    end = text.find(')\n\t\t)', idx)
    sub = text[idx:end]
    print(f"=== PINS FOR {sym} ===")
    for line in sub.splitlines():
        if "(pin " in line or "(name " in line or "(number " in line or "(xy " in line:
            print(line)
