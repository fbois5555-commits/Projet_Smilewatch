sch_path = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch"
with open(sch_path, 'r', encoding='utf-8') as f:
    text = f.read()

import re

# Find wires connected near 26.67 113.03
wire_pat = r'\(wire\s+\(pts\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\)\s+\(stroke[^)]+\)\s+\(uuid\s+"[^"]+"\)\s*\)'
wires = list(re.finditer(wire_pat, text))

print(f"Total wires: {len(wires)}")
for w in wires:
    x1, y1, x2, y2 = map(float, [w.group(1), w.group(2), w.group(3), w.group(4)])
    # Check if close to (26.67, 113.03)
    if (abs(x1 - 26.67) < 15 and abs(y1 - 113.03) < 15) or (abs(x2 - 26.67) < 15 and abs(y2 - 113.03) < 15):
        print(f"Wire: ({x1}, {y1}) -> ({x2}, {y2})")

# Also look for labels and power symbols near (26.67, 113.03)
label_pat = r'\((?:label|global_label|hierarchical_label)\s+"([^"]+)"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)'
for l in re.finditer(label_pat, text):
    name, lx, ly = l.group(1), float(l.group(2)), float(l.group(3))
    if abs(lx - 26.67) < 30 and abs(ly - 113.03) < 30:
        print(f"Label: {name} at ({lx}, {ly})")

# Look for TP2 to TP13 coordinates and connections
for tp in ["TP2", "TP3", "TP4", "TP5", "TP8", "TP9", "TP10", "TP11", "TP12", "TP13"]:
    pos = text.find(f'(property "Reference" "{tp}"')
    if pos != -1:
        start = text.rfind('(symbol', 0, pos)
        end = text.find(')\n\t)', pos)
        block = text[start:end+4]
        m_at = re.search(r'\(at\s+([\d\.-]+)\s+([\d\.-]+)', block)
        if m_at:
            print(f"{tp} at {m_at.group(1)}, {m_at.group(2)}")
