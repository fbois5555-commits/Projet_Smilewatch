with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\MCU.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

# Find U2 symbol instance
idx = text.find('(property "Reference" "U2"')
start = text.rfind('(symbol', 0, idx)
end = text.find('\n\t)', idx)
u2_inst = text[start:end]

# Find pins in lib_symbols for STM32WB55VGYx
lib_idx = text.find('(symbol "MCU_ST_STM32WB:STM32WB55VGYx"')
lib_end = text.find('(symbol "', lib_idx + 35)
lib_block = text[lib_idx:lib_end]

pins = re.findall(r'\(pin\s+\w+\s+\w+\s+\(at\s+[\d\.-]+\s+[\d\.-]+\s+[\d\.-]+\)\s+\(length\s+[\d\.-]+\)\s+\(name\s+"([^"]*)"[^\)]*\)\s+\(number\s+"([^"]*)"', lib_block)
print(f"Found {len(pins)} pins in U2 lib definition:")
for pname, pnum in pins:
    print(f"Pin {pnum:5s}: {pname}")
