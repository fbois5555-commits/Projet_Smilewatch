import pypdf
import sys
sys.stdout.reconfigure(encoding='utf-8')

pdf_path = r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Datasheet\V2\LIR3048  280mAh.pdf"
reader = pypdf.PdfReader(pdf_path)
page3 = reader.pages[2]
print(page3.extract_text())
