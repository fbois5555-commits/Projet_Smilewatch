with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch", "r", encoding="utf-8") as f:
    lines = f.readlines()

for i, l in enumerate(lines):
    if "24.13" in l and "(xy" in l:
        # print wire
        for k in range(max(0, i-3), min(len(lines), i+6)):
            print(lines[k], end="")
        print("---")
