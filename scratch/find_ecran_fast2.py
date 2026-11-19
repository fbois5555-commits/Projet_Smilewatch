with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re

blocks = pcb.split('(footprint ')
print(f"Total footprints: {len(blocks) - 1}")

for block in blocks[1:]:
    if 'Ecran.kicad_sch' in block or any(r in block for r in ['"Q1"', '"Q2"', '"R13"', '"C39"', '"C42"', '"C43"']):
        ref_m = re.search(r'\(property "Reference" "([^"]+)"', block)
        layer_m = re.search(r'\(layer "([^"]+)"\)', block)
        at_m = re.search(r'\(at ([\d\.-]+) ([\d\.-]+)(?: ([\d\.-]+))?', block)
        val_m = re.search(r'\(property "Value" "([^"]*)"', block)
        ref = ref_m.group(1) if ref_m else "?"
        layer = layer_m.group(1) if layer_m else "?"
        x = float(at_m.group(1)) if at_m else 0
        y = float(at_m.group(2)) if at_m else 0
        rot = at_m.group(3) if at_m and at_m.group(3) else "0"
        val = val_m.group(1) if val_m else ""
        print(f"Ref: {ref:6s} | Layer: {layer:6s} | at ({x:.2f}, {y:.2f}, rot={rot:5s}) | Val: {val}")
