import sys
sys.stdout.reconfigure(encoding='utf-8')

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\suivi_projet.html", "r", encoding="utf-8") as f:
    text = f.read()

import re

# find sections, cards, tabs, or tables
sections = re.findall(r'<section[^>]*id="([^"]*)"[^>]*>', text)
print("Sections:", sections)

# find all table IDs or headings
tables = re.findall(r'<table[^>]*>', text)
print("Tables count:", len(tables))

# search for dates or planning
for line in text.splitlines():
    if any(k in line.lower() for k in ["planning", "semaine", "gantt", "novembre", "décembre", "phase", "livraison", "spark"]):
        print(line[:120])
