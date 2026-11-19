import pypdf
import sys
sys.stdout.reconfigure(encoding='utf-8')

for p in [r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Datasheet\V2\Ecran 0.5 pouces.pdf", r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Datasheet\V1\GME8848-01.pdf"]:
    print(f"=== Reading {p} ===")
    reader = pypdf.PdfReader(p)
    print(f"Total pages: {len(reader.pages)}")
    for i, page in enumerate(reader.pages):
        txt = page.extract_text()
        if any(k in txt.upper() for k in ["VBAT", "CHARGE PUMP", "VCC", "VOLTAGE", "CHARACTERISTICS"]):
            print(f"--- Page {i+1} ---")
            for line in txt.splitlines():
                if any(k in line.upper() for k in ["VBAT", "CHARGE PUMP", "VCC", "VDD", "OPERATING", "MIN", "TYP", "MAX"]):
                    print("  ", line[:120])
