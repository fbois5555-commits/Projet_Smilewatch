with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re

# Find display connector C39 in PCB
pos = pcb.find('(property "Reference" "C39"')
if pos != -1:
    start = pcb.rfind('(footprint', 0, pos)
    end = pcb.find('\n\t(footprint', pos)
    print("=== Footprint C39 ===")
    m_at = re.search(r'\(at\s+([\d\.-]+)\s+([\d\.-]+)\s*([\d\.-]*)', pcb[start:end])
    print(m_at.group(0) if m_at else "No at")

# Find OLED or Ecran footprint
for fp in ["Ecran", "OLED", "DISP", "QG-8848", "Cadran"]:
    idx = pcb.find(fp)
    if idx != -1:
        print(f"Found mention of {fp} in PCB near pos {idx}")
