with open('Kicad/Carte_V2/MCU.kicad_sch', 'r', encoding='utf-8') as f:
    mcu = f.read()

idx = mcu.find('(number "G10"')
start = mcu.rfind('(pin', 0, idx)
end = mcu.find('\n\t\t\t\t)', idx)
print("G10 def in MCU.kicad_sch:")
print(mcu[start:end])
