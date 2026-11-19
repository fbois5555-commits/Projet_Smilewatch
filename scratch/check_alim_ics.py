import re

with open('Kicad/Carte_V2/Alimentation_sch.kicad_sch', 'r', encoding='utf-8') as f:
    alim = f.read()

# Let's inspect all ICs and symbols in Alimentation_sch.kicad_sch
symbols = re.findall(r'\(symbol\s+[\s\S]*?\(property\s+\"Reference\"\s+\"([^\"]+)\"[\s\S]*?\(property\s+\"Value\"\s+\"([^\"]*)\"', alim)
print("=== SYMBOLS IN Alimentation_sch.kicad_sch ===")
for ref, val in sorted(symbols):
    print(f"  {ref:12} : {val}")
