with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\MCU.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

for m in re.finditer(r'\(symbol\s+\(lib_id\s+"([^"]+)"\)[\s\S]*?\(property\s+"Reference"\s+"([^"]+)"[\s\S]*?\(property\s+"Value"\s+"([^"]*)"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)', text):
    lib, ref, val, x, y = m.group(1), m.group(2), m.group(3), float(m.group(4)), float(m.group(5))
    if 40 <= x <= 75 and 165 <= y <= 185:
        print(f"Ref: {ref:6s} Val: {val:15s} at ({x}, {y}) lib={lib}")
