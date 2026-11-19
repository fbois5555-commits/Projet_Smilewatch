import os
import re

kicad_dir = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2"

print("=== Searching for C22 in all files in Carte_V2 ===")
for root, dirs, files in os.walk(kicad_dir):
    for f in files:
        if f.endswith((".kicad_sch", ".kicad_pcb", ".csv")):
            path = os.path.join(root, f)
            with open(path, "r", encoding="utf-8", errors="ignore") as file:
                content = file.read()
            # find all occurrences of "C22"
            matches = re.findall(r'\(property\s+"Reference"\s+"C22"|Reference.*C22|"C22"', content)
            if matches or "C22" in content:
                # count
                count = content.count('"C22"')
                print(f"File: {f} -> {count} occurrences of '\"C22\"'")
                # let's see context
                for line in content.splitlines():
                    if any(p in line for p in ['"C22"', 'Reference" "C22', 'C22,', ',C22']):
                        print(f"   [{f}] {line[:120]}")
