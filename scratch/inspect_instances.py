import re

sch_path = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch"
with open(sch_path, 'r', encoding='utf-8') as f:
    text = f.read()

# Find placed instances of C22
pattern = r'\(symbol\s+\(lib_id "BQ24040DSQR:S1921"\)[\s\S]*?\n\t\)'
matches = list(re.finditer(pattern, text))
for m in matches:
    print(m.group(0))

# Let's also check TP2 to TP13 instances
pattern_tp = r'\(symbol\s+\(lib_id "[^"]*TP[^"]*"\)[\s\S]*?\n\t\)'
matches_tp = list(re.finditer(r'\(symbol\s+[^)]*TP13[\s\S]*?\n\t\)', text))
for m in matches_tp:
    print("TP13 match:", m.group(0)[:300])

# Also search for "TP13" occurrences in text
for line in text.splitlines():
    if "TP13" in line or "C22" in line or "TP2" in line:
        print(line)
