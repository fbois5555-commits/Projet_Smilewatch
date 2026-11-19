# Let's inspect symbol FDN338P in Ecran.kicad_sch
with open('Kicad/Carte_V2/Ecran.kicad_sch', 'r', encoding='utf-8') as f:
    c = f.read()

idx = c.find('(property "Reference" "Q1"')
print(c[idx-200:idx+800])
