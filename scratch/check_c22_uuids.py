import re

sch_path = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch"
pcb_path = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb"

with open(sch_path, 'r', encoding='utf-8') as f:
    sch = f.read()

with open(pcb_path, 'r', encoding='utf-8') as f:
    pcb = f.read()

print("=== SCHEMATIC C22 ===")
for m in re.finditer(r'\(symbol[\s\S]*?\(property "Reference" "C22"[\s\S]*?\n\t\)', sch):
    block = m.group(0)
    uuid_m = re.search(r'\(uuid "([^"]+)"\)', block)
    path_m = re.search(r'\(path "([^"]+)"', block)
    print("SCH Block uuid:", uuid_m.group(1) if uuid_m else "?")
    print("SCH Block path:", path_m.group(1) if path_m else "?")

print("\n=== PCB C22 ===")
for m in re.finditer(r'\(footprint[\s\S]*?\(property "Reference" "C22"[\s\S]*?\n\t\)', pcb):
    block = m.group(0)
    uuid_m = re.search(r'\(uuid "([^"]+)"\)', block)
    path_m = re.search(r'\(path "([^"]+)"', block)
    sheet_m = re.search(r'\(sheetfile "([^"]+)"', block)
    print("PCB Footprint uuid:", uuid_m.group(1) if uuid_m else "?")
    print("PCB Footprint path:", path_m.group(1) if path_m else "?")
    print("PCB Sheetfile:", sheet_m.group(1) if sheet_m else "?")
