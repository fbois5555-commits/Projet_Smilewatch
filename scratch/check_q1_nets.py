# Trace nets for components
import re

with open('Kicad/Carte_V2/Ecran.kicad_sch', 'r', encoding='utf-8') as f:
    c = f.read()

# Let's inspect the pins of Q1 and Q2 in detail
# Q2: FDN335N at (173.99, 67.31) rot 0
# Pin 1 (G): (173.99, 67.31)
# Pin 2 (S): (173.99 + 7.62, 67.31 - (-5.08)) = (181.61, 72.39)
# Pin 3 (D): (173.99 + 7.62, 67.31 - 10.16) = (181.61, 57.15)

# Q1: FDN338P at (208.28, 55.88) rot 0 (Wait, it moved from 59.69 to 55.88!)
# Pin 1 (G): (208.28, 55.88)
# Pin 2 (S): (208.28 + 7.62, 55.88 - (-5.08)) = (215.90, 60.96) ??? Or let's check!
# Pin 3 (D): (208.28 + 7.62, 55.88 - 10.16) = (215.90, 45.72) ??? Wait, let's check wire coordinates around Q1!

wires = re.findall(r'\(wire\s+\(pts\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s*\)', c)

print("Wires connecting to Q1 pins:")
for w in wires:
    x1, y1, x2, y2 = map(float, w)
    if (abs(x1 - 208.28) < 0.1 or abs(x2 - 208.28) < 0.1 or 
        abs(x1 - 215.9) < 0.1 or abs(x2 - 215.9) < 0.1):
        if 40 <= y1 <= 75 and 40 <= y2 <= 75:
            print(f"  ({x1}, {y1}) -> ({x2}, {y2})")

# Let's check symbol definition for FDN338P in the file again to be 100% sure of pin coordinates
idx = c.find('(symbol "FDN338P:FDN338P"')
idx_pin = c.find('(pin ', idx)
end_sym = c.find('(symbol "', idx_pin)
print("\nFDN338P symbol pins:")
print(c[idx_pin:end_sym])
