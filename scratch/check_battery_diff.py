import subprocess
import sys
sys.stdout.reconfigure(encoding='utf-8')

# Let's inspect Alimentation_sch diff specifically for battery / C22
res = subprocess.run(["git", "diff", "Kicad/Carte_V2/Alimentation_sch.kicad_sch"], capture_output=True, text=True, encoding='utf-8', errors='ignore')

for block in res.stdout.split("@@"):
    if any(k in block for k in ["C22", "BT", "S1921", "TP", "+3V8", "GNDPWR"]):
        print("=== BLOCK ===")
        print(block[:1200])
