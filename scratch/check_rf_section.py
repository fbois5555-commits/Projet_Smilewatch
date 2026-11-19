import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

# Let's find positions of FL1, ANT1, R21, R22, R24
for ref in ['FL1', 'ANT1', 'R21', 'R22', 'R24', 'Y1', 'Y2']:
    idx = mcu.find(f'(property "Reference" "{ref}"')
    if idx != -1:
        start = mcu.rfind('(symbol', 0, idx)
        end = mcu.find('\n\t)', idx)
        sym_block = mcu[start:end]
        at_m = re.search(r'\(at\s+([\d\.-]+)\s+([\d\.-]+)(?:\s+([\d\.-]+))?\)', sym_block)
        val_m = re.search(r'\(property\s+\"Value\"\s+\"([^\"]*)\"', sym_block)
        pos = at_m.groups() if at_m else ("?", "?")
        val = val_m.group(1) if val_m else "?"
        print(f"{ref:6} ({val:20}) at ({pos[0]}, {pos[1]})")

# Check wires around FL1 and ANT1
# Let's print all wires within the bounding box of FL1 / ANT1
