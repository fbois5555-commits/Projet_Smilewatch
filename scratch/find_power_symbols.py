sch_path = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch"
with open(sch_path, 'r', encoding='utf-8') as f:
    text = f.read()

import re

# Find all symbols in Alimentation_sch with their ref, pos, and pins
symbols = re.findall(r'\(symbol\s+\(lib_id\s+"([^"]+)"\)[\s\S]*?\(property\s+"Reference"\s+"([^"]+)"\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)[\s\S]*?\(instances[\s\S]*?\n\t\)', text)

print(f"Found {len(symbols)} placed symbols.")

# Let's inspect symbols around (26.67, 113.03) and around (232.41, 27.94)
for lib_id, ref, x, y in symbols:
    x_val, y_val = float(x), float(y)
    if (abs(x_val - 26.67) < 30 and abs(y_val - 113.03) < 30) or ref in ['C22', 'BT1', 'U4', 'IC1', 'IC2', 'Q3', 'TH1', 'TH2'] or 'TP' in ref:
        print(f"Ref: {ref:8s} | Lib: {lib_id:30s} | at ({x_val:6.2f}, {y_val:6.2f})")
