import os
import time

for f in ["Alimentation_sch.kicad_sch", "_autosave-Alimentation_sch.kicad_sch", "Carte_V2.kicad_sch", "Carte_V2.kicad_pcb"]:
    p = os.path.join(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2", f)
    if os.path.exists(p):
        mtime = os.path.getmtime(p)
        print(f"{f}: modified {time.ctime(mtime)}, size {os.path.getsize(p)}")
