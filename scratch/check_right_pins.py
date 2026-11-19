import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

# All pins of U2 with their coordinates on PCB
pads = re.findall(r'\(pad\s+\"([A-K]\d+)\"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)[\s\S]*?\(net\s+\"([^\"]+)\"\)', pcb)

print("=== PINS ON THE RIGHT SIDE OF U2 (X > 105.0) ===")
for pnum, dx, dy, net in pads:
    ax = round(104.6025 - float(dx), 4)
    ay = round(68.3825 - float(dy), 4)
    if ax > 105.0:
        # check pin name and adc in sch
        m = re.search(r'\(name\s+\"([^\"]+)\"[\s\S]*?\(number\s+\"' + pnum + r'\"', mcu)
        pname = m.group(1) if m else "?"
        # check if ADC
        m_adc = re.search(r'\(number\s+\"' + pnum + r'\"[\s\S]*?\(alternate\s+\"(ADC1_IN\d+)\"', mcu)
        adc = m_adc.group(1) if m_adc else "No ADC"
        free = "LIBRE" if "unconnected" in net else "UTILISÉ"
        print(f"Bille {pnum:4} ({pname:6}, {adc:10}) at ({ax:8.4f}, {ay:8.4f}) -> {free:7} : {net}")
