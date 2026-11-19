import re

for sch in ["MCU.kicad_sch", "Carte_V2.kicad_sch"]:
    path = rf"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\{sch}"
    with open(path, "r", encoding="utf-8") as f:
        text = f.read()
    
    print(f"=== Symbols in {sch} ===")
    for ref in ["ANT1", "R21", "R24", "Y1", "C21", "C23", "L1", "L2"]:
        pos = text.find(f'(property "Reference" "{ref}"')
        if pos != -1:
            start = text.rfind('(symbol', 0, pos)
            end = text.find('\n\t)', pos)
            print(f"Ref {ref} found in {sch}:")
            # find value
            m_val = re.search(r'\(property "Value" "([^"]*)"', text[pos:pos+300])
            print(f"  Value: {m_val.group(1) if m_val else '?'}")
