with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\MCU.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

# Find STM32WB55 instance pins and pin names
idx = text.find('(property "Reference" "IC4"')
if idx == -1:
    idx = text.find('STM32WB55')

# Find the instance block
start = text.rfind('(symbol', 0, idx)
end = text.find('\n\t)', idx)
inst = text[start:end]

# Find all pin mappings or pin uuids
# Also let's find the symbol definition in lib_symbols
sym_def_idx = text.find('(symbol "STM32WB55')
if sym_def_idx != -1:
    sym_end = text.find('(symbol "', sym_def_idx + 10)
    sym_def = text[sym_def_idx:sym_end]
    pins = re.findall(r'\(pin\s+(\w+)\s+(\w+)\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)\s+([\d\.-]+)\)[^\)]*\(name\s+"([^"]*)"[^\)]*\)\s+\(number\s+"([^"]*)"', sym_def)
    print(f"Found {len(pins)} pins in MCU definition:")
    for p in pins:
        print(f"Pin {p[6]:5s} ({p[5]:15s}) at ({p[2]}, {p[3]}) type={p[0]}")
