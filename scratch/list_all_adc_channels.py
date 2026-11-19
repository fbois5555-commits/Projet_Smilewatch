import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

# Let's list all 16 ADC channels of STM32WB55:
# ADC1_IN1 to ADC1_IN16
for ch in range(1, 17):
    pattern = rf'\(name\s+\"([^\"]+)\"[\s\S]*?\(number\s+\"([^\"]+)\"[\s\S]*?\(alternate\s+\"ADC1_IN{ch}\"'
    m = re.search(pattern, mcu)
    if m:
        pname, pnum = m.groups()
        print(f"ADC1_IN{ch:2}: Pin {pnum:4} ({pname})")
    else:
        # try reverse order (alternate before number)
        pattern2 = rf'\(alternate\s+\"ADC1_IN{ch}\"[\s\S]*?\(number\s+\"([^\"]+)\"'
        m2 = re.search(pattern2, mcu)
        if m2:
            print(f"ADC1_IN{ch:2}: Pin {m2.group(1)}")
        else:
            print(f"ADC1_IN{ch:2}: Not found")
