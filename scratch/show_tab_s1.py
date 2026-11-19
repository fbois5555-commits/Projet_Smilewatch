import sys
sys.stdout.reconfigure(encoding='utf-8')

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\suivi_projet.html", "r", encoding="utf-8") as f:
    text = f.read()

pos = text.find("Session SolidWorks Terminée &amp; Validée")
if pos != -1:
    print(text[pos-100:pos+1500])
