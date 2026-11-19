with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Carte_V2.kicad_pcb", "r", encoding="utf-8") as f:
    pcb = f.read()

import re

for uuid in ["0188df7f-8b77-4407-81bf-613ed1c8494d", "aab20941-b179-4d2c-b2a5-6f8e22e1ef63", "437cacef-c387-48b0-8a0d-5e8e3c5e13a9", "915afaf9-fa3a-4939-a202-72e77992a0c3"]:
    pos = pcb.find(uuid)
    if pos != -1:
        start = pcb.rfind('(zone', 0, pos)
        end = pcb.find('\n\t)', pos)
        print(f"=== Zone {uuid} ===")
        print(pcb[start:end+4])
