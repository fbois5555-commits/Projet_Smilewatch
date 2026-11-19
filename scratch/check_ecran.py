with open('Kicad/Carte_V2/Ecran.kicad_sch', 'r', encoding='utf-8') as f:
    c = f.read()

# Pin locations for Q2 (FDN335N) at (173.99, 67.31)
# In symbol def:
# Pin 1 (G): (0, 0) -> (173.99, 67.31)
# Pin 2 (S): (7.62, -5.08) rotated? Wait, symbol rotation is 0.
# Let's check pins of Q2 and Q1.
import re
wires = re.findall(r'\(wire\s+\(pts\s+\(xy\s+([\d\.]+)\s+([\d\.]+)\)\s+\(xy\s+([\d\.]+)\s+([\d\.]+)\)\)', c)

def find_connected(x, y, tol=0.1):
    found = []
    for w in wires:
        x1, y1, x2, y2 = map(float, w)
        if abs(x1 - x) < tol and abs(y1 - y) < tol:
            found.append((x2, y2))
        elif abs(x2 - x) < tol and abs(y2 - y) < tol:
            found.append((x1, y1))
    return found

print("All wires in x in [160, 240], y in [40, 100]:")
for w in wires:
    x1, y1, x2, y2 = map(float, w)
    if (160 <= x1 <= 240 and 40 <= y1 <= 100) or (160 <= x2 <= 240 and 40 <= y2 <= 100):
        print(f"  ({x1}, {y1}) -> ({x2}, {y2})")
