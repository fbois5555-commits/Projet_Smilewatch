with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\suivi_projet.html", "r", encoding="utf-8") as f:
    text = f.read()

import re
print("Length of suivi_projet.html:", len(text))
# Find sections related to planning or gantt or timeline
for line in text.splitlines():
    if any(k in line.lower() for k in ["planning", "tâche", "livraison", "calendrier", "délai", "semaine", "gantt"]):
        print(line[:120])
