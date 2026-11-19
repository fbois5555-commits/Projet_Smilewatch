with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re

for tp in [f"TP{i}" for i in [2, 8, 9, 10, 11, 12, 13]]:
    pos = pcb.find(f'(property "Reference" "{tp}"')
    if pos != -1:
        start = pcb.rfind('(footprint', 0, pos)
        m_at = re.search(r'\(at ([\d\.-]+) ([\d\.-]+)', pcb[start:pos])
        if m_at:
            print(f"{tp}: at ({m_at.group(1)}, {m_at.group(2)})")
