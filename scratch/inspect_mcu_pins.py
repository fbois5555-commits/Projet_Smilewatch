with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\MCU.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

# Find STM32WB55 symbol
pos = text.find('STM32WB55')
if pos != -1:
    start = text.rfind('(symbol', 0, pos)
    print("MCU symbol found near", pos)

# Find all labels in MCU.kicad_sch
labels = re.findall(r'\((?:label|global_label|hierarchical_label)\s+"([^"]+)"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)', text)
print(f"Total labels in MCU: {len(labels)}")
for name, x, y in sorted(labels):
    print(f"MCU Label: {name:20s} at ({float(x):.2f}, {float(y):.2f})")
