with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Ecran.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

# Find symbols and power symbols with their position
symbols = re.findall(r'\(symbol\s+\(lib_id\s+"([^"]+)"\)[\s\S]*?\(property\s+"Reference"\s+"([^"]+)"[\s\S]*?\(property\s+"Value"\s+"([^"]*)"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)', text)

for lib, ref, val, x, y in symbols:
    print(f"Symbol {ref:10s} val={val:12s} at ({float(x):.2f}, {float(y):.2f}) lib={lib}")
