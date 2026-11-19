import sys
sys.stdout.reconfigure(encoding='utf-8')

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\suivi_projet.html", "r", encoding="utf-8") as f:
    text = f.read()

import re
# Look for id or class related to planning or tasks
headers = re.findall(r'<h[1-4][^>]*>(.*?)</h[1-4]>', text)
print("Headers in suivi_projet.html:")
for h in headers[:25]:
    print(" -", h)

# Search for task table or task items
tasks = re.findall(r'id="(t\d+)"[^>]*>([\s\S]*?)</tr>', text)
print(f"Found {len(tasks)} tasks in table.")
for tid, content in tasks[-8:]:
    # clean tags
    clean = re.sub(r'<[^>]+>', ' ', content).strip()
    clean = ' '.join(clean.split())
    print(f"Task {tid}: {clean[:100]}")
