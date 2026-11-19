with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re

# Find TP2 to TP13 coordinates to compute centroid
tp_coords = []
for tp in [f"TP{i}" for i in [2, 8, 9, 10, 11, 12, 13]]:
    m = re.search(r'\(property "Reference" "' + tp + r'"[\s\S]*?\(at ([\d\.-]+) ([\d\.-]+)', pcb)
    if not m:
        # try find footprint
        pos = pcb.find(f'"{tp}"')
        if pos != -1:
            start = pcb.rfind('(footprint', 0, pos)
            end = pcb.find('\n\t)', pos)
            m_at = re.search(r'\(at ([\d\.-]+) ([\d\.-]+)', pcb[start:end])
            if m_at:
                tp_coords.append((float(m_at.group(1)), float(m_at.group(2))))

print("TP coords count:", len(tp_coords))
if tp_coords:
    xs = [p[0] for p in tp_coords]
    ys = [p[1] for p in tp_coords]
    print(f"Centroid of TPs: ({sum(xs)/len(xs):.3f}, {sum(ys)/len(ys):.3f})")
    print(f"Min X={min(xs):.2f}, Max X={max(xs):.2f}, Min Y={min(ys):.2f}, Max Y={max(ys):.2f}")

# Find C22 pos
pos_c22 = pcb.find('"C22"')
if pos_c22 != -1:
    start = pcb.rfind('(footprint', 0, pos_c22)
    end = pcb.find('\n\t)', pos_c22)
    m_at = re.search(r'\(at ([\d\.-]+) ([\d\.-]+)', pcb[start:end])
    if m_at:
        print(f"C22 pos: {m_at.group(1)}, {m_at.group(2)}")
