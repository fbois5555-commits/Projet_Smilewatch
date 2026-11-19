import subprocess
import sys
sys.stdout.reconfigure(encoding='utf-8')

res = subprocess.run(["git", "diff", "Kicad/Carte_V2/Carte_V2.kicad_pcb"], capture_output=True, text=True, encoding='utf-8', errors='ignore')

# Search for TP8 or newly added footprints/pads/zones
lines = res.stdout.splitlines()
print(f"Total diff lines: {len(lines)}")
for i, line in enumerate(lines):
    if any(k in line for k in ["TP8", "TestPoint", "footprint", "GNDPWR"]):
        for j in range(max(0, i-2), min(len(lines), i+12)):
            print(lines[j])
        print("---")
