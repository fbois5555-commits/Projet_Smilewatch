with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\MCU.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re
symbols = re.findall(r'\(symbol\s+\(lib_id\s+"([^"]+)"\)[\s\S]*?\(property\s+"Reference"\s+"([^"]+)"', text)
for lib, ref in symbols:
    if "IC" in ref or "U" in ref or "STM" in lib:
        print(ref, lib)
