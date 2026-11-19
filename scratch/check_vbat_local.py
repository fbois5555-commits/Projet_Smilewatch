with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\MCU.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

for line in text.splitlines():
    if any(k in line for k in ["128.27", "88.90"]):
        print(line)
