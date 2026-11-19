import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

for ref in ['C36', 'C50', 'Y1']:
    idx = mcu.find(f'(property "Reference" "{ref}"')
    if idx != -1:
        start = mcu.rfind('(symbol', 0, idx)
        end = mcu.find('\n\t)', idx)
        print(f"=== {ref} ===")
        print(mcu[start:end])
