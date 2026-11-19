import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

idx = mcu.find('(property "Reference" "U2"')
start = mcu.rfind('(symbol', 0, idx)
print("U2 placement line:")
print(mcu[start:start+120])

# Look for pin G7 in U2 instance
# Symbol position is (at X Y rot)
at_m = re.search(r'\(at\s+([\d\.-]+)\s+([\d\.-]+)', mcu[start:idx])
ux, uy = float(at_m.group(1)), float(at_m.group(2))
# In symbol def, pin G7 is at (27.94, 63.5, 180) -> connection point is at (ux + 27.94, uy - 63.5) or (ux - 27.94, uy + 63.5) etc.
print(f"U2 is at ({ux}, {uy})")

wires = re.findall(r'\(wire\s+\(pts\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s*\)', mcu)
no_connects = re.findall(r'\(no_connect\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)\)', mcu)

print(f"Total wires in MCU: {len(wires)}")
print(f"Total no_connects: {len(no_connects)}")

# Check no_connects near U2
for nx, ny in no_connects:
    nx, ny = float(nx), float(ny)
    print(f"No connect at ({nx}, {ny})")
