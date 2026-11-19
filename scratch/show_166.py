with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

pos = text.find('(at 166.37 163.83')
start = text.rfind('(symbol', 0, pos)
end = text.find('\n\t)', pos)
print(text[start:end+4])
