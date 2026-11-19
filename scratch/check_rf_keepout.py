with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re

# Find zones around ANT1 at (101.56, 62.53)
zone_blocks = re.findall(r'(\(zone[\s\S]*?\n\t\))', pcb)
print(f"Total zones: {len(zone_blocks)}")

for z in zone_blocks:
    if any(k in z for k in ["RF", "Keep", "keepout"]):
        m_name = re.search(r'\(name\s+"([^"]+)"\)', z)
        m_layers = re.search(r'\((?:layers?)\s+([^\)]+)\)', z)
        # get polygon pts
        pts = re.findall(r'\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)', z)
        # check if near ANT1
        xs = [float(p[0]) for p in pts]
        ys = [float(p[1]) for p in pts]
        if xs and ys:
            min_x, max_x = min(xs), max(xs)
            min_y, max_y = min(ys), max(ys)
            print(f"Zone name: {m_name.group(1) if m_name else 'unnamed':15s} | layers: {m_layers.group(1) if m_layers else '?'} | bbox: X=[{min_x:.2f}, {max_x:.2f}], Y=[{min_y:.2f}, {max_y:.2f}]")
