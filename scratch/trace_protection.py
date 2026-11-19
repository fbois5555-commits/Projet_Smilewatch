import re

with open('Kicad/Carte_V2/Alimentation_sch.kicad_sch', 'r', encoding='utf-8') as f:
    alim = f.read()

# Let's inspect symbols around U4 (62.23, 161.29) and Q3 (124.46, 161.29)
# Print symbols in x in [10, 160], y in [100, 200]
print("Symbols in Protection Area:")
for m in re.finditer(r'\(symbol\s+\(lib_id\s+\"([^\"]+)\"\)\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)[\s\S]*?\(property\s+\"Reference\"\s+\"([^\"]+)\"[\s\S]*?\(property\s+\"Value\"\s+\"([^\"]+)\"', alim):
    lib, x, y, ref, val = m.groups()
    x, y = float(x), float(y)
    if 10 <= x <= 160 and 100 <= y <= 200:
        print(f"  {ref:8} ({val:15}) at ({x}, {y})")

print("\nPower symbols in Protection Area:")
for m in re.finditer(r'\(symbol\s+\(lib_id\s+\"power:([^\"]+)\"\)\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)', alim):
    pwr, x, y = m.groups()
    x, y = float(x), float(y)
    if 10 <= x <= 160 and 100 <= y <= 200:
        print(f"  Power {pwr:10} at ({x}, {y})")
