import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

idx = mcu.find('(number "H1"')
end_pin = mcu.find('(pin ', idx)
print(mcu[idx-100:end_pin])
