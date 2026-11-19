import subprocess

for sch in ["Ecran.kicad_sch", "MCU.kicad_sch", "Capteurs.kicad_sch", "Moteur.kicad_sch", "Carte_V2.kicad_sch"]:
    res = subprocess.run(["git", "diff", f"Kicad/Carte_V2/{sch}"], capture_output=True, text=True)
    lines = res.stdout.splitlines()
    print(f"{sch}: {len(lines)} diff lines")
