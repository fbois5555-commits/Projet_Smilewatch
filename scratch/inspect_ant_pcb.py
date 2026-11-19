with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re

# find ANT1 in pcb
pos = pcb.find('(property "Reference" "ANT1"')
if pos != -1:
    start = pcb.rfind('(footprint', 0, pos)
    end = pcb.find('\n\t(footprint', pos)
    ant_block = pcb[start:end]
    print("=== ANT1 Footprint ===")
    m_at = re.search(r'\(at\s+([\d\.-]+)\s+([\d\.-]+)\s*([\d\.-]*)', ant_block)
    m_layer = re.search(r'\(layer\s+"([^"]+)"', ant_block)
    print(f"Layer: {m_layer.group(1) if m_layer else '?'}, At: {m_at.group(0) if m_at else '?'}")
    # find pads
    for p in re.findall(r'\(pad\s+"([^"]+)"[^\)]*\(net\s+(\d+)\s+"([^"]+)"\)', ant_block):
        print(f"Pad {p[0]}: net {p[1]} ({p[2]})")

# Search for keepout / rule_area in pcb
rule_areas = re.findall(r'\((?:zone|rule_area)[\s\S]*?\n\t\)', pcb)
print(f"Total zones/rule_areas in pcb: {len(rule_areas)}")
for z in rule_areas:
    if "keepout" in z or "ANT" in z:
        print("Keepout/Rule area found:\n", z[:300])
