with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\MCU.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re
symbols = re.findall(r'\(symbol\s+\(lib_id\s+"([^"]+)"\)[\s\S]*?\(property\s+"Reference"\s+"([^"]+)"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)', text)
for lib, ref, x, y in symbols:
    x_val, y_val = float(x), float(y)
    if abs(x_val - 128.27) < 10 and abs(y_val - 26.67) < 10:
        print(f"Near 128.27, 26.67: {ref} at ({x_val}, {y_val}) {lib}")
    if abs(x_val - 88.90) < 10 and abs(y_val - 33.02) < 10:
        print(f"Near 88.90, 33.02: {ref} at ({x_val}, {y_val}) {lib}")
