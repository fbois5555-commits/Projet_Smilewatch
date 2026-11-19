import re

with open('Kicad/Carte_V2/Carte_V2.kicad_pcb', 'r', encoding='utf-8') as f:
    pcb = f.read()

# U2 center is at (104.6025, 68.3825)
# Let's find all footprints positioned near U2 (within 10mm)
fps = re.findall(r'\(footprint\s+\"([^\"]+)\"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)(?:\s+([\d\.-]+))?\)[\s\S]*?\(property\s+\"Reference\"\s+\"([^\"]+)\"', pcb)

print("Footprints near U2:")
for fp_lib, x, y, rot, ref in fps:
    x, y = float(x), float(y)
    if abs(x - 104.6) < 8 and abs(y - 68.4) < 8:
        print(f"  {ref:10} at ({x:7.2f}, {y:7.2f}) : {fp_lib}")
