# Code — Firmware STM32WB55 SmileWatch

Ce dossier contient le firmware embarqué C pour le microcontrôleur **STM32WB55VGYx** (`SmileWatch1/`).

## Architecture Logicielle (`SmileWatch1/Core/`)

- **`Src/main.c` & `Inc/main.h`** : Initialisation HAL (horloges, GPIO, I2C1, RTC, PMIC/Boost `+5V_HR`) et boucle principale basse consommation avec entrée automatique en **mode `STOP2`** (`HAL_PWREx_EnterSTOP2Mode`) dès que l'écran OLED, le sport, `HRR60`, la torche et le SOS sont inactifs (`!SmileWatch_NeedsFastTick()`).
- **`Src/smilewatch_ui.c` & `Inc/smilewatch_ui.h`** :
  - **9 écrans × 3 vues (27 vues)** + mode `split_6h` anti-masquage des aiguilles.
  - **19 disciplines sportives** + **Triathlon** multi-étapes + **Tabata** (`20s/10s × 8`) + **Cohérence cardiaque** (`5s/5s`) + **Gainage** (`30s`).
  - **Coach Zones FC (`Z1–Z5`)**, optimiseur de charge aiguë/chronique (`TRIMP`) et `HRR60`.
  - **Santé & Sécurité** : FC/SpO2/HRV, tachycardie au repos (`>105 BPM` 10 min), réveil doux (`-20 min`), Résumé Réveil + pré-maladie, sieste détectée/programmée, SOS chute `10s` + `ICE`.
  - **Autonomie** : `Look-to-Wake`, Mode Nuit, lampe torche OLED, Pomodoro `25m/5m`, double fuseau, sédentarité `50 min`, Mode Survie Extrême (`<= 10 %` : Miyota seule).
- **`Src/oled_ch1115.c` & `Inc/oled_ch1115.h`** : Driver PMOLED `88×48` (`CH1115`, I2C `0x3C`), primitives graphiques et contraste adaptatif (`OLED_SetContrast`).
- **`Src/sensors_driver.c` & `Inc/sensors_driver.h`** : Drivers I2C1 pour `LSM6DSOX` (IMU + MLC), `MAX30101` (PPG/HRV), `TMP117` (température ±0.1 °C), `VEML6030` (lux) et `DRV2605L` (haptique).
- **`Src/miyota_driver.c` & `Inc/miyota_driver.h`** : Pilotage direct de la bobine du mouvement Miyota 2035 (`PA7`/`PB5`) avec pause devant la pilule OLED (`:24..:35`).
- **`Src/flash_storage.c` & `Inc/flash_storage.h`** : Persistance non-volatile en **Page 127 (`0x0807F000`, 4 Ko)** : réglages, 8 séances sport, et **buffer circulaire `90 %` (`328/365` jours)** avec rappel `SYNC WEB!` et horodatage de la dernière sauvegarde.
