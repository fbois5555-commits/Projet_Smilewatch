import pypdf
reader = pypdf.PdfReader('Datasheet/V2/Ecran 0.5 pouces.pdf')
for i, page in enumerate(reader.pages):
    text = page.extract_text() or ""
    if "3.3.2" in text or "Charge Pump" in text or "Figure" in text or "C1P" in text:
        print(f"=== PAGE {i+1} ===")
        print(text.encode('ascii', errors='replace').decode('ascii'))
