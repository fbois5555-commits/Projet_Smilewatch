import subprocess
import sys
sys.stdout.reconfigure(encoding='utf-8')

res = subprocess.run(["git", "diff", "Kicad/Carte_V2/Carte_V2.kicad_pcb"], capture_output=True, text=True, encoding='utf-8', errors='ignore')

# Look for C22 and TP in the diff
diff_lines = res.stdout.splitlines()
print(f"Total diff lines: {len(diff_lines)}")
for i, line in enumerate(diff_lines):
    if any(k in line for k in ['"C22"', '"TP8"', '"TP2"', 'net "+3V8"', 'net "GNDPWR"']):
        for j in range(max(0, i-3), min(len(diff_lines), i+8)):
            print(diff_lines[j])
        print("---")
