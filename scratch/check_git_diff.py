import subprocess
import sys
sys.stdout.reconfigure(encoding='utf-8')

for f in ["Kicad/Carte_V2/Alimentation_sch.kicad_sch", "Kicad/Carte_V2/Carte_V2.kicad_pcb"]:
    cmd = ["git", "diff", f]
    res = subprocess.run(cmd, capture_output=True, text=True, encoding='utf-8', errors='ignore')
    print(f"=== GIT DIFF: {f} ===")
    lines = res.stdout.splitlines()
    print(f"Total diff lines: {len(lines)}")
    for line in lines[:80]:
        print(line)
    if len(lines) > 80:
        print(f"... ({len(lines)-80} more lines)")
