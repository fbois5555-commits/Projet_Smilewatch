with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\MCU.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

# find (pin in lib definition
lib_idx = text.find('STM32WB55VGYx"')
# end of symbol is when next root symbol begins
lib_end = text.find('\n\t\t(symbol "', lib_idx)

pins = re.findall(r'\(pin\s+(\w+)\s+\w+[\s\S]*?\(name\s+"([^"]*)"[^\)]*\)\s+\(number\s+"([^"]*)"', text[lib_idx:lib_end])
print(f"Total pins found: {len(pins)}")
for ptype, pname, pnum in sorted(pins, key=lambda x: x[2]):
    print(f"Ball {pnum:4s}: {pname:25s} ({ptype})")
