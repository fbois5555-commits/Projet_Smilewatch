import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

# Let's search for (number "G7" in the whole file
idx = 0
while True:
    idx = mcu.find('(number "G7"', idx)
    if idx == -1:
        break
    start = max(0, idx - 200)
    end = min(len(mcu), idx + 200)
    print("=== Found G7 ===")
    print(mcu[start:end])
    idx += 12
