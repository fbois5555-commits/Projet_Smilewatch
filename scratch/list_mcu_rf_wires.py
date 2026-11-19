with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\MCU.kicad_sch", "r", encoding="utf-8") as f:
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

print(f"Total wires in MCU: {len(wires)}")
for w in sorted(wires):
    if any(k in w for k in ["15.24", "49.53", "60.96", "177.8", "167.64"]):
        print("RF WIRE:", w)
