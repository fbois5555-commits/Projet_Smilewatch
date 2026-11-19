with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

idx = mcu.find('at 128.27 21.59')
start = mcu.rfind('(', 0, idx)
start2 = mcu.rfind('(', 0, start - 1)
print(mcu[start2:idx+50])
