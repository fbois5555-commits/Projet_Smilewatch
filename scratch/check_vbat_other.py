import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

# Check symbols, wires near (88.9, 33.02)
for m in re.finditer(r'\(symbol\s+\(lib_id\s+\"([^\"]+)\"\)\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)', mcu):
    lib, x, y = m.group(1), float(m.group(2)), float(m.group(3))
    if abs(x - 88.9) < 15 and abs(y - 33.02) < 15:
        print(f"Symbol {lib} at ({x}, {y})")

wires = re.findall(r'\(wire\s+\(pts\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s*\)', mcu)
for w in wires:
    x1, y1, x2, y2 = map(float, w)
    if (abs(x1 - 88.9) < 1 and abs(y1 - 33.02) < 1) or (abs(x2 - 88.9) < 1 and abs(y2 - 33.02) < 1):
        print("Wire to (88.9, 33.02):", w)
