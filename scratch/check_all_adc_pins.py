import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

# Let's find all ADC pins on U2 from MCU.kicad_sch
# Find all (pin ... (name "P...") ... (number "...") with "ADC1_IN"
pins = re.findall(r'\(pin\s+[\s\S]*?\(name\s+\"([^\"]+)\"[\s\S]*?\(number\s+\"([^\"]+)\"[\s\S]*?\(alternate\s+\"([^\"]*ADC[^\"]*)\"', mcu)

print(f"Total ADC alternate pins found: {len(pins)}")
for name, num, alt in pins:
    # check what this pin is connected to on PCB
    pad_m = re.search(r'\(pad\s+\"' + num + r'\"[\s\S]*?\(net\s+\"([^\"]+)\"\)', pcb)
    net_on_pcb = pad_m.group(1) if pad_m else "NOT FOUND"
    print(f"Pin {num:4} ({name:5}, {alt:12}) -> PCB net: {net_on_pcb}")
