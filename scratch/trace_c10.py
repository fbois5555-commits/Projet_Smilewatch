import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

# Pin C10 definition:
# (at 27.94 48.26 180) ? Let's check (at ... ) for pin C10
idx = mcu.find('(number "C10"')
start = mcu.rfind('(pin', 0, idx)
end = mcu.find('\n\t\t\t\t)', idx)
print("C10 def:", mcu[start:end])

# U2 is at (138.43, 100.33)
# Pin C10 symbol pos:
m_at = re.search(r'\(at\s+([\d\.-]+)\s+([\d\.-]+)', mcu[start:idx])
px, py = float(m_at.group(1)), float(m_at.group(2))
# Schematic connection pt:
cx, cy = 138.43 + px, 100.33 - py
print(f"C10 schematic connection point: ({cx}, {cy})")

wires = re.findall(r'\(wire\s+\(pts\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s*\)', mcu)
for w in wires:
    x1, y1, x2, y2 = map(float, w)
    if (abs(x1 - cx) < 0.1 and abs(y1 - cy) < 0.1) or (abs(x2 - cx) < 0.1 and abs(y2 - cy) < 0.1):
        print("Connected wire:", w)

# Check labels/power at (cx, cy)
for m in re.finditer(r'\((?:label|hierarchical_label|global_label|symbol)\s+[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)', mcu):
    pass
