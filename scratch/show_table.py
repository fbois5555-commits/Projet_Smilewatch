import sys
sys.stdout.reconfigure(encoding='utf-8')

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\suivi_projet.html", "r", encoding="utf-8") as f:
    text = f.read()

import re
pos = text.find('table')
while pos != -1:
    end = text.find('</table>', pos)
    print(text[pos:pos+1500])
    pos = text.find('table', end)
    if pos != -1: break
