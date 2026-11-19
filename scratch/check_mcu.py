import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    c = f.read()

for ref in ['R21', 'R24', 'Y1', 'PA0']:
    print(f"=== {ref} ===")
    for m in re.finditer(r'\(symbol[\s\S]*?\(property\s+\"Reference\"\s+\"' + ref + r'\"[\s\S]*?\)\s*\)', c):
        print(m.group(0)[:400])

# Check PA0 labels
for m in re.finditer(r'\((?:label|hierarchical_label|global_label)\s+\"([^\"]*PA0[^\"]*)\"', c):
    print("PA0 label:", m.group(1))
