import re

with open('Kicad/Carte_V2/Ecran.kicad_sch', 'r', encoding='utf-8') as f:
    c = f.read()

# Let's check symbols around Q1 and Q2
print("=== SYMBOLS ===")
for m in re.finditer(r'\(symbol\s+\(lib_id\s+\"([^\"]+)\"\)\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)(?:\s+(\d+))?[\s\S]*?\(property\s+\"Reference\"\s+\"([^\"]+)\"[\s\S]*?\(property\s+\"Value\"\s+\"([^\"]+)\"', c):
    groups = m.groups()
    lib, x, y, rot, ref, val = groups[0], float(groups[1]), float(groups[2]), groups[3] or '0', groups[4], groups[5]
    if 140 <= x <= 260 and 35 <= y <= 105:
        print(f'{ref:10} {val:15} {lib:25} at ({x}, {y}) rot={rot}')

print("\n=== LABELS ===")
for m in re.finditer(r'\((?:label|hierarchical_label|global_label)\s+\"([^\"]+)\"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)', c):
    name, x, y = m.groups()
    x, y = float(x), float(y)
    if 140 <= x <= 260 and 35 <= y <= 105:
        print(f'Label: {name:15} at ({x}, {y})')

print("\n=== WIRES ===")
wires = re.findall(r'\(wire\s+\(pts\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s*\)', c)
for x1, y1, x2, y2 in wires:
    x1, y1, x2, y2 = map(float, [x1, y1, x2, y2])
    if (140 <= x1 <= 260 and 35 <= y1 <= 105) or (140 <= x2 <= 260 and 35 <= y2 <= 105):
        print(f'  ({x1}, {y1}) -> ({x2}, {y2})')
