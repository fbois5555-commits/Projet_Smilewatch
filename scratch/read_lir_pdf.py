import pypdf
import sys
sys.stdout.reconfigure(encoding='utf-8')

pdf_path = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Datasheet\V2\LIR3048  280mAh.pdf"
reader = pypdf.PdfReader(pdf_path)
print("Pages:", len(reader.pages))
text = ""
for i, page in enumerate(reader.pages):
    print(f"--- Page {i+1} ---")
    print(page.extract_text()[:600])
