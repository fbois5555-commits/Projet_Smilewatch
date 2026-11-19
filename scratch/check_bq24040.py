import re

with open('Kicad/Carte_V2/Alimentation_sch.kicad_sch', 'r', encoding='utf-8') as f:
    alim = f.read()

idx = alim.find('(property "Reference" "IC2"')
start = alim.rfind('(symbol', 0, idx)
end = alim.find('\n\t)', idx)
print("=== IC2 (BQ24040DSQR) ===")
print(alim[start:end])

# Also check pins in symbol def
idx_sym = alim.find('(symbol "BQ24040DSQR:BQ24040DSQR"')
print("=== BQ24040DSQR Symbol Pins ===")
print(alim[idx_sym:idx_sym+1200])
