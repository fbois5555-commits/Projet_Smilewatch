import re

sch_path = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Ecran.kicad_sch"
with open(sch_path, 'r', encoding='utf-8') as f:
    text = f.read()

print("--- Symbols in Ecran.kicad_sch ---")
symbols = re.findall(r'\(symbol\s+\(lib_id\s+"([^"]+)"\)[\s\S]*?\(property\s+"Reference"\s+"([^"]+)"[\s\S]*?\(property\s+"Value"\s+"([^"]*)"[\s\S]*?\(instances[\s\S]*?\n\t\)', text)

for lib, ref, val in symbols:
    print(f"Ref: {ref:8s} | Val: {val:15s} | Lib: {lib}")

for item in ["Q1", "Q2", "R13"]:
    pos = text.find(f'(property "Reference" "{item}"')
    if pos != -1:
        start = text.rfind('(symbol', 0, pos)
        end = text.find('\n\t)', pos)
        print(f"\n--- {item} block ---")
        print(text[start:end+4])
