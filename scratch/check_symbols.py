import re

with open('Kicad/Carte_V2/Ecran.kicad_sch', 'r', encoding='utf-8') as f:
    c = f.read()

# Symbols
for m in re.finditer(r'\(symbol\s+\(lib_id\s+\"([^\"]+)\"\)\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)[\s\S]*?\(property\s+\"Reference\"\s+\"([^\"]+)\"[\s\S]*?\(property\s+\"Value\"\s+\"([^\"]+)\"', c):
    lib, x, y, ref, val = m.groups()
    x, y = float(x), float(y)
    if 140 <= x <= 240 and 40 <= y <= 100:
        print(f'{ref} ({val}, {lib}) at ({x}, {y})')

# Labels
for m in re.finditer(r'\((?:label|hierarchical_label|global_label)\s+\"([^\"]+)\"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)', c):
    name, x, y = m.groups()
    x, y = float(x), float(y)
    if 140 <= x <= 240 and 40 <= y <= 100:
        print(f'Label: {name} at ({x}, {y})')
