# Let's inspect positions.csv or bom.csv in Carte_V2
with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\positions.csv", "r", encoding="utf-8") as f:
    text = f.read()

for line in text.splitlines():
    if any(k in line for k in ["C22", "TP", "BT", "IC2"]):
        print(line)
