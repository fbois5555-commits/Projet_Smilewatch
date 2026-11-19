import sys
sys.stdout.reconfigure(encoding='utf-8')

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\suivi_projet.html", "r", encoding="utf-8") as f:
    text = f.read()

import re
tasks = re.findall(r'<div class="task-card"[^>]*data-id="(t\d+)"[^>]*>([\s\S]*?)</div>\s*</div>', text)
print(f"Found {len(tasks)} tasks.")
for tid, content in tasks:
    title_m = re.search(r'<div class="task-title">([^<]+)</div>', content)
    title = title_m.group(1) if title_m else "?"
    print(f"{tid}: {title}")
