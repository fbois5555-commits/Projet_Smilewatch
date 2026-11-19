import re

with open('Kicad/Carte_V2/Alimentation_sch.kicad_sch', 'r', encoding='utf-8') as f:
    alim = f.read()

# Let's inspect connections around Q3 and U4
# Q3 is DMN2028UFU (dual N-MOSFET)
# U4 is AP9101C (battery protection IC)
for line in alim.splitlines():
    if any(k in line for k in ['GNDPWR', 'AP9101', 'DMN2028', 'TP8', 'C22']) and ('label' in line or 'property' in line):
        print(line[:120])
