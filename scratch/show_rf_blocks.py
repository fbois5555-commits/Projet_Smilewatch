with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\MCU.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re
for ref in ["ANT1", "R21", "R24"]:
    pos = text.find(f'(property "Reference" "{ref}"')
    start = text.rfind('(symbol', 0, pos)
    end = text.find('\n\t)', pos)
    print(f"=== Block for {ref} ===")
    print(text[start:end+4])
