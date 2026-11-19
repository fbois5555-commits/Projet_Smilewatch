import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

# Let's find pin "VBAT" of STM32WB55
for m in re.finditer(r'\(name \"VBAT\"[\s\S]*?\(number \"([^\"]+)\"', mcu):
    print("VBAT pin number:", m.group(1))

# Let's check in symbol def what pin number is VBAT
# In WLCSP100, pin A10 is VBAT?
for line in mcu.splitlines():
    if 'VBAT' in line and any(k in line for k in ['pin', 'power', 'label', '+3V']):
        print(line[:120])
