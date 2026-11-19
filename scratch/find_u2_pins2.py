with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\MCU.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

lib_idx = text.find('STM32WB55VGYx')
print(text[lib_idx:lib_idx+600])
