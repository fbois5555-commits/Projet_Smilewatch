import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

# Let's find every ADC1_INx
matches = re.finditer(r'\(name\s+\"([^\"]+)\"[\s\S]*?\(number\s+\"([^\"]+)\"[\s\S]*?\(alternate\s+\"(ADC1_IN\d+)\"', mcu)

adc_pins = []
seen = set()
for m in matches:
    pname, pnum, adc_ch = m.groups()
    if pnum in seen:
        continue
    seen.add(pnum)
    pad_m = re.search(r'\(pad\s+\"' + pnum + r'\"[\s\S]*?\(net\s+\"([^\"]+)\"\)', pcb)
    net = pad_m.group(1) if pad_m else "None"
    at_m = re.search(r'\(pad\s+\"' + pnum + r'\"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)\)', pcb)
    pos = at_m.groups() if at_m else ("?", "?")
    adc_pins.append((pnum, pname, adc_ch, net, pos))

print("\n=== TOUTES LES VRAIES BROCHES ADC DU STM32WB55 (WLCSP100) ===")
for pnum, pname, adc_ch, net, pos in sorted(adc_pins):
    free = "LIBRE" if "unconnected" in net else "UTILISÉ"
    print(f"Bille {pnum:4} ({pname:5}, {adc_ch:10}) offset ({pos[0]:>5}, {pos[1]:>5}) -> {free:7} : {net}")
