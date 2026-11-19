with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Kicad\Carte_V2\MCU.kicad_sch", "r", encoding="utf-8") as f:
    text = f.read()

import re

# find pin PA0 in STM32WB55VGYx
idx = text.find('(name "PA0"')
if idx != -1:
    print(text[idx-50:idx+250])
