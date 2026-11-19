with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

pos_c22 = pcb.find('(property "Reference" "C22"')
start_c22 = pcb.rfind('(footprint', 0, pos_c22)
end_c22 = pcb.find('\n\t(footprint', pos_c22)
print("=== C22 Footprint ===")
print(pcb[start_c22:start_c22+1200])

pos_tp8 = pcb.find('(property "Reference" "TP8"')
start_tp8 = pcb.rfind('(footprint', 0, pos_tp8)
end_tp8 = pcb.find('\n\t(footprint', pos_tp8)
print("=== TP8 Footprint ===")
print(pcb[start_tp8:start_tp8+1200])
