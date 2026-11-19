with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

pos = pcb.find('(property "Reference" "TP2"')
start = pcb.rfind('(footprint', 0, pos)
end = pcb.find('\n\t)', pos)
print(pcb[start:pos+100])
