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
| **Conversion d'énergie**| PMIC & Régulation | Convertisseur élévateur nanoPower **MAX17222** (+5V pour LEDs capteurs, inductance **1.0 µH** `MLP1005M1R0DT0S1`), LDOs +3.3V / +1.8V **TPS7A0233DBVR / TPS7A0218PDBVR** (25 nA), protection batterie **AP9101CAK-ATTRG1**. |
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

## 📂 Contenu du Dossier Technique

```
audits/
├── README.md                          # Présentation technique et spécifications (ce document)
├── Code/
│   └── SmileWatch1/                   # Firmware STM32WB55 (STM32CubeIDE - C)
├── Kicad/
│   ├── Carte_V2/                      # Carte mère V2 (6 couches : schémas + PCB KiCad 10)
│   │   ├── Carte_Capteur_V2/          # PCB satellite capteurs optiques (KiCad 10)
│   │   └── Cadran/                    # Cadran PCB + fichiers DXF (KiCad 10)
│   └── Composant/                     # Librairie locale référencée par KiCad (ABM13W 32 MHz)
├── 3D/                                # Modèles CAO SolidWorks actifs (.SLDPRT/.SLDASM) + exports .step frais des 3 PCB
├── Datasheet/
│   └── V2/                            # Fiches techniques des composants de la V2
├── Commande/                          # Export panier composants (PDF / XLS)
└── Fabrication_assemblage/
    └── Notice_montage_montre.pdf      # Notice d'assemblage mécanique & électronique V3.8
```
