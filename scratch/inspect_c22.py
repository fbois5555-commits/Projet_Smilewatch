import re

sch_path = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch"
with open(sch_path, 'r', encoding='utf-8') as f:
    text = f.read()

# Let's find the symbol block for C22
c22_pos = text.find('(property "Reference" "C22"')
if c22_pos != -1:
    print("--- C22 block ---")
    start = text.rfind('(symbol', 0, c22_pos)
    end = text.find(')\n  )', c22_pos)
    print(text[start:start+1000])

# Let's find wires or labels connected near C22 coordinates
