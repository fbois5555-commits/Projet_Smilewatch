import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

# Let's search for pin H1 in MCU.kicad_sch
idx = mcu.find('(number "H1"')
if idx != -1:
    start = mcu.rfind('(pin', 0, idx)
    end = mcu.find('\n\t\t\t\t)', idx)
    print("=== Pin H1 in MCU.kicad_sch ===")
    print(mcu[start:end])
