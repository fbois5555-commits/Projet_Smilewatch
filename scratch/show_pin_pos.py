with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Ecran.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

idx = text.find('(symbol "FDN338P_1_1"')
end = text.find(')\n\t\t)', idx)
sub = text[idx:end]
for line in sub.splitlines():
    if "pin " in line or "(at " in line:
        print(line)
