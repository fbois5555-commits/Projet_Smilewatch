with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Ecran.kicad_sch", "r", encoding="utf-8") as f:
    lines = f.readlines()

wires = []
i = 0
while i < len(lines):
    line = lines[i].strip()
    if line == "(wire":
        pts = ""
        while i < len(lines) and "(stroke" not in lines[i]:
            if "(xy " in lines[i]:
                pts += lines[i].strip() + " "
            i += 1
        wires.append(pts)
    i += 1

print(f"Total wires: {len(wires)}")
for w in sorted(wires):
    print(w)
