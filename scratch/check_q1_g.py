import re

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

pattern = r'\(segment\s+\(start\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(end\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(width\s+([\d\.-]+)\)\s+\(layer\s+\"([^\"]+)\"\)\s+\(net\s+\"([^\"]+)\"\)'
for m in re.finditer(pattern, pcb):
    x1, y1, x2, y2, w, layer, net = m.groups()
    if 'Q1' in net or 'Q2' in net:
        print(f'{net:20} {layer:8} w={w} ({x1}, {y1}) -> ({x2}, {y2})')
