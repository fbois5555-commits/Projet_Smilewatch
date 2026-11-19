import sys
sys.stdout.reconfigure(encoding='utf-8')

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\REFLEXIONS.md", "r", encoding="utf-8") as f:
    text = f.read()

print("Length of REFLEXIONS.md:", len(text))
print("Last 1500 chars:")
print(text[-1500:])
