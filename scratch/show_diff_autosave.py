import difflib

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Alimentation_sch.kicad_sch", "r", encoding="utf-8") as f:
    orig = f.readlines()

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\_autosave-Alimentation_sch.kicad_sch", "r", encoding="utf-8") as f:
    auto = f.readlines()

diff = list(difflib.unified_diff(orig, auto, n=3))
print(f"Diff lines count: {len(diff)}")
for line in diff[:100]:
    print(line, end="")
