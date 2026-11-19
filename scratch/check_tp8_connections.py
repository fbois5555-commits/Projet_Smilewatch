with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re

# find segments connected to TP8 pad at (111.506, 89.535)
pad_x, pad_y = 111.506, 89.535
segments = re.findall(r'\(segment\s+\(start ([\d\.-]+) ([\d\.-]+)\)\s+\(end ([\d\.-]+) ([\d\.-]+)\)\s+\(width ([\d\.-]+)\)\s+\(layer "([^"]+)"\)\s+\(net "([^"]+)"\)', pcb)

print("Tracks connected to TP8:")
for x1, y1, x2, y2, w, layer, net in segments:
    x1, y1, x2, y2 = float(x1), float(y1), float(x2), float(y2)
    if (abs(x1 - pad_x) < 0.1 and abs(y1 - pad_y) < 0.1) or (abs(x2 - pad_x) < 0.1 and abs(y2 - pad_y) < 0.1):
        print(f"Track: ({x1}, {y1}) -> ({x2}, {y2}) width={w} layer={layer} net={net}")

# check if pad connects to filled zone on B.Cu
zones = re.findall(r'\(zone[\s\S]*?\(layer "B\.Cu"\)[\s\S]*?\(net "GNDPWR"\)[\s\S]*?\n\t\)', pcb)
print(f"Found {len(zones)} GNDPWR zone(s) on B.Cu")
