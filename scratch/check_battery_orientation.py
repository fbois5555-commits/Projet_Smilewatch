import sys
sys.stdout.reconfigure(encoding='utf-8')

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\REFLEXIONS.md", "r", encoding="utf-8") as f:
    text = f.read()

for line in text.splitlines():
    if "LIR3048" in line or "P0-ELEC-1" in line or "TP2" in line or "S1921" in line:
        print(line[:130])
