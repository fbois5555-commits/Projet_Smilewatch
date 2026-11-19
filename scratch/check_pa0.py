import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

# Pin G7 is PA0
# Let's find symbol instance U2 and pin G7 coordinates
idx = mcu.find('(pin "G7"')
print(mcu[idx-100:idx+400])

# Let's find wires connecting to U2 pin G7
# In symbol U2: at what coordinate is pin G7?
idx_sym = mcu.find('(symbol "STM32WB55VGYx:STM32WB55VGYx"')
idx_g7 = mcu.find('(name "PA0"', idx_sym)
print(mcu[idx_g7-200:idx_g7+300])
