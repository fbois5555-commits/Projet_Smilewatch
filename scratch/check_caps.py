import re

with open('Kicad/Carte_V2/Ecran.kicad_sch', 'r', encoding='utf-8') as f:
    c = f.read()

# Let's inspect C43 and C44
# In KiCad symbol C_Small:
# pin 1 is top/right, pin 2 is bottom/left (depends on orientation)
for ref in ['C43', 'C44', 'C42', 'C20', 'C45', 'C39']:
    idx = c.find(f'(property "Reference" "{ref}"')
    if idx != -1:
        start = c.rfind('(symbol', 0, idx)
        end = c.find('\n\t)', idx)
        print(f"=== {ref} ===")
        print(c[start:end+3])

# Check C39 connector pin connections
print("\n=== C39 connector pin labels/wires ===")
for m in re.finditer(r'\(pin\s+\"(\d+)\"\s+\(uuid\s+\"([^\"]+)\"\)\s*\)', c):
    pass
