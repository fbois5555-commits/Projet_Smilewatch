# SmileWatch — Montre Connectée Hybride Haute Précision

> **Projet R&D — Conception mécatronique, électronique et horlogère avancée.**  
> Montre hybride alliant l'élégance et la sobriété d'un mouvement analogique à aiguilles, la discrétion d'un affichage numérique micro-OLED, et des capteurs biomédicaux de grade clinique, sans dépendre d'une application propriétaire grâce aux normes universelles Bluetooth SIG.

---

## 🌟 Vision & Philosophie du Projet

La **SmileWatch** est une montre hybride conçue pour répondre à un cahier des charges exigeant :
1. **Élégance horlogère & Discrétion** : Un vrai cadran horloger avec aiguilles physiques animées par un mouvement électromécanique, intégrant subtilement un écran micro-OLED 0.5" pour les notifications et les métriques de santé.
2. **Standard ouvert sans application smartphone dédiée** : Exploitation native des profils Bluetooth SIG standards (CTS, ANCS, HRS, HTS, BAS, DIS) permettant à la montre de fonctionner directement avec iOS et Android (synchronisation de l'heure, notifications directes du smartphone, suivi santé sur Apple Santé / Google Health Connect) sans application tierce requise.
3. **Précision biomédicale** : Capteur optique PPG (fréquence cardiaque & SpO2) et capteur de température cutanée à précision médicale (±0.1 °C) couplé à un insert thermique dédié.
4. **Zéro connecteur / Étanchéité totale** : Châssis métallique monobloc étanche avec recharge inductive sans fil Qi et programmation/test interne via Tag-Connect.

---

## 🛠️ Spécifications Techniques

| Domaine | Caractéristique | Spécification |
| :--- | :--- | :--- |
| **Boîtier / Carrure** | Matériau & Fabrication | **Acier Inox 316L** en **fabrication additive métallique (SLM / DMLS)** avec **reprises d'usinage CNC** des portées critiques (joints, portées d'étanchéité, lunette). |
| **Glace** | Supérieure & Inférieure | Glace supérieure en **Saphir plat** avec joint d'étanchéité Hytrel/Téflon ; fenêtre inférieure en verre minéral optique scellé à la colle UV pour le capteur PPG. |
| **Cadran** | Conception | **Cadran usiné en PCB (FR4 0.4 mm)** faisant office de cadran décoratif (sérigraphie/dorure, index, masquage) avec fenêtre découpée pour l'OLED et passage d'aiguilles. |
| **Mouvement analogique**| Horlogerie | Mouvement quartz **Miyota 2035 modifié (sans couronne)**, dont la bobine du moteur pas-à-pas est directement pilotée par le microcontrôleur via des impulsions électriques étalonnées. |
| **Afficheur numérique** | Écran | **PMOLED 0.5" monochrome (88 × 48 pixels)**, contrôleur **CH1115**, interface I2C, nappe 14 broches repliée sous le cadran avec écran acrylique de protection. |
| **Microcontrôleur (MCU)**| Processeur & Sans-fil | **STMicroelectronics STM32WB55** (Dual-Core : ARM Cortex-M4 @ 64 MHz pour l'applicatif + Cortex-M0+ dédié à la stack RF BLE 5.4), filtre RF intégré **MLPF-WB-02D3**, antenne céramique 2.4 GHz **Johanson 2450AT**. |
| **Capteurs** | Biomédical & Mouvement | • **TMP117** : Température corporelle haute précision (±0.1 °C de grade médical), interfacé via un pont/pad thermique métallique.<br>• **MAX30101** : Capteur optique PPG (Fréquence cardiaque & Oxymétrie de pouls SpO2) avec masque optique Kapton noir anti-diaphonie (120 µm).<br>• **LSM6DSOX** : IMU 6 axes (accéléromètre + gyroscope) avec Machine Learning Core (MLC) pour le podomètre et la reconnaissance de gestes au poignet ultra-basse consommation.<br>• **VEML6030** : Capteur de luminosité ambiante avec guide d'onde lumineux pour l'ajustement automatique de l'OLED. |
| **Interface & Haptique**| I/O & Retours | Contrôleur haptique évolué **TI DRV2605L** + vibreur CMS **HD-EMB1204-SM** ; 2 boutons poussoirs tactiles latéraux étanches **Alps Alpine SKSLLAE010**. |
| **Batterie & Autonomie**| Alimentation | Pile bouton rechargeable Li-Ion **LIR3048 (280 mAh, 3.7 V)** maintenue par contact latéral CMS Harwin (S1921-46R) et calage en mousse Poron. |
| **Recharge** | Sans fil | Récepteur sans fil Qi **TI BQ51003** + bobine de réception Würth Elektronik (**760308101309**) + chargeur linéaire Li-Ion **TI BQ24040**. |
| **Conversion d'énergie**| PMIC & Régulation | Convertisseur élévateur **TPS61023DRLR** (+5.0V pour LEDs capteurs, `R23/R24 = 150k/20k`, inductance **1.0 µH** `DFE18SBN1R0ME0L` `0603`), LDOs +3.3V / +1.8V **TPS7A0233DBVR / TPS7A0218PDBVR** (25 nA), protection batterie **AP9101CAK-ATTRG1** (`C5588142`). |
| **Débogage & Flash** | Connectique | Empreinte **Tag-Connect TC2030-IDC-NL** (SWD 6 broches sans embase physique). |

---

## 📡 Stratégie Bluetooth : Profils Standards & Écosystème

Pour garantir une pérennité totale, une compatibilité multi-OS immédiate et s'affranchir du développement/maintenance d'une application mobile tierce, la SmileWatch implémente directement les **spécifications et profils standardisés Bluetooth SIG et Apple/Android** :

* **CTS (Current Time Service - `0x1805`)** : Synchronisation automatique et transparente de l'heure et du fuseau horaire dès la connexion au smartphone.
* **ANCS (Apple Notification Center Service)** & **Notification Provider Android** : Réception native des alertes (appels entrants, SMS, calendrier, messageries) directement relayées sur l'écran OLED sans passer par une app intermédiaire.
* **HRS (Heart Rate Service - `0x180D`)** : Transmission standardisée de la fréquence cardiaque, reconnue nativement par Apple Santé, Google Health Connect, Strava, Garmin, etc.
* **HTS (Health Thermometer Service - `0x1809`)** : Exportation de la température corporelle clinique mesurée par le TMP117.
* **BAS (Battery Service - `0x180F`)** : Niveau de batterie affiché nativement dans le widget batterie du smartphone.
* **DIS (Device Information Service - `0x180A`)** : Renseignement du nom de modèle, révision hardware, firmware et numéro de série.

---

## 🧩 Architecture Matérielle & Modulaire (Notice V3.8)

L'assemblage physique de la montre repose sur une **architecture recto-verso modulaire à deux sous-ensembles**, assemblés selon une séquence millimétrée :

### 1. Module Supérieur (Face avant)
* **Outillage impératif** : Utilisation d'une **cale 3D d'assemblage (posage O4)** insérée par le bas de la carrure pour encaisser l'effort axial lors du chassage de la glace saphir et préserver le mouvement Miyota.
* **Mouvement horloger** : Positionnement du Miyota 2035 au centre, coiffé par le PCB supérieur (chevauchement de 0.8 mm). Soudure des micro-fils de la bobine du Miyota directement sur les pastilles de 1 mm du PCB supérieur.
* **Carrure flottante & Écran** : Architecture à carrure 316L électriquement flottante (suppression de l'époxy conductrice à l'argent initialement prévue en V3.8, le plan de masse interne 6 couches assurant le contrepoids RF). Soudure directe à plat de la nappe 14 broches (pas 0.62 mm) de l'OLED CH1115, repliement et collage sous le cadran avec fenêtre acrylique.
* **Aiguilles & Chassage Saphir** : Pose des aiguilles des heures et minutes à midi pile, pose du joint Téflon/Hytrel et chassage en force de la glace saphir à la presse d'établi. Retrait de la cale 3D par le bas.

### 2. Module Inférieur (Face arrière & Capteurs)
* **Module thermique** : Insert métallique chassé dans la base polymère, contact thermique avec le capteur TMP117 via pad élastomère.
* **Bobine Qi & Batterie** : Bobine Würth 760308101309 collée dans la base polymère + disque isolant Kapton. Insertion de la batterie LIR3048 (280 mAh) dans le contact latéral CMS, calée par pastille Poron.
* **Optique PPG** : Masque Kapton noir (120 µm) centré sur le MAX30101 (isolation optique émetteur/récepteur), scellement de la glace minérale inférieure à la colle UV.

### 3. Liaison Inter-Modules & Fermeture (Point de non-retour)
* **Interconnexion** : Connecteur THT (Samtec) assurant la liaison électrique rigide entre le PCB supérieur et le PCB capteur inférieur. Soudure par le dessous et arasage des broches à ras. Soudure des micro-fils de la bobine Qi.
* **Fermeture** : Graissage léger du joint torique périphérique (O-ring) de la base polymère et chassage en force du fond dans la carrure (compression axiale finale assurée par les mousses Poron).

---

## 📁 Arborescence du Projet

```text
Projet_Smilewatch/
├── 3D/                          # Fichiers CAO SolidWorks (Pièces .SLDPRT, Assemblages .SLDASM, STEP)
│   ├── carrure.SLDPRT/.STEP     # Boîtier principal (conçu pour impression 3D métal SLM + usinage)
│   ├── Cadran.SLDPRT            # Cadran horloger (conçu sur base PCB FR4 0.4mm)
│   ├── Carte_Capteur_V1/V2.step # Encombrement 3D de la carte capteurs
│   ├── Carte_V1 / Carte_V2.step # Encombrement 3D de la carte mère
│   ├── Miyato_2035.SLDPRT       # Modèle 3D du mouvement horloger
│   ├── Pad_thermique / Pont...  # Pièces de transfert thermique pour le TMP117
│   └── Montre.mp4               # Animation de rendu et cinématique d'assemblage
├── Code/                        # Firmware embarqué sous STM32CubeIDE
│   └── SmileWatch1/             # Projet STM32CubeMX / CubeIDE pour STM32WB55
│       ├── Core/                # Code source C applicatif (main.c, interruptions, init I2C/GPIO)
│       └── Drivers/             # HAL STM32WBxx et CMSIS
├── Commande/                    # Devis, références et paniers de composants (Mouser/Digikey/JLCPCB)
├── Datasheet/                   # Fiches techniques des composants électroniques
│   ├── V1/                      # Datasheets de l'architecture V1 (STM32WB5MMG SiP)
│   └── V2/                      # Datasheets de l'architecture V2 (STM32WB55, filtres, antenne)
├── Fabrication_assemblage/      # Guides et documents de fabrication
│   └── Notice_montage_montre.pdf# Manuel d'intégration mécanique modulaire recto-verso (V3.8)
├── Kicad/                       # Projets de CAO électronique KiCad
│   ├── Carte_V1/                # Première itération matérielle
│   ├── Carte_V2/                # Schémas hiérarchiques et PCB v2 (MCU, Alim Qi, Capteurs, Écran, Moteur)
│   │   ├── Carte_Capteur_V2/    # Sous-carte capteurs (TMP117, MAX30101, boost TPS61023, TCA9517D) & production/
│   │   └── production/          # Fichiers de fabrication PCB (Gerbers, Drill, CPL, BOM JLCPCB)
│   └── Composant/               # Empreintes et symboles personnalisés
├── Media/                       # Rendu 3D, photos, vues éclatées et transparentes
├── README.md                    # Présentation et documentation centrale du projet
└── REFLEXIONS.md                # Carnet d\'idées, backlog et points d\'attention techniques
```

---

## 🎯 Statut Actuel & Feuille de Route

- [x] **Architecture système & Sélection des composants** : Définition complète (STM32WB55 WLCSP-100, LIR3048 280 mAh, Qi BQ51003, capteurs cliniques).
- [x] **CAO Mécanique 3D (SolidWorks 2026) & PCB Cadran FR4** : Modélisation carrure, base, verres, posage O4 et conception du cadran horloger FR4 0.4 mm (dorure ENIG sur masque noir mat).
- [x] **Notice de montage modulaire** : Rédaction du manuel d'intégration mécanique (V3.8, mise à jour V3.9 prévue).
- [x] **Audit d'Ingénierie Global 360° (08 Novembre 2026)** : Inspection intégrale sans approximation des 3 PCB KiCad 10, des datasheets constructeurs et des 50 interférences SolidWorks de `Montre.SLDASM` (détails et correctifs consignés dans `REFLEXIONS.md` §13 et `suivi_projet.html`).
- [x] **Passe Corrective Électronique, Datasheets & BOM V2.1 (11 Novembre 2026)** :
  - **`Carte_V2`** : Polarité batterie `LIR3048` corrigée (`BT1` = `C22` latéral `+3V8`, `TP8` `6×6 mm` sur `GNDPWR`, `TP2` supprimé — *point d'attention : contact central plat excentré à $r=11.05\text{ mm}$ sans ressort*) ; switch OLED `Q1`/`Q2` câblé (`Q2.2` à `GND`, `Q1.2`+`R15` sur `+3V8`, `R13 = 560 kΩ`) ; RF corrigée (`R21`/`R24 = DNP` + `in_pos_files no`, Keepout 6 couches `ANT1`, quartz `Y1 = ABM13W 8 pF` + `12 pF C0G`) ; pont ADC `VBAT` (`2×1 MΩ + 100 nF` sur `PC4`), LDOs `TPS7A0233/18`, `R5 = 1.5 kΩ`, `R7 = DNP`, `C46 = 100 nF`, `R16 = 330 Ω`, `U4 = AP9101CAK-ATTRG1` (`C5588142`, Auto-Wake-Up), `C0G2_50V1 = 220 nF` (`C21120`), `R27 = 10 kΩ` (`C25744`, remplace `TH2`), `TH1 = 10k NTC` (`C91636`), assainissement complet des `JLCPCB Part#` passifs. **ERC = 0 erreur (62 warnings), DRC (règles projet) + parité = 0 violation, 0 non connecté** *(20 alertes `--severity-all` préexistantes connues sur `S1/S2` bord `0.17–0.19 mm` et freins thermiques `IC2`)*.
  - **`Carte_Capteur_V1` & `Cadran`** : Boost `+5.0V` migré vers **`TPS61023DRLR`** (`IC3`, `C919459`, stk `6 681`) + **`L1 = 1.0 µH` `0603` (`DFE18SBN1R0ME0L`, `C882989`, stk `2 188`)** + **`R23 = 150 kΩ` (`C25755`) / `R24 = 20 kΩ` (`C25765`)** + `C18/C19 = 10 µF` (`C96446`) ; **`U3 = TCA9517DR`** (`C182421`, **stk `5 738` en stock JLC**) inversé et rerouté Side A (`+1V8` MAX30101) / Side B (`+3V3` MCU) ; îlot thermique `B.Cu` + 2 vias + keepout `U2_Thermal_Iso` sous `TMP117` (`U2`). **ERC = 0 erreur (`11` warnings), DRC + parité = 0 violation** (`Cadran` inchangé avec marge OLED conservée : ERC `0` / DRC `0` erreur, `55` warnings sérigraphie bord).
  - **Datasheets & Livrables usine mis à jour** : `Datasheet/V2/` actualisé (`tps61023.pdf`, `tps7a02.pdf` EN `SBVS277C`, `ABM13W`, `NCP15XH103F0SRC`, `DFE18SBN1R0ME0L`, `Samtec_TW_Series`, `CL10A106MA8NRNC.pdf`, `GRM1555C1H222GA01D.pdf` ; retrait `MAX17220`, `AP2127`, `Hirose`, `74438323100`), packages **`production/`** (Gerbers `.zip`, Drill, IPC-356, `bom.csv`, `positions.csv`) régénérés pour les 3 cartes, BOM schémas `Carte_V2.csv` / `Carte_Capteur_V1.csv` à jour, et modèles **`3D/Carte_V2.3f.step`** / **`3D/Carte_Capteur_V2f.step`** ré-exportés.
- [x] **Finalisation Routage, Empreintes JLC & Modèles 3D `Carte_V2` (14 Novembre 2026)** :
  - **Empreintes & Modèles 3D** : Bibliothèque `Smilewatch_V2_JLC.pretty` et `Carte_V2.kicad_pcb` mises à jour avec chemins `.step` corrigés (`${KIPRJMOD}/../Composant/...`) pour `ANT1`, `FL1`, `Y1`, `Q3`, `IC2`, et création de `LD-SM-430.step` (`4,0 × 11,0 × 3,6 mm`) pour le moteur `M1`.
  - **Antenne `ANT1` & RF `50 Ω`** : Zone keep-out `4,0 × 6,0 mm` 6 couches intégrée à `ANT1` (`Pin 4 = FEED`, `Pin 1–3 = GND`), placée à `2,81 mm` du bord (`R = 19,45 mm`, un seul coin déborde de `0,35 mm`, `BT1` hors zone active) ; ligne coplanaire `GCPW` ajustée à **`0,135 mm`** / jeu **`0,10 mm`** pour le stackup **`JLC06081H-2116`** (`0,8 mm`, 6 couches, `0,1349 mm` sur l'`Impedance calculator` JLCPCB, ~100 € moins cher que `2313`), sur `U2.K4 → FL1 → Pi-network R21/R22/R24 → ANT1.4` avec plan `In1.Cu` continu et vias de couture `GND`.
  - **Prochaine étape immédiate (`15 Nov 2026`)** : Corriger les rotations 3D sur JLCPCB / `positions.csv` (`IC8`, `IC6`, `IC4`, `U4`, `IC1`, vérifier `ANT1`), attendre la livraison de `U2` (`C1850399`) dans `My Parts`, puis lancer la commande PCBA (`JLC06081H-2116`, `Depanel = Yes`) et finaliser la validation des lèvres fines (`0,50 / 0,64 mm`) de `3D/carrure.SLDPRT`.
- [ ] **Verrouillage avant commande Usine (Points Méca, Datasheets & Sourcing en attente)** :
  - **Mécanique CAO (`Montre.SLDASM`)** : Portée saphir (`+1.35 mm`), gorge O-ring ISO 3601 (25 %), `Batterie.SLDPRT` (`4.90 mm`) et `Canal_lum.SLDPRT` travaillés (`09/11`, proto PLA lancé) ; `3D/carrure.SLDPRT` contrôlée (`51,04 × 44,21 × 9,65 mm`, paroi `1,45–1,48 mm` OK, lèvres `0,50 / 0,64 mm` à vérifier). **Modifications & contrôles restants sous SolidWorks avec les `.STEP` frais** : **inverser le clips** `Base`/`carrure` pour faciliter la fabrication, statuer sur les composants `F.Cu` sous la dalle OLED (`Q1`, `Q2`, `7×0603` `[P0-MECA-7]`), confirmer poche sous `J1` + lamage Qi dans `Base.SLDPRT` `[P0-MECA-6]`, intégrer poussoirs étanches `Bouton.SLDASM` `[P1-MECA-9]` et vérifier dégagement radial `TPS61023`/`L1 0603` `[P1-MECA-10]`.
  - **Points d'attention Datasheets JLC (`V2.1`)** : **`U4` (`AP9101CAK-ATTRG1` `C5588142`, stk `162` ✅)** = variante `CAK` **Auto-Wake-Up** (`VCU=4.400 V` protégée en aval par `BQ24040` à `4.20 V`, `VDL=2.800 V` adaptée à la `LIR3048`, `0 V` autorisée) ; **`C0G2_50V1` (`C21120` `CL10B224KA8NNNC`)** = `220 nF 25 V X7R` (au lieu de `C0G 50 V`) sur résonance série Qi `10 µH` (`760308101309`) ; **`TPS61023` (`IC3`)** alimenté par `IC6` (`TPS7A0233` `200 mA` max ➔ pulses `MAX30101` `≤ 50 mA`), `+5.06 V` typ (`C19/C26 = 2×10 µF 25V X5R`).
  - **Sourcing JLCPCB SMT (`15 Nov 2026`)** : Tous les composants JLC de `Carte_Capteur_V1` sont en stock (`J1` main). Sur **`Carte_V2`** (`bom.csv` réaligné sur KiCad, `46` lignes / `99` composants) : **`IC1`, `IC8`, `FL1`, `TH1` sécurisés dans `My Parts`** ; `ANT1` (`646`), `Y1` (`380`), `M1` (`2 382`), `Q3` (`170 444`) en stock public ; **seul `U2` (`C1850399`) reste en attente de livraison** (activer **Epoxy Filled & Capped** + `JLC06081H-2116`). Hors-PCBA à souder/monter manuellement : `C39`, `J2`, `BT1`/`TP8`, `TP1`/`TP3`, `TP6`/`TP7`.
  - **Firmware `[P2-FW-16]`** : Reporté après le GO usine matériel.
- [ ] **Fabrication des prototypes matériels** :
  - Commande de la carrure en fabrication additive métallique (SLM/DMLS Inox 316L) + reprise CNC et fabrication de la Base.
  - Fabrication et assemblage PCB (PCBA JLCPCB pour carte mère 6 couches, carte capteurs et cadran).
- [ ] **Développement Firmware (`STM32WB55VGYx` WLCSP-100)** :
  - Création du nouveau projet STM32CubeIDE sur cible `STM32WB55VGYx` (remplaçant le squelette V1 `STM32WB5MMGHx`) avec `HSE 32 MHz`, `LSE` et pile BLE double-cœur (`STM32_WPAN` + OTA).
  - Implémentation des profils BLE standards (CTS, ANCS, HRS, HTS, BAS, DIS).
  - Pilote pas-à-pas bipolaire amorti (impulsion calibrée + freinage électrodynamique `LOW-LOW`) pour le mouvement Miyota 2035.
  - Driver I2C DMA pour l'affichage CH1115 (OLED 88×48) et les capteurs, et gestion veille ultra-basse consommation (`STOP 2` + réveil MLC `LSM6DSOX`).
