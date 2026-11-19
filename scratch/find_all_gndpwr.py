with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

for m in re.finditer(r'\(symbol[\s\S]*?\(property "Value" "GNDPWR"[\s\S]*?\n\t\)', text):
    block = m.group(0)
    at = re.search(r'\(at ([\d\.-]+) ([\d\.-]+)', block)
    ref = re.search(r'\(property "Reference" "([^"]+)"', block)
    print(f"GNDPWR at ({at.group(1)}, {at.group(2)}) ref={ref.group(1)}")
