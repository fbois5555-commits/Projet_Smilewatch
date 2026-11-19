import re

with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

# Let's inspect all symbols in MCU.kicad_sch
symbols = re.findall(r'\(symbol\s+[\s\S]*?\(property\s+\"Reference\"\s+\"([^\"]+)\"[\s\S]*?\(property\s+\"Value\"\s+\"([^\"]*)\"', mcu)
print("=== SYMBOLS IN MCU.kicad_sch ===")
for ref, val in sorted(symbols):
    print(f"  {ref:12} : {val}")

# Check PA0
print("\n=== SEARCH FOR PA0 ===")
for line in mcu.splitlines():
    if 'PA0' in line or 'G7' in line or 'ADC' in line:
        print(line)

# Check ANT / RF symbols
print("\n=== SEARCH FOR ANT / RF ===")
for line in mcu.splitlines():
    if any(k in line for k in ['ANT', '2450', 'MLPF', 'BALUN', 'R21', 'R24', 'L1', 'L2', 'Y1', 'Y2']):
        print(line[:120])
