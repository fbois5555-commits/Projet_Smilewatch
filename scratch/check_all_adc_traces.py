import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

# Let's inspect all ADC pins on STM32WB55 and see if ANY of them already has a via or track attached:
# True ADC pins on STM32WB55:
# PA0 (G7), PA1 (H6 is PA7? Let's check PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9)
# PC0, PC1 (G8), PC2 (G9), PC3 (G10), PC4, PC5 (H5)

# Find all pads of U2 that have an ADC function
pins_mcu = re.findall(r'\(name\s+\"([^\"]+)\"[\s\S]*?\(number\s+\"([A-K]\d+)\"[\s\S]*?\(alternate\s+\"(ADC1_IN\d+)\"', mcu)

print("=== ALL ADC PINS ON U2 AND THEIR TRACES/VIAS ===")
for pname, pnum, adc_ch in pins_mcu:
    # check in PCB if there is a track starting/ending at this pad
    m_pad = re.search(r'\(pad\s+\"' + pnum + r'\"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)[\s\S]*?\(net\s+\"([^\"]+)\"\)', pcb)
    if m_pad:
        dx, dy, net = float(m_pad.group(1)), float(m_pad.group(2)), m_pad.group(3)
        ax = round(104.6025 - dx, 4)
        ay = round(68.3825 - dy, 4)
        
        # Check connected segments
        connected_tracks = []
        for sm in re.finditer(r'\(segment\s+\(start\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(end\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(width\s+([\d\.-]+)\)\s+\(layer\s+\"([^\"]+)\"\)\s+\(net\s+\"([^\"]+)\"\)', pcb):
            x1, y1, x2, y2, w, l, tnet = sm.groups()
            x1, y1, x2, y2 = map(float, [x1, y1, x2, y2])
            if (abs(x1 - ax) < 0.15 and abs(y1 - ay) < 0.15) or (abs(x2 - ax) < 0.15 and abs(y2 - ay) < 0.15):
                connected_tracks.append((l, w, (x1, y1), (x2, y2), tnet))
                
        print(f"Pin {pnum:4} ({pname:5}, {adc_ch:10}) at ({ax:8.4f}, {ay:8.4f}) net={net}")
        if connected_tracks:
            for ct in connected_tracks:
                print(f"    Track on {ct[0]} w={ct[1]} {ct[2]} -> {ct[3]} net={ct[4]}")
