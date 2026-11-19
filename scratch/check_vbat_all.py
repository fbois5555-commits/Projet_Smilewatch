with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

import re
matches = [m.start() for m in re.finditer(r'\"VBAT\"', mcu)]
print("Occurrences of VBAT in MCU.kicad_sch:", len(matches))
for pos in matches:
    print(mcu[pos-50:pos+100])
