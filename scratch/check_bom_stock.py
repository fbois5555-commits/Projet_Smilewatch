import csv
import json
import urllib.request

BOM_PATH = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\production\bom.csv"
EXTRA_PARTS = [
    ("ANT1 (new)", "C17410360"),
    ("FL1 (new)", "C1855732"),
    ("Y1 (new)", "C254218"),
    ("Q3 (new)", "C5123939"),
    ("M1 (new)", "C41348533"),
    ("IC8 (alt YZF-T)", "C2150692"),
    ("IC8 (alt YZF-R)", "C81079"),
]

def jlc_get(code):
    url = "https://jlcpcb.com/api/overseas-pcb-order/v1/shoppingCart/smtGood/selectSmtComponentList"
    req = urllib.request.Request(
        url,
        data=json.dumps({"keyword": code, "currentPage": 1, "pageSize": 5}).encode(),
        headers={"Content-Type": "application/json", "User-Agent": "Mozilla/5.0"},
    )
    res = json.loads(urllib.request.urlopen(req, timeout=10).read().decode())
    for c in (res.get("data", {}).get("componentPageInfo") or {}).get("list") or []:
        if c["componentCode"] == code:
            return c
    return None

def main():
    seen = {}
    with open(BOM_PATH, encoding="utf-8-sig") as f:
        for r in csv.DictReader(f):
            code = (r.get("LCSC Part #") or r.get("LCSC") or "").strip()
            if code and code.startswith("C") and code[1:].isdigit():
                seen[code] = r.get("Designator", "")
    for label, code in EXTRA_PARTS:
        seen[code] = label

    alerts = []
    for code, des in sorted(seen.items()):
        c = jlc_get(code)
        stk = c["stockCount"] if c else -1
        model = c["componentModelEn"] if c else "?"
        if stk < 100:
            alerts.append((stk, code, des, model))

    alerts.sort()
    print(f"Checked {len(seen)} LCSC parts. Low stock (<100):")
    for stk, code, des, model in alerts:
        print(f"  stk={stk:4d} | {code:10s} | {des:18s} | {model}")

if __name__ == "__main__":
    main()
