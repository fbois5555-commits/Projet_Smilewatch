with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re
import math

pad_x, pad_y = 111.506, 89.535

blocks = pcb.split('(footprint ')
for b in blocks[1:]:
    ref_m = re.search(r'\(property "Reference" "([^"]+)"', b)
    ref = ref_m.group(1) if ref_m else "?"
    layer_m = re.search(r'\(layer "([^"]+)"\)', b)
    layer = layer_m.group(1) if layer_m else "?"
    if layer == "B.Cu" and ref != "TP8":
        at_fp = re.search(r'\(at ([\d\.-]+) ([\d\.-]+)', b)
        if at_fp:
            fx, fy = float(at_fp.group(1)), float(at_fp.group(2))
            dist = math.hypot(pad_x - fx, pad_y - fy)
            if dist < 8.0:
                print(f"FP on B.Cu: {ref} at ({fx}, {fy}), dist={dist:.2f} mm")
