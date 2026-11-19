import sys
sys.stdout.reconfigure(encoding='utf-8')

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\suivi_projet.html", "r", encoding="utf-8") as f:
    text = f.read()

pos = text.find('<table class="inv-table">')
if pos != -1:
    end = text.find('</table>', pos)
    print(text[pos:end+8])
