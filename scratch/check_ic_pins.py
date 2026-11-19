with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

for sym in ["AP9101CK", "DMN2028UFU-7"]:
    idx = text.find(f'"{sym}_1_1"')
    if idx == -1: idx = text.find(f'"{sym}"')
    idx_end = text.find('(symbol "', idx+30)
    print(f"=== {sym} pins ===")
    pins = re.findall(r'\(pin\s+\w+\s+\w+[\s\S]*?\(name\s+"([^"]*)"[^\)]*\)\s+\(number\s+"([^"]*)"', text[idx:idx_end])
    for p in pins:
        print(f"Pin {p[1]}: {p[0]}")
