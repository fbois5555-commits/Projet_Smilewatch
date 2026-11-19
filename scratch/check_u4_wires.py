with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

# Let's see the wires around U4 at (62.23, 161.29) and Q3 at (124.46, 161.29)
lines = text.splitlines()
for i, l in enumerate(lines):
    if any(k in l for k in ["161.29", "153.67", "166.37", "148.59"]):
        print(f"Line {i}: {l}")
