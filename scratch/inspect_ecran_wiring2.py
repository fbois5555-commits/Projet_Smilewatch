import re

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Ecran.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

pattern = re.compile(r'\(wire\s*\(pts\s*\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s*\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\)', re.DOTALL)
wires = pattern.findall(text)
print(f"Total wires in Ecran: {len(wires)}")

for x1, y1, x2, y2 in wires:
    x1, y1, x2, y2 = float(x1), float(y1), float(x2), float(y2)
    if (150 <= x1 <= 230 and 45 <= y1 <= 80) or (150 <= x2 <= 230 and 45 <= y2 <= 80):
        print(f"Wire: ({x1:.2f}, {y1:.2f}) -> ({x2:.2f}, {y2:.2f})")

# Let's see the lib_symbols for FDN338P and FDN335N
for name in ["FDN338P", "FDN335N"]:
    start = text.find(f'(symbol "{name}:{name}"')
    if start != -1:
        end = text.find('\n\t\t)', start)
        sub = text[start:end]
        print(f"\nSub-block for {name}:")
        for line in sub.splitlines():
            if "(pin " in line or "(name " in line or "(number " in line:
                print(line)
