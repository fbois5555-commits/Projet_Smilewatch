with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\MCU.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re
# check any wire near 166.37 and 163.83 or 36.83
wire_matches = re.findall(r'\(wire[\s\S]*?\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s*\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)', text)
for w in wire_matches:
    pts = [(float(w[0]), float(w[1])), (float(w[2]), float(w[3]))]
    for x, y in pts:
        if abs(x - 166.37) < 2 and (abs(y - 163.83) < 2 or abs(y - 36.83) < 2):
            print(f"Wire connected to PA0: {pts}")
