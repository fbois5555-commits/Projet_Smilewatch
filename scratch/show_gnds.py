with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

for line in text.splitlines():
    if "GNDPWR" in line or "GND" in line and "symbol" in line:
        print(line[:120])
