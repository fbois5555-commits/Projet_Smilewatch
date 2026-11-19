import re

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

# All 16 ADC pins:
adc_list = [
    ('F8', 'PC0', 'ADC1_IN1'),
    ('G8', 'PC1', 'ADC1_IN2'),
    ('G9', 'PC2', 'ADC1_IN3'),
    ('G10', 'PC3', 'ADC1_IN4'),
    ('G7', 'PA0', 'ADC1_IN5'),
    ('G6', 'PA1', 'ADC1_IN6'),
    ('F6', 'PA2', 'ADC1_IN7'),
    ('J8', 'PA3', 'ADC1_IN8'),
    ('K10', 'PA4', 'ADC1_IN9'),
    ('K9', 'PA5', 'ADC1_IN10'),
    ('H7', 'PA6', 'ADC1_IN11'),
    ('H6', 'PA7', 'ADC1_IN12'),
    ('G4', 'PC4', 'ADC1_IN13'),
    ('H5', 'PC5', 'ADC1_IN14'),
    ('J7', 'PA8', 'ADC1_IN15'),
    ('K8', 'PA9', 'ADC1_IN16')
]

for pnum, pname, adc_ch in adc_list:
    m = re.search(r'\(pad\s+\"' + pnum + r'\"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)[\s\S]*?\(net\s+\"([^\"]+)\"\)', pcb)
    if m:
        dx, dy, net = float(m.group(1)), float(m.group(2)), m.group(3)
        ax = round(104.6025 - dx, 4)
        ay = round(68.3825 - dy, 4)
        print(f"{pnum:4} ({pname:5}, {adc_ch:10}) at ({ax:8.4f}, {ay:8.4f}) -> net={net}")
