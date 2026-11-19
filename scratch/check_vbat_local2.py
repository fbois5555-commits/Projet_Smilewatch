with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\MCU.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

for m in [text.find('(xy 128.27 21.59)'), text.find('(at 88.90')]:
    if m != -1:
        print("--- MATCH ---")
        print(text[m-100:m+200])
