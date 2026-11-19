import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

# Let's find wires connected to pin C10
# In symbol STM32WB55VGYx, what is the position of C10?
idx = mcu.find('(number "C10"')
print(mcu[idx-100:idx+200])
