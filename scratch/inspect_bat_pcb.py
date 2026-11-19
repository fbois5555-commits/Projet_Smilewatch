with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re

# Find C22
pos_c22 = pcb.find('(property "Reference" "C22"')
start_c22 = pcb.rfind('(footprint', 0, pos_c22)
end_c22 = pcb.find('\n\t)', pos_c22)
block_c22 = pcb[start_c22:end_c22+2]

at_c22 = re.search(r'\(at ([\d\.-]+) ([\d\.-]+)(?: ([\d\.-]+))?', block_c22)
pad_c22 = re.search(r'\(pad "1"[^\)]*\(net "([^"]+)"\)', block_c22)
print("C22:", at_c22.groups() if at_c22 else "no at", "net:", pad_c22.group(1) if pad_c22 else "no net")

# Find TP8
pos_tp8 = pcb.find('(property "Reference" "TP8"')
start_tp8 = pcb.rfind('(footprint', 0, pos_tp8)
end_tp8 = pcb.find('\n\t)', pos_tp8)
block_tp8 = pcb[start_tp8:end_tp8+2]

at_tp8 = re.search(r'\(at ([\d\.-]+) ([\d\.-]+)(?: ([\d\.-]+))?', block_tp8)
pad_tp8 = re.search(r'\(pad "1"[^\)]*\(net "([^"]+)"\)', block_tp8)
print("TP8:", at_tp8.groups() if at_tp8 else "no at", "net:", pad_tp8.group(1) if pad_tp8 else "no net")

# Check zones on B.Cu
zones = re.findall(r'\(zone[\s\S]*?\(layer "B\.Cu"\)[\s\S]*?\(net "([^"]+)"\)[\s\S]*?\n\t\)', pcb)
print("B.Cu zones nets:", zones)
