with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb_text = f.read()

import re

# Find footprint for C22 and TP2..TP13
fps = ["C22"] + [f"TP{i}" for i in [2,3,4,5,8,9,10,11,12,13]]

for fp in fps:
    # search for footprint block containing reference
    m = re.search(r'\(footprint\s+"([^"]+)"[\s\S]*?\(property\s+"Reference"\s+"' + fp + r'"[\s\S]*?\n\t\)', pcb_text)
    if m:
        block = m.group(0)
        at_m = re.search(r'\(at\s+([\d\.-]+)\s+([\d\.-]+)', block)
        layer_m = re.search(r'\(layer\s+"([^"]+)"\)', block)
        # find pads in this block
        pads = re.findall(r'\(pad\s+"([^"]+)"\s+(\w+)\s+(\w+)\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)[^\)]*\)\s+\(size\s+([\d\.-]+)\s+([\d\.-]+)\)[^\)]*\(net\s+(\d+)\s+"([^"]+)"\)', block)
        print(f"FP: {fp:5s} | at {at_m.group(1) if at_m else '?'},{at_m.group(2) if at_m else '?'} | layer {layer_m.group(1) if layer_m else '?'} | pads: {[(p[0], p[8]) for p in pads]}")
