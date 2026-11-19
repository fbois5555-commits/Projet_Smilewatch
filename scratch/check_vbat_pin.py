import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

# Check everything near (128.27, 21.59)
for m in re.finditer(r'\(symbol\s+\(lib_id\s+\"([^\"]+)\"\)\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)', mcu):
    lib, x, y = m.group(1), float(m.group(2)), float(m.group(3))
    if abs(x - 128.27) < 5 and abs(y - 21.59) < 5:
        print(f"Symbol {lib} at ({x}, {y})")

for m in re.finditer(r'\((?:label|hierarchical_label|global_label)\s+\"([^\"]+)\"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)', mcu):
    name, x, y = m.group(1), float(m.group(2)), float(m.group(3))
    if abs(x - 128.27) < 5 and abs(y - 21.59) < 5:
        print(f"Label {name} at ({x}, {y})")
