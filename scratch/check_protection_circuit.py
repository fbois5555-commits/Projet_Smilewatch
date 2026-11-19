import re

with open('Kicad/Carte_V2/Alimentation_sch.kicad_sch', 'r', encoding='utf-8') as f:
    alim = f.read()

# Let's inspect U4 (AP9101C) and Q3 (DMN2028UFU)
for ref in ['U4', 'Q3', 'C22', 'TP8']:
    idx = alim.find(f'(property "Reference" "{ref}"')
    if idx != -1:
        start = alim.rfind('(symbol', 0, idx)
        end = alim.find('\n\t)', idx)
        print(f"=== {ref} ===")
        print(alim[start:end])
