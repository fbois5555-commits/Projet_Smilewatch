import re

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\Ecran.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

# Replace CRLF with LF to be sure
text_lf = text.replace('\r\n', '\n')

wires = re.findall(r'\(wire\s+\(pts\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\)', text_lf)
print(f"Total wires in Ecran: {len(wires)}")
for x1, y1, x2, y2 in wires:
    x1, y1, x2, y2 = float(x1), float(y1), float(x2), float(y2)
    # Check if close to Q1 (208.28, 59.69) or Q2 (173.99, 67.31)
    if (150 <= x1 <= 230 and 40 <= y1 <= 80) or (150 <= x2 <= 230 and 40 <= y2 <= 80):
        print(f"Wire: ({x1:.2f}, {y1:.2f}) -> ({x2:.2f}, {y2:.2f})")

# Let's inspect pins of Q1 and Q2 in their lib definitions
def show_symbol_pins(sym_name):
    idx = text_lf.find(f'(symbol "{sym_name}"')
    if idx != -1:
        end = text_lf.find('\n\t\t)', idx)
        block = text_lf[idx:end]
        pins = re.findall(r'\(pin\s+(\w+)\s+(\w+)\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(length\s+[\d\.-]+\)\s+\(name\s+"([^"]*)"[^\)]*\)\s+\(number\s+"([^"]*)"', block)
        print(f"Symbol {sym_name} pins:")
        for ptype, pshape, px, py, prot, pname, pnum in pins:
            print(f"  pin {pnum} ({pname}) at ({px}, {py}, rot={prot}) type={ptype}")

show_symbol_pins("FDN338P:FDN338P")
show_symbol_pins("FDN335N:FDN335N")
