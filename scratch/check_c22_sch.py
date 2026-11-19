sch_path = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch"
with open(sch_path, 'r', encoding='utf-8') as f:
    text = f.read()

import re
pos = text.find('(property "Reference" "C22"')
while pos != -1:
    print("--- Instance ---")
    start = text.rfind('(symbol', 0, pos)
    end = text.find('(symbol', pos)
    if end == -1: end = len(text)
    print(text[start:min(start+800, end)])
    pos = text.find('(property "Reference" "C22"', pos+1)
