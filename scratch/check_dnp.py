import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

for ref in ['R21', 'R24', 'R22']:
    idx = mcu.find(f'(property "Reference" "{ref}"')
    if idx != -1:
        start = mcu.rfind('(symbol', 0, idx)
        end = mcu.find('\n\t)', idx)
        block = mcu[start:end]
        dnp_m = re.search(r'\(dnp\s+(\w+)\)', block)
        in_bom_m = re.search(r'\(in_bom\s+(\w+)\)', block)
        val_m = re.search(r'\(property\s+\"Value\"\s+\"([^\"]*)\"', block)
        print(f"=== {ref} ===")
        print(f"  Value : {val_m.group(1) if val_m else '?'}")
        print(f"  dnp   : {dnp_m.group(1) if dnp_m else '?'}")
        print(f"  in_bom: {in_bom_m.group(1) if in_bom_m else '?'}")
