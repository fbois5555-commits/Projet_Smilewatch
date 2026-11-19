import re

with open('Kicad/Carte_V2/Ecran.kicad_sch', 'r', encoding='utf-8') as f:
    c = f.read()

# C43 is at (184.15, 80.01) rot 180
# In C_Small, pin 1 is (0, 2.54), pin 2 is (0, -2.54)
# With rot 180: pin 1 is (0, -2.54), pin 2 is (0, 2.54)
# So at (184.15, 80.01):
# pin 1: (184.15, 80.01 - 2.54) = (184.15, 77.47)
# pin 2: (184.15, 80.01 + 2.54) = (184.15, 82.55)

# C44 is at (196.85, 80.01) rot 180
# pin 1: (196.85, 77.47)
# pin 2: (196.85, 82.55)

# Let's trace wires connecting to (184.15, 77.47) and (196.85, 77.47)
wires = re.findall(r'\(wire\s+\(pts\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s*\)', c)

for pt in [(184.15, 77.47), (196.85, 77.47), (184.15, 82.55), (196.85, 82.55)]:
    print(f"Connections to {pt}:")
    for w in wires:
        x1, y1, x2, y2 = map(float, w)
        if (abs(x1 - pt[0]) < 0.05 and abs(y1 - pt[1]) < 0.05) or (abs(x2 - pt[0]) < 0.05 and abs(y2 - pt[1]) < 0.05):
            print(f"  {w}")
