import sys
sys.stdout.reconfigure(encoding='utf-8')

with open(r"C:\Users\fbois\Documents\Perso\Projet_Smilewatch\Audit_Complet_Smilewatch_V2.md", "r", encoding="utf-8") as f:
    text = f.read()

pos = text.find("P0-ELEC-1 :")
if pos != -1:
    print(text[pos:pos+1800])
