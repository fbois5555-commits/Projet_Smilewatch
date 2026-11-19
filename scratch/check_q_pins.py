with open('Kicad/Carte_V2/Ecran.kicad_sch', 'r', encoding='utf-8') as f:
    c = f.read()

idx = c.find('(symbol "FDN338P:FDN338P"')
idx2 = c.find('(pin ', idx)
print(c[idx2:idx2+1500])
