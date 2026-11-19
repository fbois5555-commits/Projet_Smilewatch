with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

import re
idx_start = mcu.find('(symbol "MCU_ST_STM32WB:STM32WB55VGYx"')
if idx_start == -1:
    idx_start = mcu.find('STM32WB55VGYx')
idx_end = mcu.find('(symbol "2450AT07A0100001T', idx_start)
sym = mcu[idx_start:idx_end] if idx_end != -1 else mcu[idx_start:idx_start+50000]

print("Symbol slice length:", len(sym))

# Find all pins in sym:
# Each pin has: (pin ... (name "...") ... (number "...") ... )
pins = re.findall(r'\(pin\s+\w+\s+\w+\s+[\s\S]*?\(name\s+\"([^\"]+)\"[\s\S]*?\(number\s+\"([^\"]+)\"[\s\S]*?\n\t\t\t\t\)', sym)
print("Total pins in STM32WB55:", len(pins))

for ptext in sym.split('\n\t\t\t\t(pin '):
    m_alt = re.findall(r'\"(ADC1_IN\d+)\"', ptext)
    m_num = re.search(r'\(number\s+\"([A-K]\d+)\"', ptext)
    m_name = re.search(r'\(name\s+\"([^\"]+)\"', ptext)
    if m_alt and m_num and m_name:
        print(f"{m_alt[0]:10} -> Pin {m_num.group(1):4} ({m_name.group(1)})")
