with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Ecran.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

for sym in ["FDN338P", "FDN335N"]:
    idx = text.find(f'(symbol "{sym}:{sym}_1_1"')
    if idx == -1:
        idx = text.find(f'(symbol "{sym}:{sym}"')
    end = text.find('(symbol', idx + 20)
    print(f"=== {sym} lib definition ===")
    print(text[idx:idx+1200])
