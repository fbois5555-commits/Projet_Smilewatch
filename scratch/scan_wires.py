with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Ecran.kicad_sch", "r", encoding="utf-8") as f:
    lines = f.readlines()

wires = []
i = 0
while i < len(lines):
    line = lines[i].strip()
    if line == "(wire":
        # collect wire
        pts_line = ""
        while i < len(lines) and "(stroke" not in lines[i]:
            pts_line += lines[i] + " "
            i += 1
        wires.append(pts_line)
    i += 1

print(f"Parsed {len(wires)} wires by line scanner.")
for w in wires:
    if any(k in w for k in ["208.28", "173.99", "154.94", "160.02", "168.91", "227.33", "245.11"]):
        print("MATCH:", w.strip())
