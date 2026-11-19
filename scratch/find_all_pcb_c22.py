pcb_path = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb"
with open(pcb_path, 'r', encoding='utf-8') as f:
    pcb = f.read()

import re

fps = pcb.split('(footprint ')
print(f"Total footprints: {len(fps)-1}")

for fp in fps[1:]:
    if '"C22"' in fp:
        ref_m = re.search(r'\(property "Reference" "([^"]+)"', fp)
        fp_name = fp[:fp.find('"')]
        sheet_m = re.search(r'\(sheetfile "([^"]+)"', fp)
        path_m = re.search(r'\(path "([^"]+)"', fp)
        at_m = re.search(r'\(at ([\d\.-]+) ([\d\.-]+)', fp)
        print("--- PCB FP with C22 ---")
        print(f"Ref: {ref_m.group(1) if ref_m else '?'}")
        print(f"Type: {fp_name}")
        print(f"Sheet: {sheet_m.group(1) if sheet_m else '?'}")
        print(f"Path: {path_m.group(1) if path_m else '?'}")
        print(f"At: {at_m.group(1) if at_m else '?'}, {at_m.group(2) if at_m else '?'}")
