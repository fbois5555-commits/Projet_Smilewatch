import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

# Let's inspect wires around (10 to 110, 160 to 190)
wires = re.findall(r'\(wire\s+\(pts\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s*\)', mcu)

print("Wires in RF section (x in [10, 110], y in [160, 190]):")
for w in wires:
    x1, y1, x2, y2 = map(float, w)
    if (10 <= x1 <= 110 and 160 <= y1 <= 190) or (10 <= x2 <= 110 and 160 <= y2 <= 190):
        print(f"  ({x1}, {y1}) -> ({x2}, {y2})")

# Labels in RF section
for m in re.finditer(r'\((?:label|hierarchical_label|global_label)\s+\"([^\"]+)\"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)', mcu):
    name, x, y = m.group(1), float(m.group(2)), float(m.group(3))
    if 10 <= x <= 110 and 160 <= y <= 190:
        print(f"Label: {name} at ({x}, {y})")

# Power symbols in RF section
for m in re.finditer(r'\(symbol\s+\(lib_id\s+\"power:([^\"]+)\"\)\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)', mcu):
    name, x, y = m.group(1), float(m.group(2)), float(m.group(3))
    if 10 <= x <= 110 and 160 <= y <= 190:
        print(f"Power: {name} at ({x}, {y})")
