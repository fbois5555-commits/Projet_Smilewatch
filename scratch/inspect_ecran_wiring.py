import re

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Ecran.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

# Let's inspect the pins of FDN338P and FDN335N in the symbol definitions of the lib:
# Find symbol definition of FDN338P and FDN335N
for name in ["FDN338P:FDN338P", "FDN335N:FDN335N"]:
    idx = text.find(f'(symbol "{name}"')
    if idx != -1:
        end = text.find('\n\t\t)', idx)
        print(f"=== Symbol def for {name} ===")
        print(text[idx:idx+800])

# Let's see the wires in Ecran.kicad_sch
wire_matches = re.findall(r'\(wire\s+\(pts\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s*\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\)', text)
print(f"Total wires in Ecran.kicad_sch: {len(wire_matches)}")
for x1, y1, x2, y2 in wire_matches:
    print(f"Wire ({x1}, {y1}) -> ({x2}, {y2})")

# Labels in Ecran.kicad_sch
for l in re.finditer(r'\((?:label|global_label|hierarchical_label)\s+"([^"]+)"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)', text):
    print(f"Label: {l.group(1)} at ({l.group(2)}, {l.group(3)})")
