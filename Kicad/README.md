# Kicad — Conception Électronique & PCB

Ce dossier regroupe les schémas électriques, le routage PCB et les bibliothèques KiCad de la SmileWatch.

## Points Clés Matériels

- **Architecture 6 couches** : séparation stricte des plans de masse analogique/RF/puissance et intégration dans un encombrement circulaire horloger.
- **Cœur RF & MCU** : `STM32WB55VGYx` (WLCSP-100), filtre RF intégré `MLPF-WB-02D3`, antenne céramique 2.4 GHz `Johanson 2450AT` avec carrure acier 316L flottante.
- **Capteurs I2C1** : `LSM6DSOX` (`0x6A`), `MAX30101` (`0x57`), `TMP117` (`0x48`), `VEML6030` (`0x10`), `DRV2605L` (`0x5A`), écran PMOLED `CH1115` (`0x3C`).
- **Alimentation & Recharge Qi** : Récepteur sans fil `BQ51003` + chargeur Li-Ion `BQ24040` pour pile `LIR3048`, boost `+5V` `TPS61023` commandé par `HR_5V_EN`, LDOs `TPS7A0233` / `TPS7A0218` (`25 nA`).
- **Programmation** : Empreinte `Tag-Connect TC2030-IDC-NL` (SWD 6 broches sans connecteur).
