import sys
sys.stdout.reconfigure(encoding='utf-8')

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\suivi_projet.html", "r", encoding="utf-8") as f:
    text = f.read()

import re
for tid in ["t21", "t22", "t23", "t24", "t25", "t26", "t27", "t28"]:
    pos = text.find(f'data-id="{tid}"')
    if pos != -1:
        end = text.find('</div>\n                </div>', pos)
        block = text[pos:pos+400]
        # find title
        tm = re.search(r'<div class="task-title">(.*?)</div>', block)
        print(f"{tid}: {tm.group(1) if tm else 'No title'}")
