with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Ecran.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

idx = text.find('(symbol "FDN338P_1_1"')
print(text[idx:idx+1000])

idx2 = text.find('(symbol "FDN335N_1_1"')
print(text[idx2:idx2+1000])
