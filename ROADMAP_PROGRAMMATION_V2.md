# 🗺️ Roadmap de Programmation & Validation Matérielle — SmileWatch V2

> **Document de Référence R&D & Firmware**  
> *Date de mise à jour : 13 Novembre 2026*  
> *Cible matérielle : STM32WB55VGYx (Dual Core ARM Cortex-M4 @ 64 MHz + Cortex-M0+ BLE 5.4)*

---

## 🌊 1. Test d'Étanchéité Sans Risque (Protocole "Montre Vide")

Pour tester l'étanchéité réelle du boîtier **sans jamais risquer de noyer l'électronique**, voici le protocole horloger standardisé :

### Protocole du "Témoin Papier Buvard" (Zero Danger)
1. **Assembler la carrure et la base À VIDE** (sans PCB, sans batterie, sans mouvement Miyota) :
   - Glace saphir chassée avec son joint Hytrel/Téflon.
   - Poussoirs montés avec leurs joints d'étanchéité.
   - Verre capteur inférieur scellé à la colle UV.
   - Joint torique graissé (silicone horloger) et fond clipsé.
2. **Placer un buvard témoin à l'intérieur :**
   - Glisser un morceau de papier buvard ou de sopalin imbibé d'une goutte de colorant (ou papier réactif à l'eau) à l'intérieur du boîtier vide.
3. **Test à la cloche à dépression (Test d'air étanche) :**
   - À l'atelier de ton école ou chez un horloger : mise sous pression d'air (3 à 5 bars) dans une cloche de test horlogère sèche. Si le manomètre chute, il y a fuite, **sans que la montre n'ait touché une seule goutte d'eau**.
4. **Test en immersion (si cloche non disponible) :**
   - Plonger le boîtier vide dans un bac d'eau tiède à 30–50 cm de profondeur pendant 30 minutes.
   - Sortir, sécher soigneusement l'extérieur, ouvrir : si le buvard intérieur est sec à 100 %, l'étanchéité est validée.

### Faut-il commander une carrure et une base en plus ?
* **Base résine SLA :** **OUI, absolument (en commander 2 ou 3 d'office).** En résine SLA, le surcoût est dérisoire (quelques euros par pièce). En avoir une pour le test destructif/étanchéité et une pour l'assemblage final est la règle de l'art.
* **Carrure SLM 316L :** **NON indispensable si le budget est serré, mais recommandé si tu peux.** Comme le test se fait **à vide**, la carrure ne risque rien (l'inox 316L ne craint pas l'eau). Tu peux tester l'étanchéité du boîtier vide, le sécher, puis monter l'électronique dedans une fois le test réussi. Si le budget de ton projet/école le permet, avoir 2 carrures permet de tester l'usinage des portées sur la première avant de faire la pièce définitive.

---

## 🧲 2. Assemblage par Clipsage : Faisable à la main ?

* **Oui, mais avec une cale d'appui (ou presse manuelle d'établi) :**
  - Le clipsage manuel pur "au pouce" est risqué : pour assurer l'écrasement radial du joint torique et franchir le cran de clipsage sur toute la périphérie (Ø 38 mm), il faut une poussée axiale uniforme de l'ordre de **50 à 100 N (5 à 10 kg)**.
  - Si tu pousses avec les doigts de travers, la base polymère va se mettre en biais, coincer ou cisailler le joint torique.
* **La bonne méthode atelier :**
  - Poser la carrure face avant vers le bas sur le **posage O4** (pour ne pas rayer le saphir).
  - Poser la base au dos avec son joint torique huilé.
  - Utiliser une **petite presse d'horloger à crémaillère ou un étau doux avec mors téflon** pour appliquer une pression plane et progressive jusqu'au "clic" franc du verrouillage.

---

## 🚀 3. Roadmap Firmware Complète (Zero App Smartphone)

### 📌 Architecture Logicielle Dual-Core STM32WB55
```
┌────────────────────────────────────────────────────────┐
│  Cortex-M4 (Applicatif & Algorithmes)                  │
│  • Moteur pas-à-pas (Miyota 2035)                      │
│  • Traitement signal PPG (MAX30101) & Température     │
│  • UI Monochrome & Menus (OLED CH1115)                │
│  • Machine Learning Core & Gestes (LSM6DSOX)          │
│  • Fichier Flash Local (Historique Sport / Logs)       │
└──────────────────────────▲─────────────────────────────┘
                           │ IPCC (Mailbox Inter-cœur)
┌──────────────────────────▼─────────────────────────────┐
│  Cortex-M0+ (Stack RF & Profils BLE SIG Standards)     │
│  • CTS (0x1805) : Synchronisation de l'Heure           │
│  • ANCS (Apple) / ANS (Android) : Notifications        │
│  • HRS (0x180D) : Cardio direct vers Apple Santé       │
│  • HTS (0x1809) : Température médicale vers OS         │
│  • BAS (0x180F) : Widget Batterie smartphone           │
│  • HID : Contrôle Musique / Déclencheur Photo          │
│  • OTA : Mise à jour sans fil                          │
└────────────────────────────────────────────────────────┘
```

---

### ⏱️ Phase 1 : Boot & Horlogerie Autonome (Semaine 1)
- [ ] **Drivers GPIO & PWM Miyota 2035 :**
  - Génération d'impulsions bipolaires calibrées (31.2 ms, 1.4 V) + freinage dynamique `LOW-LOW`.
  - Mode avance rapide (train d'impulsions à 32 Hz) pour rattrapage horaire automatique.
- [ ] **Gestionnaire d'Heure RTC Interne :**
  - Base de temps haute précision sur quartz externe 32.768 kHz (`Y2`).
  - Décompte logiciel des positions d'aiguilles (suivi de la position théorique des aiguilles en RAM).
- [ ] **Profil Bluetooth CTS (Current Time Service - `0x1805`) :**
  - Récupération transparente de l'heure du téléphone à la connexion.
  - Calcul du différentiel et rotation automatique des aiguilles vers l'heure exacte sans intervention utilisateur.

---

### 📱 Phase 2 : Notifications & Affichage Micro-OLED (Semaine 2)
- [ ] **Driver I2C DMA Écran OLED CH1115 (88×48) :**
  - Gestion du framebuffer mémoire (528 octets de RAM).
  - Primitives graphiques (texte, icônes, jauges, animations de transition).
- [ ] **Service Apple ANCS & Android ANS :**
  - Écoute passive des notifications du smartphone.
  - Filtrage des alertes prioritaires (Appels, SMS, WhatsApp, Calendrier).
  - Dégagement temporaire des aiguilles (si besoin de lisibilité) et affichage du message sur l'OLED.
- [ ] **Rétroéclairage Automatique (VEML6030) :**
  - Ajustement automatique du contraste OLED selon la luminosité ambiante mesurée.

---

### 🩺 Phase 3 : Biomédical & Santé Native (Semaine 3)
- [ ] **Acquisition & Filtrage PPG (MAX30101) :**
  - Activation dynamique du convertisseur élévateur +5V `TPS61023`.
  - Filtrage passe-bande IIR pour extraction du rythme cardiaque (BPM).
  - Algorithme de calcul de la SpO2 (ratio Rouge/IR).
- [ ] **Mesure Température Clinique (TMP117) :**
  - Échantillonnage à basse consommation, précision ±0.1 °C.
  - Compensation thermique des dérives capteurs.
- [ ] **Diffusion Bluetooth Standard :**
  - Service **HRS (`0x180D`)** : injection directe du BPM dans Apple Santé / Google Health Connect.
  - Service **HTS (`0x1809`)** : injection directe de la température cutanée.
  - Détection automatique du port de la montre (On-Wrist Detection) : coupure des mesures si retirée.

---

### 🏋️ Phase 4 : Sport & Activité Autonome Sans Smartphone (Semaine 4)
- [ ] **Podomètre Matériel Autonome (LSM6DSOX) :**
  - Comptage des pas en tâche de fond ultra-basse consommation.
- [ ] **Mode Musculation Autonome :**
  - Détection automatique des mouvements périodiques via le Machine Learning Core (MLC).
  - Comptage des répétitions par série et chronomètre automatique du temps de repos sur l'OLED.
- [ ] **Mode Piscine / Natation :**
  - Détection des virages/poussées au mur pour incrémenter les longueurs de bassin.
  - Classification du style de nage (Crawl / Brasse).
- [ ] **Enregistrement en Mémoire Flash Interne :**
  - Sauvegarde locale automatique des séances (format compact d'historique).
  - Consultation des totaux directement sur l'écran OLED sans avoir besoin d'ouvrir un téléphone.

---

### ⚡ Phase 5 : Gestion d'Énergie & Finitions (Semaine 5)
- [ ] **Mode Veille Profonde (`STOP 2`) :**
  - Consommation cible en veille : `< 10 µA`.
  - Réveil instantané sur mouvement du poignet (*Raise-to-wake* via LSM6DSOX) ou sur appui bouton.
- [ ] **Gestion de Batterie (Profil BAS `0x180F`) :**
  - Lecture ADC tension batterie `PC4`.
  - Affichage direct du pourcentage dans le widget batterie natif du smartphone.
- [ ] **Contrôleur Multimédia HID :**
  - Appui bouton = Play/Pause musique ou déclencheur photo à distance.
- [ ] **Mise à Jour Sans Fil (OTA BLE DFU) :**
  - Capacité de mettre à jour le code STM32 par Bluetooth sans câble.
