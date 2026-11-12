# 🧠 Carnet de Réflexions, Idées & Points d'Attention — SmileWatch

> [!TIP]
> 📊 **Cockpit de Suivi Interactif Disponible** : Vous pouvez ouvrir directement dans votre navigateur web le tableau de bord interactif du projet : [suivi_projet.html](file:///c:/Users/fbois/Documents/Perso/Projet_Smilewatch/suivi_projet.html). Vous pourrez y cocher vos tâches en un clic avec mise à jour en temps réel de votre barre de progression et sauvegarde automatique !


> Ce document sert de **mémoire technique partagée** tout au long du projet.  
> On y consigne au fil de nos discussions toutes les idées, les choix techniques en suspens, les arbitrages méca/élec et les points d'attention cruciaux pour les étapes ultérieures (firmware, tests, industrialisation).

---

## ⚡ 1. Électronique & Routage PCB (À vérifier/arbitrer pour la V2)

- [x] **Connecteur d'Écran OLED CH1115 (VALIDÉ — SOUDURE DIRECTE 14P AU PAS DE 0.62 mm)** :
  - **Datasheet V2 inspectée** : Topwin `QG-8848TSWYG02` (contrôleur CH1115, 88×48). Nappe nue souple sans raidisseur rigide, 14 broches au pas propriétaire de 0.62 mm.
  - **Empreinte physique validée** : L'empreinte sur `Carte_V2.kicad_pcb` est géométriquement exacte (14 pastilles CMS au pas de 0.62 mm).
  - **Décision d'intégrité logicielle** : Maintien des noms originaux `C39` et `ConnectorMicromatch-14` pour éviter toute désynchronisation du schéma hiérarchique KiCad 10.

### Alimentation & Gestion d'énergie
- [x] **Bruit d'alimentation & Inductance Boost sur le MAX30101 (`11 Nov 2026` — Migré TPS61023)** : Sur `Carte_Capteur_V1`, le boost `MAX17222ELT+T` a été remplacé par **`TPS61023DRLR`** (`SOT-563`, `C919459`, stk `6 681`) avec **`L1 = 1.0 µH` (`DFE18SBN1R0ME0L`, `0603`, `C882989`, stk `2 188`)**, pont **`R23 = 150 kΩ` (`C25755`) / `R24 = 20 kΩ` (`C25765`)** ($V_{OUT} = 0.595 \times (1 + 150/20) \approx 5.06\text{ V}$), et `C18/C19 = 10 µF` (`C96446`).
- [x] **Consommation en veille (Quiescent Current — `11 Nov 2026`)** : Les LDOs `IC6`/`IC4` ont été basculés en BOM vers **`TPS7A0233DBVR` / `TPS7A0218PDBVR`** ($I_Q = 25\text{ nA}$), et le boost `TPS61023` (`EN`) ainsi que `U3` (`TCA9517D`) disposent de commandes dédiées (`/EN_5V_HR`, `/EN_TCA`).
- [x] **Mesure de tension batterie LIR3048 (`11 Nov 2026`)** : Pont diviseur `R25/R26 = 1 MΩ` (`C26083`) + filtrage `C56 = 100 nF` implanté sur `+3V8` vers **`PC4`** (`U2.G4`, fuite permanente ~`1.9 µA`).
- [x] **Sécurité & Polarité Batterie LIR3048 (`11 Nov 2026`)** :
  - Sur la `LIR3048` EEMB (p. 3), la coupelle cylindrique externe Ø30 mm est **`⊕` POSITIF** et le capot central étagé est **`⊖` NÉGATIF**.
  - Câblage corrigé sur `Carte_V2` (`B.Cu`) : ressort latéral `C22` (`S1921`) sur **`+3V8`**, pastille `TP8` (`6×6 mm`) sur **`GNDPWR`** (`TP2` supprimé). *Point d'attention audit : `TP8` est une pastille plate excentrée ($r = 11.05\text{ mm}$, emprise $R \approx 8.05\text{–}14.05\text{ mm}$) sans ressort central ($R \le 8.5\text{ mm}$).*
  - Hauteur `Batterie.SLDPRT` recalée à `4.90 mm` (`09/11`) avec calage Poron.

### Haute Fréquence, RF & Carrure métallique
- [x] **Arbitrage Carrure Métallique : Carrure Inox 316L Flottante (Abandon de la mise à la masse)** :
  - **Décision validée** : La carrure 316L ne sera **PAS** reliée au GND du circuit imprimé.
  - **Pourquoi ? (Gain colossal de fiabilité mécanique)** :
    - Élimine la colle conductrice époxy à l'argent sur le chant du PCB (qui s'effrite sous les chocs).
    - Évite d'ajouter des lames-ressorts mécaniques délicates ou des usinages supplémentaires.
    - Simplifie radicalement la procédure d'emboîtage de la notice de montage.
  - **Impact RF & Portée** :
    - Le plan de masse interne du PCB sert d'unique contrepoids RF pour l'antenne céramique Johanson 2450AT.
    - L'onde Bluetooth s'échappe principalement vers le haut à travers la **glace saphir** (et partiellement par le fond minéral).
    - L'atténuation supplémentaire liée à la proximité de la paroi d'acier flottante est de ~3 à 5 dB : la portée effective passe de ~10-15 m en champ libre à **~4 à 6 mètres**.
    - **Conclusion** : 4 à 6 m est largement suffisant pour une montre connectée (le smartphone étant au poignet, dans la poche ou sur le bureau à 1-2 m).
  - **Règle de routage impérative** : Conserver un dégagement maximal (au moins 1.0 à 1.5 mm de vide ou résine) entre l'antenne céramique et la paroi intérieure en acier pour limiter le désaccordage capacitif.
- [ ] **Ligne 50 Ohms** : Valider l'adaptation d'impédance entre le filtre intégré MLPF-WB-02D3 et l'antenne en fonction du stackup PCB JLCPCB.

### Débogage & Signaux
- [x] **DELs (LEDs) de débogage hardware : ABANDONNÉ / NON RETENU (Arbitrage encombrement)** :
  - **Décision validée** : Aucune DEL de débogage ne sera ajoutée sur `Carte_V2`.
  - **Pourquoi ? (Manque de place critique)** :
    - Le boîtier STM32WB55 WLCSP-100 présente une densité extrême avec un pas de billes de 0.4 mm.
    - Sans recours à des micro-vias borgnes/enterrés (procédé prohibitif en coût et complexité), les canaux de routage sur `F.Cu` et couches internes sont totalement saturés.
    - Tenter de sortir une broche comme `H1` (`PB1`) vers une DEL 0402 externe et sa résistance bloquerait le retour de masse et les alimentations sensibles.
  - **Stratégie de test retenue** : Le débogage initial est garanti par le connecteur de programmation SWD Tag-Connect (SWDIO, SWCLK, VCP UART) pour le flash initial et les traces console, puis par télémétrie sans fil en BLE OTA dès le coprocesseur M0+ opérationnel.
- [x] **Résistance 0 ohm / Shunt Miyota (TP6) : ABANDONNÉ / NON RETENU (Arbitrage encombrement)** :
  - **Décision validée** : Pas de résistance CMS 0402 de 0 Ω intercalée sur la ligne bobine Miyota.
  - **Pourquoi ?** : Absence d'espace physique disponible à proximité des pastilles de soudure de la bobine. Liaison directe conservée depuis le pont en H / driver.
  - **Sécurité** : La protection contre la saturation de la bobine est entièrement assurée par le firmware (timer matériel coupant l'impulsion motrice à 10–31.25 ms max et passage des GPIOs en haute impédance ou freinage dynamique LOW-LOW).
- [ ] **Protections ESD** : Diodes TVS sur les lignes GPIO des boutons poussoirs tactiles exposés à la manipulation.

### 🏁 Statut Carte Mère `Carte_V2` & Carte Capteurs `Carte_Capteur_V1` (Mise à jour 11 Novembre 2026)
- **Routage 6 couches complet (`Carte_V2`)** : Stackup JLCPCB JLC06161H-3313 (Top, In1.Cu, In2.Cu, In3.Cu, In4.Cu, Bottom).
- **Contrôles KiCad 10 propres (`11 Nov 2026`)** :
  - `Carte_V2` : **ERC = 0 erreur (`63` warnings)** ; **DRC + parité schématique = 0 violation, 0 non connecté, 0 parité**.
  - `Carte_Capteur_V1` : **ERC = 0 erreur (`10` warnings)** ; **DRC + parité schématique = 0 violation, 0 non connecté, 0 parité**.
  - `Cadran` : **DRC + parité = 0 erreur**, 0 non connecté (`55` warnings `silk_edge_clearance` liés aux index en bordure).
- **Correctifs RF & BOM passifs appliqués (`11 Nov 2026`)** : `R21 = DNP`, `R24 = DNP`, `Keep out area` `ANT1` sur les 6 couches, quartz `Y1` passé en `8 pF` (`ABM13W-32.0000MHZ-8-B1U-T5`, `C36/C50 = 12 pF C0G`), et tous les `JLCPCB Part#` passifs assainis (*compromis noté sur `C0G2_50V1` : X7R 50V `C76621` en `0603` faute de C0G 330 nF disponible dans ce boîtier*).
- **Prochaine étape** : Pause documentation terminée — revue indépendante complète par **Argon** et autres IA pour comparaison avant verrouillage mécanique (`P0-MECA-6/7`, `P1-MECA-9/10`), sourcing des ruptures JLCPCB et export Gerbers/STEP.
- [ ] **Résistances de tirage I2C (Pull-ups)** : Vérifier la valeur résultante des pull-ups sur le bus partagé après permutation `U3` (`OLED`, `TMP117`, `MAX30101`, `LSM6DSOX`, `VEML6030`, `DRV2605L`).

---

## ⚙️ 2. Mécanique, Horlogerie, Thermique & Matériaux

### Audit Critique de la Notice de Montage V3.8
- [ ] **Irréversibilité de la liaison inter-PCB (THT arasé)** : La notice prévoit d'insérer le peigne THT entre carte mère et carte capteurs, de souder par le dessous et d'araser. Sur un prototype, cela rend l'ouverture et le dépannage impossibles sans dessoudage destructif. Privilégier des micro-connecteurs mezzanine, ou tester 100% de la chaîne électrique avant de souder définitivement.
- [x] **Masse GND par colle époxy argent sur le chant du PCB (SUPPRIMÉ)** : Décision prise d'abandonner la liaison de masse avec la carrure (carrure flottante). On supprime donc totalement l'étape de dépose de colle époxy argent (Ligne 45 de la notice V3.8). Montage plus propre, plus rapide, sans risque d'effritement.
- [ ] **Soudure de la bobine Miyota** : Ne jamais toucher les micro-fils de 15 µm au fer à souder ; souder impérativement sur les languettes/plots métalliques du châssis de bobine.


### Maintien du Mouvement Miyota 2035 & Isolement Mécanique
- [ ] **Éviter la colle UV sur le Miyota** : 
  - Risque critique de capillarité vers les micro-engrenages, le rotor pas-à-pas Lavet ou les rubis, ce qui bloquerait le mouvement de façon irréversible.
  - Empêche tout démontage/remplacement en SAV.
- [ ] **Maintien isostatique** :
  - Privilégier un centrage radial par une bague/portée polymère (cercle d'emboîtage ou épaulement dans le cadran/cale).
  - Maintien axial (Z) par compression douce entre le cadran et le PCB supérieur à l'aide d'un coussinet amortisseur sans contraindre la rotation des aiguilles.
  - Étude de chaîne de cotes pour éliminer tout risque d'hyperstatisme (effort transmis sur les pivots ou le canon d'aiguilles).

### Capteur Thermique & Interface Peau (TMP117)
- [ ] **Fabrication du Disque Thermique** :
  - Pièce cylindrique traversante assurant le contact cutané.
  - Matériau recommandé : **Acier Inox 316L décolleté** (parfaite biocompatibilité cutanée, résistance à la sueur, aspect coordonné au boîtier) ou cuivre nickelé/doré.
- [ ] **Pad Élastomère Thermique & Taux de compression** :
  - Matériau type Bergquist Gap Pad (haute conductivité thermique > 3 W/m·K).
  - **Taux de compression cible : 20% à 30%**. En dessous de 10%, la résistance thermique de contact augmente ; au-dessus de 40%, la contrainte mécanique excessive risque de fissurer le PCB ou d'arracher les soudures du boîtier DFN du TMP117.

### Base Inférieure en Résine
- **Procédé de Fabrication Extérieur Confirmé (SLA / Stéréolithographie)** :
  - La pièce sera commandée en impression résine extérieure haute précision (ex: résine noire/opaque type ImagineBlack / JLCPCB SLA 8001 ou équivalent).
  - **Avantages critiques** :
    - Tolérance dimensionnelle de ±0.05 mm (indispensable pour la gorge du joint torique et le chassage du verre 12 mm).
    - Étanchéité intrinsèque de la matière (contrairement à l'impression FDM à filament dont les microporosités inter-couches fuient sous l'eau).
    - États de surface lisses au contact de la peau (aucun post-traitement de ponçage lourd).

- [ ] **Validation du matériau et procédé** :
  - Impression 3D stéréolithographie (SLA / DLP haute résolution).
  - Choix de résine : Privilégier une résine biocompatible peau certifiée ISO 10993 (type résine dentaire ou Formlabs BioMed Clear/Tough) ou usinage CNC en PEEK / Delrin (POM-C) pour éviter toute dermatite de contact avec la sueur et assurer une parfaite étanchéité du joint torique.

### Bobine Qi (Recharge sans fil)
- [ ] **Vérification du blindage et de l'isolant de la bobine** :
  - La bobine Würth 760308101309 possède une plaque de ferrite au dos (blindage magnétique indispensable face à l'électronique) et un adhésif de montage.
  - Les spires supérieures en cuivre émaillé sont affleurantes : l'ajout du disque isolant en Kapton (mentionné dans la notice V3.8) est **strictement nécessaire** pour éviter tout court-circuit contre la batterie ou des masses métalliques.

### Vibreur Haptique & Outillage Boutons
- [ ] **Mousse d'isolation du moteur haptique (DRV2605L / HD-EMB1204-SM)** :
  - Vérifier la référence exacte commandée dans le panier (mousse Poron microcellulaire haute densité).
  - Rôle : découpler les vibrations pour éviter les bruits parasites métalliques et orienter l'énergie vibratoire vers le poignet.
- [ ] **Taraudage des passages de boutons** :
  - Identifier le filetage exact requis (ex: M1.0, M1.2 ou M1.4) et commander le taraud machine ou main correspondant pour la reprise après impression 3D métal.

---

## ⏱️ 3. Pilotage du Mouvement Miyota 2035 (Motorisation & Électronique)

### Architecture d'Interfaçage GPIO ↔ Bobine Miyota (3.3V vs 1.55V)
- [ ] **Faut-il un Pont Diviseur de Tension (PDT) ? NON (Surtout pas de shunt à la masse) !** :
  - **Incompatibilité bipolaire** : Le moteur pas-à-pas Lavet requiert une alternance stricte de polarité ($H^+ > H^-$ à la seconde $N$, puis $H^- > H^+$ à la seconde $N+1$). Un pont diviseur conventionnel avec résistance de pied reliée au GND fausserait la symétrie de commande en tirant un des côtés vers la masse et créerait des fuites constantes.
  - **Deux solutions propres et élégantes** :
    1. **Option 1 : Résistance Série Unique ($R_{\text{série}} \approx 2.7\text{ k}\Omega$ à $3.3\text{ k}\Omega$)** :
       - La résistance interne de la bobine du Miyota 2035 est elle-même de **$R_{\text{bobine}} \approx 2.8\text{ k}\Omega$**.
       - En plaçant une simple résistance série $R_s$ de $2.7\text{ k}\Omega$ sur l'une des lignes (`GPIO_A --- [ 2.7k ] --- Bobine --- GPIO_B`), l'ensemble forme naturellement un diviseur de tension série sans aucune fuite vers la masse :
         $$V_{\text{bobine}} = 3.3\text{ V} \times \frac{2.8\text{ k}\Omega}{2.7\text{ k}\Omega + 2.8\text{ k}\Omega} \approx 1.68\text{ V}$$
         $$I_{\text{crête}} = \frac{3.3\text{ V}}{5.5\text{ k}\Omega} \approx 0.60\text{ mA}$$
       - Tension et courant correspondent exactement à la pile oxyde d'argent d'origine (1.55V - 1.65V, ~0.55 mA). L'inversion de polarité est 100% symétrique.
    2. **Option 2 : Commande Directe GPIO 3.3V par Réduction d'Impulsion Firmware (Zéro composant)** :
       - Le courant direct sous 3.3V est $I = 3.3\text{ V} / 2.8\text{ k}\Omega \approx 1.18\text{ mA}$. Le GPIO du STM32WB55 encaisse 8 à 20 mA sans aucune difficulté.
       - Le basculement du rotor dépend de l'impulsion de flux magnétique $\int I \cdot dt$. À courant double (~1.2 mA vs 0.55 mA), il suffit de réduire la durée de l'impulsion à **8 ms – 12 ms** (au lieu des 31.25 ms d'origine), ou d'appliquer un PWM à 50% sous 3.3V.
       - Avantage : Aucun composant externe, consommation d'énergie par pas rigoureusement identique (~39 µJ).
       - Précaution : Éviter une impulsion de 3.3V trop longue (ex: 32 ms) qui provoquerait un rebond mécanique du rotor contre les encoches du stator (bruit ou saut de 2 secondes au lieu d'une).

### Séquence de Commande & Freinage Électrodynamique
- [ ] **Machine d'états GPIO Push-Pull (Crucial pour la stabilité des aiguilles)** :
  - **Seconde Paire (Pas $N$)** : `H+` = HIGH (3.3V), `H-` = LOW (0V) pendant 10 à 30 ms.
  - **Inter-pas / Repos (970 à 990 ms)** : **`H+` = LOW et `H-` = LOW** (les deux sorties à 0V).
    - *Pourquoi LOW-LOW plutôt que High-Z ?* En reliant les deux extrémités de la bobine au GND, on court-circuite la bobine. Toute oscillation mécanique du rotor ou choc au poignet induit un courant de Lenz dans le circuit fermé qui amortit immédiatement le mouvement : c'est le **freinage électrodynamique**, indispensable pour éviter que la trotteuse ne tremble.
  - **Seconde Impaire (Pas $N+1$)** : `H+` = LOW (0V), `H-` = HIGH (3.3V) pendant 10 à 30 ms.
  - **Repos** : Retour à `H+` = LOW et `H-` = LOW.

### Avance Rapide & Calibration
- [ ] **Rattrapage d'heure automatique (Fast-forward)** : Fréquence d'impulsion maximale supportée par le train d'engrenages du Miyota 2035 (testée empiriquement entre 16 Hz et 32 Hz) pour synchroniser rapidement les aiguilles après reconnexion ou changement de fuseau horaire.
- [ ] **Zéro mécanique (Calibration initiale)** : Définir la procédure d'initialisation (alignement manuel à midi lors de l'assemblage ou calibration via boutons poussoirs).

---

## 📶 4. Bluetooth SIG & Expérience Utilisateur (Sans App)

### Synchronisation & Notifications
- [ ] **CTS (Current Time Service)** : Gérer la négociation automatique du fuseau horaire et de l'heure d'été/hiver dès la connexion Bluetooth avec le smartphone.
- [ ] **ANCS (Apple) / Notification Provider (Android)** : 
  - Gérer un buffer FIFO léger dans le Cortex-M4 pour tronquer intelligemment les notifications (expéditeur, sujet, aperçu du message) adaptées aux 88×48 pixels de l'écran.
  - Définir le filtrage des notifications (ex : appels entrants prioritaires avec vibration forte vs SMS discrets).
- [ ] **Appairage sécurisé (Bonding)** : Utiliser le protocole *Numeric Comparison* ou *Passkey Entry* en affichant le code à 6 chiffres sur l'écran micro-OLED.

### Santé & Mesures
- [ ] **Mode d'échantillonnage de la fréquence cardiaque (HRS)** : Mesure par salves périodiques (ex : 30 s toutes les 15 min ou sur détection d'activité par le LSM6DSOX) pour préserver les 280 mAh.
- [ ] **Mesure de température (HTS)** : Calibrer le temps de réponse thermique du pont métallique et appliquer l'algorithme d'estimation de la température centrale.

---

## 💡 5. Boîte à Idées & Fonctionnalités Futures

- [ ] **Geste "Lever de poignet" (Wrist Tilt)** : Allumer l'OLED sur interruption matérielle de l'IMU LSM6DSOX.
- [ ] **Mode "Survie Horlogère" (Batterie critique)** : Si batterie < 5%, couper OLED, capteurs et BLE pour ne garder que l'avance des aiguilles.
- [ ] **Profils de vibration haptiques** avec le DRV2605L (double clic subtil vs alarme).
- [ ] **Double tap pour réveiller** via accéléromètre.

---

## 🔋 6. Bilan d'Autonomie par Calcul (Batterie LIR3048 — 280 mAh)

### Hypothèses de calcul
* **Capacité utile** : 280 mAh × 90% = **252 mAh** (marge de décharge jusqu'au seuil de coupure LDO/MCU à ~3.0V).
* **Consommations unitaires modélisées** :
  * **Mouvement Miyota 2035** : impulsion de 31.25 ms @ 1.5V toutes les secondes -> ~7 µA moyen ramené au 3.7V de la batterie.
  * **STM32WB55 (Veille Stop 2 + RTC LSE 32.768 kHz)** : ~3.5 µA.
  * **BLE Connecté (Maintien de liaison)** : 
    * Intervalle 1s + Slave Latency 4 : ~20 µA.
    * Intervalle 500ms sans latence : ~35 µA.
  * **Podomètre LSM6DSOX (Accéléromètre seul @ 26 Hz)** : ~15 µA en continu 24/7.
  * **Écran PMOLED 0.5" (CH1115)** : ~12 mA en affichage actif.
  * **Cardiofréquencemètre MAX30101** : ~15 mA pendant une acquisition PPG (LEDs + conversion).
  * **Vibreur Haptique (DRV2605L)** : ~80 mA pendant 100 ms par vibration.
  * **Courants de fuite résiduels et LDOs** : ~30 à 40 µA.

---

### Synthèse des 4 Scénarios d'Autonomie

| Scénario | Profil d'utilisation | Courant moyen | Autonomie calculée |
| :--- | :--- | :--- | :--- |
| **1. Survie Horlogère** | Montre analogique seule (Aiguilles + RTC), BLE éteint, capteurs éteints, OLED éteint. | ~15.5 µA | **~670 jours (~22 mois)** |
| **2. Hybride Éco** | Aiguilles + BLE connecté (1s/latence), podomètre 24/7, cardio 1x/heure (30s), 20 notifications/jour (5s OLED + vibreur). | ~222 µA | **~47 jours (~1.5 mois)** |
| **3. Hybride Standard** | Aiguilles + BLE connecté (500ms), podomètre 24/7, cardio toutes les 15 min (30s), 50 notifs/jour + 30 réveils poignet (10 min écran total/j). | ~696 µA | **~15 jours** |
| **4. Intensif / Sport** | Cardio optique continu (100%), BLE rapide, écran consulté très fréquemment. | ~8.5 mA | **~30 heures** |

> **Le levier numéro 1 de l'autonomie** : L'échantillonnage du capteur cardiaque `MAX30101`. À 15 mA en pic, faire une mesure toutes les 15 minutes au lieu du continu fait passer l'autonomie de 30 heures à **15 jours**.
>
> ✅ **Mise à jour (11 Nov 2026 — LDO `TPS7A02`)** : Remplacement *pin-to-pin* appliqué dans les schémas & PCB `Carte_V2` (`IC6 = TPS7A0233DBVR` / `C5142805` stk `12,659`, `IC4 = TPS7A0218PDBVR` / `C3748843` stk `3,217`, $I_Q = 25\text{ nA}$), restaurant le potentiel d'autonomie *Survie Horlogère* (~670 j ; noter la fuite permanente résiduelle ~`1.9 µA` du pont `R25/R26 = 2×1 MΩ` sur `PC4`).

---

## 📡 7. Robustesse Bluetooth : Que se passe-t-il avec une carte "pas tout à fait réussie" ?

### Bilan de liaison théorique (Link Budget)
* **Puissance d'émission ($P_{TX}$)** STM32WB55 : programmable jusqu'à **+6 dBm** (retenons **+4 dBm**).
* **Sensibilité de réception smartphone ($P_{RX}$)** : **~ -95 dBm** (valeur typique iPhone / Android récent en 1 Mbps).
* **Budget de liaison disponible** : 4 - (-95) = **99 dB**.

### Atténuations réelles dans l'environnement de la montre

1. **Pertes en espace libre ($FSPL$) à 2.44 GHz** :
   * À 3 m : -50 dB
   * À 5 m : -54 dB
   * À 10 m : -60 dB
2. **Effet de la carrure métallique Inox 316L** : L'antenne est enchâssée dans du métal. L'onde ne sort que par le verre saphir et le fond résine -> **-10 à -14 dB** de perte de directivité.
3. **Absorption par le corps humain (poignet)** : Le corps absorbe fortement le 2.4 GHz dans la direction opposée au verre -> **-10 à -15 dB**.
4. **Pénalité pour carte imparfaite (détuning / désadaptation RF)** :
   * Ligne 50 Ohms non idéale, proximité du plan de masse : return loss dégradé de -15 dB à -6 dB -> **-3 dB** de perte par désadaptation.
   * Chute de rendement de l'antenne céramique 2450AT -> **-3 à -5 dB**.
   * **Total pénalité carte imparfaite : -6 à -8 dB**.

---

### Marge résiduelle et Portée estimée

$$\text{Marge} = \text{Budget de liaison (99 dB)} - [FSPL + \text{Boîtier (12 dB)} + \text{Corps (12 dB)} + \text{Carte imparfaite (7 dB)}]$$

* **À 3 mètres (Critique utilisateur)** :
  * Pertes totales ≈ 50 + 12 + 12 + 7 = 81 dB.
  * **Marge restante : +18 dB**.
  * **Verdict** : **Ultra-robuste (> 99.9% de paquets reçus)**. Même avec un routage RF aapproximatif et la montre plaquée contre le corps, la liaison tiendra sans aucune déconnexion.
* **À 5 mètres (Cible idéale utilisateur)** :
  * Pertes totales ≈ 54 + 12 + 12 + 7 = 85 dB.
  * **Marge restante : +14 dB**.
  * **Verdict** : **Très bon fonctionnement dans la même pièce**. Les retransmissions BLE absorberont d'éventuelles micro-coupures de manière totalement transparente pour les notifications.
* **À 10 mètres (Distance optimale)** :
  * Pertes totales ≈ 60 + 12 + 12 + 7 = 91 dB.
  * **Marge restante : +8 dB** en champ direct.
  * **Verdict** : Réalisable en visibilité directe sans obstacle. Si le corps de l'utilisateur ou une cloison en plâtre s'intercale, le signal chutera près du plancher de bruit (-95 dBm), causant des pertes temporaires de paquets.

### Recommandations firmware pour bétonner la liaison
* Configurer le **`Supervision Timeout` BLE à 4 ou 5 secondes** (au lieu de 1 ou 2 s) pour que la montre ne se déconnecte pas si vous passez rapidement derrière un mur ou à plus de 6-7 mètres.
* Activer la puissance d'émission STM32WB à **+4 dBm ou +6 dBm** (le surcoût en courant pendant les microsecondes d'émission est négligeable sur la moyenne globale).

---

## ⏳ 8. Scénario Décharge Profonde & Stockage Prolongé ("Sortie du Placard")

### 1. Que se passe-t-il pendant l'oubli prolongé dans le placard ?
1. **Arrêt du système (~3.0V)** : Lorsque la batterie descend vers 3.0V, le superviseur de tension BOR (Brown-Out Reset) du STM32WB55 fige le MCU. Le moteur Miyota s'arrête (les aiguilles se figent à leur position courante).
2. **Décharge lente puis coupure de sécurité (~2.5V)** :
   - L'autodécharge naturelle de la chimie Li-Ion (~2-3% par mois) continue de vider lentement la cellule.
   - Dès que la tension passe sous le seuil critique ($V_{ODP} approx 2.5 \text{V}$), le composant de protection **`AP9101C` coupe immédiatement le MOSFET de décharge**.
   - **Protection active** : La batterie est totalement isolée du reste du circuit. Le courant résiduel de veille de l'AP9101C est inférieur à 100 nA. Cela protège la cellule LIR3048 contre la dégradation chimique irréversible pendant 1 à 2 ans.

### 2. Que se passe-t-il quand on pose la montre sur le chargeur sans fil Qi ?
1. **Démarrage à froid sans batterie (Zero-Power Boot Qi)** :
   - La bobine Würth reçoit le champ magnétique de la base Qi.
   - Le récepteur **TI BQ51003** s'auto-alimente directement depuis l'énergie inductive (il n'a pas besoin de batterie pour fonctionner).
   - Il négocie le protocole Qi et délivre du 5V régulé sur sa sortie `OUT`.
2. **Régénération douce par le chargeur TI BQ24040** :
   - Le BQ24040 détecte une tension de batterie inférieure à 2.8V.
   - Il entre automatiquement en **mode Pré-charge (Trickle Charge)** à ~10-20 mA pour réveiller la chimie sans échauffement.
   - Dès que la tension remonte au-dessus du seuil de recouvrement ($V_{ODR} approx 2.9 \text{V}$), l'AP9101C referme le MOSFET : la charge principale (Fast Charge) prend le relais.
3. **Délai de boot système (~2 à 5 minutes)** :
   - La montre n'ayant pas de circuit "Power-Path" dédié, le MCU attend que la batterie remonte au-dessus du seuil de reset BOR (~3.0V).
   - Au bout de quelques minutes, les LDOs délivrent un 3.3V stable et le STM32WB démarre proprement (Power-On Reset).

### 3. Comment les aiguilles et l'heure se recalibrent-elles ?
* **Perte de l'heure interne (RTC)** : Le calendrier interne est réinitialisé. Dès que le Bluetooth s'active et se reconnecte au smartphone, le service **CTS (Current Time Service)** injecte immédiatement l'heure et la date courantes.
* **Problème du Miyota 2035 (Pas de capteur de position absolu)** : Le MCU ne sait pas à quelle heure les aiguilles se sont arrêtées.
* **Stratégie firmware recommandée** :
  - L'écran OLED s'allume avec un message : *"Calibration aiguilles : Positionner à 12:00"*.
  - L'utilisateur utilise les 2 boutons pour faire avancer les aiguilles à midi pile (ou un appui long pour l'avance rapide).
  - Une fois validé à 12:00:00, le MCU calcule le delta avec l'heure CTS du smartphone et fait tourner les aiguilles en avance rapide (train d'impulsions à 20-30 Hz) jusqu'à l'heure exacte en quelques secondes.

---

### ⚖️ Matrice de Charge & Équilibre : Smilewatch V2 vs Coupe de France de Robotique (CdF)

> [!IMPORTANT]
> **Clarification des Projets Concomitants** :
> 1. **Coupe de France de Robotique (CdF 2026-2027 — DEE_2701) — GROS ROBOT ARES** :
   - **Échéance académique officielle** : **Soutenance le Mardi 15 Décembre 2026 à 13h30**.
   - **Projet réel** : Le **Gros Robot** officiel de cette année 2026-2027 (et non le PAMI de l'an dernier !).
   - **Dépôt local exact** : `D:\ENSEA_2A\2627_Projet2A_CDFR_ARES` (Remote GitHub : `https://github.com/YaayY360/2627_Projet2A_CDFR_ARES.git`, propriétaire : **YaayY360 / Yanis Grave**).
   - **Rôle de François (Pôle Mécanique avec Alexis Morot-Gaudry)** :
     - Conception 3D complète du système de tir de balles (`Robot/Mechanic/Balle.SLDPRT`, rails).
     - Conception et dimensionnement du bras robotique 4 axes (`Dimensionnement_du_bras.py`, servomoteurs Dynamixel XL430, préhenseur ventouse).
     - Programmation du bras 4 axes (cinématique inverse & pilotage STM32).
     - Management transverse de l'équipe (électronique, IHM, SOD, stratégie).
> 2. **Smilewatch V2** : Projet de montre connectée hybride mené en parallèle. L'objectif est d'atteindre l'assemblage complet et le flashage OTA d'ici mi-décembre, avec une **semaine tampon de secours en Janvier 2027** en cas d'imprévu.

#### Règles de Gestion du Temps & Contraintes ENSEA
* **Lieux & Horaires de travail (Règle d'accès stricte)** :
  - **Règle d'or ENSEA** : L'école ferme ses entrées à **20h00 pile** (au-delà, les badges extérieurs sont bloqués ; sortie possible jusqu'à 21h30 du lundi au jeudi et 20h00 le vendredi).
  - **Conséquence directe pour la piscine (18h30 - 19h45)** : En sortant de la piscine à 19h45, on ne tente pas de réintégrer l'école : **on rentre directement chez soi** pour travailler sur son PC personnel au calme.
  - *Midis (12h00 - 13h30)* : Sanctuaire pause déjeuner / repos (zéro travail projet, indispensable pour tenir le rythme).
  - *Jeudi après-midi* : Réservé à l'encadrement des étudiants de 1ère année (bloqué).
  - *Cours Magistraux (CM)* : Séchables stratégiquement pour libérer des blocs continus de FabLab/atelier quand une urgence d'usinage ou de routage l'exige.
* **Charge Académique Incompressible** :
  - **Chaque séance de TP** = 2h de préparation amont + 1h de compte-rendu aval.
  - **Chaque Partiel** = **10h de révision bloquées** dans les 3-4 jours précédant l'épreuve :
    - *Examen Signal (Lundi 30 Nov à 13h30)* : 10h réparties entre le 26 et le 29 Nov.
    - *Examen Automatique (Lundi 07 Déc à 13h30)* : 10h réparties entre le 03 et le 06 Déc.
* **Piscine & Équilibre Sportif (2 séances par semaine)** :
  - **Mardi soir de 18h30 à 19h45** à la *Piscine du Parvis de Cergy* (décompression post-FabLab).
  - **Jeudi soir de 18h30 à 19h45** à la *Piscine du Parvis de Cergy* (décompression post-tutorat 1ère année).
  - 12 séances au total synchronisées dans Google Agenda jusqu'à mi-décembre !
* **Délais Industriels & Réusinage CNC (Smilewatch)** :
  - *Commande PCBA & SLM 316L* : Dimanche 01 Novembre.
  - *Réception PCBA JLCPCB* : ~16 Novembre ➔ Flashage Tag-Connect sur table à la maison.
  - *Réception SLM 316L brute* : ~18 Novembre ➔ **Dépôt immédiat en réusinage CNC de précision (délai 8-10 jours ouvrés)**.
  - *Récupération carrure CNC finie* : ~28 Novembre - 02 Décembre ➔ Emboîtage mécanique final en Semaine 7.

---

## 📅 9. Planning Révisé : Aligné sur l'Emploi du Temps ENSEA (Soutenance S7 le 15 Décembre)

> **🎯 Cible Absolue** : **Mardi 15 Décembre 2026 à 13h30 — Soutenance du Projet S7 (DEE_2701)** devant le jury ENSEA !  
> **Contrainte de Conception** : La montre doit être **100% assemblée, étanche et fermée** au poignet lors de la soutenance, avec le **Bootloader BLE OTA** actif pour faire une démonstration en direct (mise à jour d'un écran ou d'une fonction sans fil depuis le smartphone devant le jury).  
> **Buffer de sécurité** : Une semaine de secours optionnelle est identifiée début Janvier 2027 (04 au 09 Janvier) en cas d'ajustements firmware finaux.

---

### Le Compte à Rebours Structuré (Synchronisé Google Agenda & FabLab)

```text
S1 (26 Oct - 01 Nov) [Vacances Toussaint] : Figeage KiCad V2 + Cadran FR4 + Lancement Commandes
S2 (02 Nov - 08 Nov) [FabLab Mar 03 Nov]  : Suivi PCBA JLCPCB + Finition Posage O4
S3 (09 Nov - 15 Nov) [FabLab Mar 10 Nov]  : Banc de test de table + Configuration CubeWB BLE_Ota
S4 (16 Nov - 22 Nov) [FabLab Mar 17 Nov]  : Réception PCB + 1er Flash SWD Tag-Connect (Pile RF + OTA)
S5 (23 Nov - 29 Nov) [FabLab Mar 24 Nov]  : JALON DÉCISIF — Validation Flash BLE OTA sur table
S6 (30 Nov - 06 Déc) [FabLab Mar 01 Déc]  : Réception SLM 316L + Montage mécanique à blanc
S7 (07 Déc - 13 Déc) [FabLab Mar 08 Déc]  : Assemblage horloger final Notice V3.8 + Fermeture étanche
S8 (14 & 15 Déc)     [Mardi 15 Déc 13h30] : SOUTENANCE PROJET S7 ENSEA DEVANT LE JURY !
Secours (04-09 Jan)  [Optionnelle]        : Buffer de réserve post-Noël si finitions nécessaires
```

---

### Détail Semaine par Semaine & Créneaux FabLab

#### 📅 Semaine 1 (26 Octobre - 01 Novembre) : Figeage & Commandes (Vacances Toussaint)
* **Plein temps disponible (Vacances)** :
  - **Lundi 26 Oct** : Inventaire des pièces en stock, mesure de la nappe OLED au pied à coulisse, fix DRV2605L (`A2` vers GND), empreinte résistance Miyota `TP6`, 2 DELs de debug.
  - **Mardi 27 Oct** : Conception KiCad Cadran FR4 0.4 mm (index or ENIG, fenêtre OLED, canon d'aiguilles).
  - **Mercredi 28 Oct** : Revue STEP carrure 316L (+0.3 mm surépaisseur joints) & devis SLM.
  - **Jeudi 29 Oct** : Commandes taraud boutons, pads thermiques et consommables.
  - **Samedi 31 Oct** : Contrôle visuel Gerbers & stocks SMT LCSC.
  - **Dimanche 01 Nov (Jalon Critique)** : Validation et paiement du panier JLCPCB PCBA + SLM 316L avant reprise des cours lundi.

#### 📅 Semaine 2 (02 - 08 Novembre) : Suivi Production & Posage O4
* **Séance Projet FabLab (Mardi 03 Nov 13h30 - 17h30)** :
  - Suivi de la validation des fichiers de production chez JLCPCB (DFM review).
  - Nettoyage et ébavurage du posage horloger O4 imprimé 3D au FabLab.
  - Préparation de l'outillage horloger (brucelles d'horloger, loupe, cabrons).

#### 📅 Semaine 3 (09 - 15 Novembre) : Banc de Test & Firmware Minimal
* **Lundi 09 Nov (08h30 - 10h30, Agenda Google)** : Reprise SolidWorks du montage d'assemblage suite à la détection de l'erreur de montage sur l'empilement des aiguilles Miyota 2035 et ajustement de la chaîne de cotes axiale avant validation finale.
* **Séance Projet FabLab (Mardi 10 Nov 13h30 - 17h30)** :
  - Montage du banc de test électrique de table (alimentation régulée 3.7V, multimètre de précision).
  - Configuration du projet STM32CubeIDE sur la base de l'exemple officiel ST `BLE_Ota`.
  - Test de la sonde ST-Link et du câble Tag-Connect TC2030 sur carte de test.

#### 📅 Semaine 4 (16 - 22 Novembre) : Réception PCB & 1er Flash SWD (Jalon Majeur)
* **Séance Projet FabLab (Mardi 17 Nov 13h30 - 17h30)** :
  - Réception du colis PCBA JLCPCB (Carte Mère, Carte Capteurs, Cadran FR4).
  - Contrôle qualité sous binoculaire (alignement BGA/QFN, absence de ponts d'étain).
  - Premier allumage sous alimentation de laboratoire avec limitation de courant (50 mA).
  - Flashage initial via Tag-Connect : Coprocesseur M0+ (pile RF Bluetooth ST) + Bootloader OTA.
  - Test de l'allumage des 2 DELs de debug (Heartbeat vert).

#### 📅 Semaine 5 (23 - 29 Novembre) : Validation Flash Sans Fil BLE OTA (Jalon Décisif)
* **Séance Projet FabLab (Mardi 24 Nov 13h30 - 17h30)** :
  - **L'instant de vérité** : Connexion Bluetooth avec le smartphone via *ST BLE Sensor* / *nRF Connect*.
  - Téléversement d'un nouveau firmware applicatif **100% sans fil en Bluetooth**.
  - Validation du pilotage de la trotteuse du Miyota (impulsion 10 ms + freinage électrodynamique LOW-LOW).
  - Validation de l'affichage sur l'OLED CH1115 (logo Smilewatch).
  - *Note : Weekend à Rennes du 27 au 29 Novembre (Anniversaire).*

#### 📅 Semaine 6 (30 Novembre - 06 Décembre) : Réception SLM 316L & Montage Mécanique
* *Attention : Examen Signal DTI_2201/2202 le Lundi 30 Nov à 13h30.*
* **Séance Projet FabLab (Mardi 01 Déc 13h30 - 17h30)** :
  - Réception de la carrure 316L imprimée métal et rectifiée CNC.
  - Contrôle dimensionnel des portées de joints toriques et de la portée de glace.
  - Montage à blanc mécanique dans le Posage O4 : vérification des jeux internes et taraudage des vis de poussoirs.

#### 📅 Semaine 7 (07 - 13 Décembre) : Assemblage Final Notice V3.8 & Fermeture Étanche
* *Attention : Examen Automatique DEE_2401 le Lundi 07 Déc à 13h30.*
* **Séance Projet FabLab (Mardi 08 Déc 13h30 - 17h30)** :
  - **Assemblage final complet selon la Notice V3.8** :
    1. Pose du cadran FR4, de l'écran OLED et emboîtage des aiguilles du Miyota.
    2. Liaison inter-PCB, mise en place de la batterie LIR3048 et de la bobine Qi.
    3. Pose du joint torique et fermeture étanche du fond de boîte.
  - Test ultime : recharge sans fil sur socle Qi et mise à jour firmware OTA sur la montre définitivement fermée !

#### 📅 Semaine 8 (14 - 15 Décembre) : Répétition & Soutenance Finale S7
* **Lundi 14 Décembre (18h00 - 20h00)** : Répétition générale de la présentation orale et préparation des slides (architecture, défis RF 316L, compromis Lavet, démonstration en direct).
* **Mardi 15 Décembre à 13h30** : 🎓 **SOUTENANCE PROJET S7 DEVANT LE JURY ENSEA (DEE_2701)** !
  - Démonstration live : montre au poignet, fonctionnelle et étanche, téléversement d'un affichage en Bluetooth devant le jury !

#### 🛡️ Semaine de Secours Optionnelle (04 - 09 Janvier 2027)
* Semaine de contingence tampon post-fêtes si un ajustement logiciel ou des mesures complémentaires pour le rapport final sont requis.

---

## 🔍 10. Audit Détaillé des Pinouts KiCad vs Datasheets (Révisé le 08 Nov — MàJ Correctifs le 11 Nov 2026)

### ✅ ANOMALIE OCTOBRE CORRIGÉE SUR PCB :
* **`IC8` (Driver Haptique TI DRV2605L - Fichier `Moteur.kicad_sch` / `Carte_V2.kicad_pcb`)** :
  * **Statut vérifié le 08 Nov 2026** : **CORRIGÉ ✅**. Sur `Carte_V2.kicad_pcb`, la bille **A2 (`REG`)** est désormais bien isolée sur `Net-(IC8-REG)` et reliée uniquement au condensateur `C34 = 1 µF` vers `GND`.

---

### 🔍 VÉRIFICATIONS PINOUTS (MISES À JOUR SUITE À L'AUDIT 360° DU 08 NOV 2026) :

1. **Microcontrôleur STM32WB55VGYx (Boîtier WLCSP-100)** :
   * **Ligne RF (✅ CORRIGÉ LE 11 NOV)** : Broche K4 (`RF1`) -> `MLPF-WB-02D3` (`FL1`) -> `ANT1` (`2450AT07A0100`). **`R21` et `R24` passés en `DNP`**, et zone **`Keep out area`** sans cuivre/pistes/vias/composants créée sur les **6 couches** (`F.Cu..B.Cu`, `5.0 × 5.05 mm`) autour de `ANT1`. *(Attention appro : `FL1 C5270481` et `ANT1 C17531353` à stock `0` chez JLCPCB).*
   * **Connecteur SWD Tag-Connect TC2030 (J1)** : 1=VCC (+3V3), 2=SWDIO (PA13), 3=NRST, 4=SWCLK (PA14), 5=GND, 6=SWO (PB3). **Brochage rigoureusement identique au standard Tag-Connect**.
   * **Oscillateurs (✅ CORRIGÉ POUR 8 pF — ⚠️ `Y1.4` NC)** : Quartz 32.768 kHz `ECS-.327` sur PC14/PC15 OK. Quartz HSE 32 MHz `Y1` remplacé par **`ABM13W-32.0000MHZ-8-B1U-T5`** ($C_L = 8\text{ pF}$, `C6455449`, stock JLCPCB `0`) avec `C36 = C50 = 12 pF C0G` (`C1547`). *Point mineur : `Y1.4` reste non connecté (`NC`) au lieu d'être relié à `GND`.*
   * **Bus I2C1** : PB8 (`SCL`) et PB9 (`SDA`). **Conforme aux fonctions alternatives STM32WB**.
   * **VDDUSB & Mesure VBAT (✅ CORRIGÉ LE 11 NOV)** : `VDDUSB` à `GND` conforme ST ; pont diviseur **`R25/R26 = 1 MΩ` (`C26083`) + `C56 = 100 nF`** implanté de `+3V8` vers **`PC4`** (`U2.G4`) pour télémétrie batterie `BAS`.

2. **Capteurs & Module Déporté (`Carte_Capteur_V2`)** :
   * **LSM6DSOX (IMU 6 axes)** : CS à +3V3 (active le mode I2C). SDO/SA0 à GND (adresse I2C = 0x6A). INT1 et INT2 reliés au MCU. **Parfait**.
   * **VEML6030 (Luminosité)** : ADD à GND (adresse I2C = 0x10). INT vers MCU (PA12). **Parfait** *(`Canal_lum.SLDPRT` retouché le `09/11`, à confirmer en vérification d'interférences `Montre.SLDASM`)*.
   * **TMP117 (Température clinique — ✅ CORRIGÉ LE 11 NOV)** : ADD0 à GND (0x48), ALERT vers MCU. **`Pad 7` isolé relié par 2 vias thermiques à une pastille ENIG dédiée (`1.2×1.8 mm` sur `B.Cu`) + zone keepout `U2_Thermal_Iso` sur `In1.Cu/In2.Cu`**.
   * **MAX30101 (Cardio/SpO2)** : VLED+ sur rail +5V dédié, VDD sur +1V8 (`C2859066`, stk `65`).
   * **TPS61023DRLR (Boost +5V — ✅ MIGRÉ & REROUTÉ LE 11 NOV / ⚠️ MÉCA & BUDGET 3V3)** : Remplacement du `MAX17222` par **`TPS61023DRLR` (`SOT-563`, `C919459`, stk `6 651`)**, **`L1 = 1.0 µH` `0603` (`DFE18SBN1R0ME0L`, `C882989`, stk `2 188`, $I_{\text{sat}}=1.9\text{ A}$, $DCR=0.12\ \Omega$)**, pont **`R23 = 150 kΩ` / `R24 = 20 kΩ`** (`C25755` / `C25765`, $V_{\text{OUT}} \approx 5.06\text{ V}$ typ PWM / `5.11 V` PFM) et `C18/C19/C26 = 10 µF 25V X5R` (`C96446`, ~`8–12 µF` eff. sous 5 V `≥ 4 µF` min). Alimenté par `+3V3` (`IC6 TPS7A0233` `200 mA` max) ➔ compatible pulses `MAX30101` (`≤ 50 mA`). *Reste à confirmer en 3D sur `.STEP` frais : dégagement radial de `IC3/L1` face à la jupe Ø38 mm (`P1-MECA-10`).*
   * **TCA9517DR (Répéteur I2C — ✅ CORRIGÉ & CONFIRMÉ EN STOCK LE 11 NOV)** : Permutation complète effectuée sur schéma et PCB `Carte_Capteur_V1` (`#FLG03` supprimé, re-routage propre à **0 erreur DRC**) : **Side A (`VCCA = +1V8`, `SDAA/SCLA`)** côté `MAX30101` et **Side B (`VCCB = +3V3`, `SDAB/SCLB`)** côté `STM32WB55`. Référence `C182421` vérifiée en stock JLCPCB (**`5 738` pcs**) — pas de changement vers `PCA9517DR` (`C51269` à stock `0`).

3. **Alimentation, Écran OLED & Charge Qi (`Alimentation_sch.kicad_sch` / `Ecran.kicad_sch`)** :
   * **Batterie `LIR3048` (✅ KICAD CORRIGÉ LE 11 NOV — ⚠️ POINT DE VIGILANCE)** : Sur `Carte_V2` (`B.Cu`), `BT1` (ex-`C22`, `S1921`, latéral, $r=14.95\text{ mm}$) est désormais relié à **`+3V8`** (`⊕`), `TP2` est supprimé et **`TP8`** (`B.Cu`, `6×6 mm`) est relié à **`GNDPWR`** (`⊖`). *Vigilance pour audit Argon : `TP8` est une pastille plate excentrée ($r=11.05\text{ mm}$, emprise $R\approx 8.05\text{–}14.05\text{ mm}$) et non un contact ressort centré ($R\le 8.5\text{ mm}$) ; vérifier l'alignement avec le joint de sertissage du capot `⊖`.*
   * **Étage Écran OLED `Q1`/`Q2` (✅ ÉLEC CORRIGÉ LE 11 NOV — 🔴 MÉCA `P0-MECA-7` OUVERT)** : `Q2.2` (`FDN335N`) relié à `GND`, `Q1.2` (`FDN338P`) + `R15.1` reliés à `+3V8`, `Q1.3` Drain vers `/Ecran/VBAT`, et `R13 = 560 kΩ` (`C132339`). *Collision mécanique `P0-MECA-7` encore ouverte : `Q1`, `Q2` (`TSOT-23`) et `C6/C7/C8/C20/C41/C42/C43` (`0603`) sont toujours implantés sur `F.Cu` sous la dalle OLED.*
   * **Récepteur Qi BQ51003 (⚠️ VÉRIF `C21120`)** : `R7 = DNP` et **`R5 = 1.5 kΩ`** (`C25867`, $I_{\max} \approx 155\text{ mA}$ > `119.2 mA` du `BQ24040`). `C0G2_50V1 = 220 nF` (`C21120 = CL10B224KA8NNNC`, **25 V X7R ±10 %** au lieu de `C0G 50 V` — valeur `220 nF` accordée à la bobine `760308101309` `10 µH` + `C0G3 = 2.2 nF 50V C0G` `C2987940`, mais tenue/pertes `X7R 25V` à valider), `R27 = 10 kΩ` (`C25744`, remplace `TH2`), `TH1 = 10k NTC` (`C91636`, stk `68`).
   * **Chargeur BQ24040** : $R_{ISET} = 4.53\text{ k}\Omega \implies I_{CHG} \approx 119.2\text{ mA}$, $R_{PRETERM} = 2.0\text{ k}\Omega \implies I_{TERM} \approx 12\text{ mA}$, `~CHG` sur MCU. **Parfait**.
   * **Protection AP9101CAK & Double MOSFET DMN2028UFU (✅ `CAK` Auto-Wake-Up)** : **`C46 = 100 nF`** (`C14663`), **`R16 = 330 Ω`** (`C25104`), `R17 = 2 kΩ` (`C4109`) conformes à la Note 4 (`R1=330–470Ω`, `C1=10–1000nF`, `R2=300Ω–4kΩ`). **`U4 = AP9101CAK-ATTRG1`** (`C5588142`, **`stk=162`** en stock JLC au 12 Nov, `Q3 = DMN2028UFU-13` `C17388236`) : seuils `VCU=4.400 V / VDL=2.800 V / VDU=3.000 V`, variante **`CAK` = Auto-Wake-Up actif** + charge `0 V` autorisée (démarrage direct dès pose de la batterie).

## 📦 11. Bilan d'Approvisionnement & Inventaire Réel (Objectif : 2 Montres SmileWatch)
*Mis à jour le 11 Novembre 2026 (vérification temps réel des stocks JLCPCB SMT & commandes Sparks).*

### Synthèse de Disponibilité pour 2 Exemplaires

| Désignation | Référence Technique / Détails | Quantité / Statut | Action Requise |
| :--- | :--- | :--- | :--- |
| **Écran micro-OLED 0.5"** | Topwin `QG-8848TSWYG02` (Contrôleur CH1115, 88×48) | ✅ **Reçu (x2)** | Datasheet V2 analysée : 14 broches au pas de 0.62 mm. Soudure directe confirmée. |
| **Batterie Rechargeable** | LIR3048 Li-ion (3.7V - 280 mAh) | ✅ **Reçu (x2)** | Stock OK. Prête pour l'intégration mécanique. |
| **Mouvement Quartz** | Miyota 2035 (2 fils de bobine à souder) | ✅ **2 en stock atelier + 2 neufs commandés (Sparks, 09 Nov)** | Expédition Sparks en cours. |
| **Aiguilles Horlogères** | Aiguilles Heures & Minutes pour Miyota 2035 | ✅ **Reçu (x2 sets)** | Stock OK. Pose au Bergeon 6767 lors de la phase 1. |
| **Verre Principal Supérieur** | Glace Saphir plat haute résistance | ✅ **Reçu** | Stock OK pour le chassage avec posage O4. |
| **Joint en I supérieur** | Joint Téflon / Hytrel profil en I | ✅ **Reçu** | Stock OK pour l'étanchéité du verre supérieur. |
| **Verre Fond (Bottom) 12 mm** | Glace minérale trempée Ø12 mm | ✅ **Reçu** | Fenêtre optique capteur cardio MAX30101 & TMP117. |
| **Boutons Poussoirs** | 4 boutons poussoirs physiques latéraux | ✅ **Reçu (4 pièces)** | 2 boutons par montre. Prêts pour montage dans la carrure. |
| **Colle UV Optique** | Adhésif acrylique polymérisable UV | ✅ **En Stock Atelier** | Prête pour le scellement étanche du verre minéral 12 mm. |
| **Joint Torique Fond-Carrure** | O-ring Nitrile/Viton étanchéité radiale | 🚚 **En transit** | Commande en cours d'acheminement. |
| **Mousses Poron & Adhésifs** | Poron microcellulaire (vibreur 4.5mm, calage batterie) | 🔍 **À vérifier** | À fouiller dans les placards à la rentrée (si manquant, commander Jeudi 29 Oct). |
| **Micro-taraud Poussoirs** | Taraud micro-mécanique pour vis de poussoirs | 🚚 **Commandé (Sparks, 09 Nov)** | Commandé avec les 2 Miyota de rechange. |
| **Composants Électroniques CMS** | `Carte_V2` & `Carte_Capteur_V1` (BOM JLCPCB assainie le 11-12 Nov) | 🟢 **Pré-commande JLCPCB effectuée (12 Nov 18h)** | **Pré-commande validée pour 2 prototypes** : `U2` (`C1850399` x2), `FL1` (`C5270481` x27), `ANT1` (`C17531353` x9), `Y1` (`C6455449` x2), `Q3` (`DMN2028UFU-13` `C17388236`), `M1` (`C20107647` x2) ; `U4` (`C5588142` stk 162). Délai appro : ~4-7 jours avant lancement commande PCBA. |
| **Carrures 316L & Bases Résine** | SLM 316L (`carrure`) + Résine SLA (`Base` + cale O4) | 🧪 **Proto PLA lancé (09 Nov)** | Finaliser `P0-MECA-6/7`, `P1-MECA-9/10` sur les `.STEP` ré-exportés avant envoi SLM/SLA. |

---

## 🩺 12. Audit & Contrôle de la Carte Capteurs (`Carte_Capteur_V1` / V2) — MàJ 11 Nov 2026

La carte fille capteurs (`Kicad/Carte_V2/Carte_Capteur_V2/Carte_Capteur_V1.kicad_pcb`) a été finalisée en version **V2.1** avec le boost `TPS61023` et validée sans erreur face aux datasheets et stocks JLCPCB le **11 Novembre 2026** (**ERC = 0 erreur / 11 warnings ; DRC + parité schématique = 0 violation, 0 non connecté**).

### 🎯 Architecture Validée
1. **Mesure optique PPG / SpO2** : `MAX30101EFD+T` (`IC5`, `C2859066`, stk `65`).
2. **Boost optique +5.06V** : **`TPS61023DRLR`** (`IC3`, `SOT-563`, `C919459`, stk `6 681`) avec inductance **`L1 = 1.0 µH` (`DFE18SBN1R0ME0L`, `0603`, `C882989`, stk `2 188`)**, pont **`R23 = 150 kΩ` / `R24 = 20 kΩ`** (`C25755` / `C25765`) et `C18/C19 = 10 µF` (`C96446`).
3. **Thermomètre cutané ±0.1 °C** : `TMP117AIDRVR` (`U2`, `C699536`, stk `42`) sur îlot thermique `B.Cu` isolé (`1.2×1.8 mm` ENIG, 2 vias + keepout).
4. **Translateur & Buffer I2C** : `TCA9517DR` (`U3`, `C182421`, **stk `5 738`**).
5. **Connecteur inter-PCB** : Samtec `TW-05-02-G-D-157-075` (`J1` — montage manuel hors PCBA).

### 📋 État de la Checklist Technique (11 Novembre 2026)
- [ ] **Homogénéisation des références non indexées (mineur, sans blocage DRC)** : Des symboles fantômes/non indexés (`C`, `IC`, `J`, `L`, `R`, `U`) subsistent dans `Carte_Capteur_V1.kicad_sch` (parité PCB néanmoins à `0` erreur). À nettoyer cosmétiquement avant archivage final.
- [x] **Valeurs des composants passifs & BOM JLCPCB (`11 Nov 2026`)** : Découplage `10 µF` (`C96446`) + `100 nF` (`C1525`) + `4.7 µF` (`C19666`), `R8 = 5 kΩ` (`C477765`), `R10/R21/R22 = 4.7 kΩ` (`C25900`), `R23 = 150 kΩ` (`C25755`), `R24 = 20 kΩ` (`C25765`), et **`L1 = 1.0 µH` `0603` (`C882989`)**.
- [x] **Intégrité I2C & Permutation `U3` (`TCA9517D`) (`11 Nov 2026`)** : **CORRIGÉ & REROUTÉ**. Side A sans offset (`VCCA = +1V8`, `SDAA/SCLA`) connecté au `MAX30101` ($V_{OLA} \approx 0.10\text{ V} < V_{IL(\max)} = 0.54\text{ V}$) ; Side B (`VCCB = +3V3`, `SDAB/SCLB`) connecté vers `J1` / `STM32WB55`. `#FLG03` en doublon supprimé. `C182421` confirmé en stock JLCPCB (`5 738`).
- [x] **Commande Boost `EN` & Migration `TPS61023` (`11 Nov 2026`)** : `/EN_5V_HR` et `/EN_TCA` câblés sur `J1` ; nouveau routage `TPS61023` + `L1 0603` validé à **0 erreur DRC** et packages `production/` générés (`Carte_Capteur_V1.zip`, `bom.csv`, `positions.csv`, `Carte_Capteur_V1.csv`).
- [x] **Thermique `U2` (`TMP117`) (`11 Nov 2026`)** : `Pad 7` flottant relié par **2 vias thermiques isolés** à une pastille ENIG dédiée (`1.2×1.8 mm` sur `B.Cu/B.Mask`) + zone keepout `U2_Thermal_Iso` (`In1.Cu/In2.Cu`) confirmée.
- [x] **ERC & DRC Carte Capteurs (`11 Nov 2026`)** : `ERC = 0 erreur` (`11` warnings), `DRC --schematic-parity --refill-zones = 0 violation, 0 non connecté, 0 parité`.
- [ ] **Dégagement radial `IC3/L1` (`P1-MECA-10`)** : Contrôler en 3D sur le nouveau `3D/Carte_Capteur_V2f.step` que `TPS61023` + `L1 0603` passent sans collision dans la jupe carrure.

---

## 🚨 13. Audit d'Ingénierie Global 360° (08 Nov) & Suivi Passe V2.1 (MàJ 11 Nov 2026 — Vérifs Élec/Datasheets & Restes Méca/Sourcing)

### 📋 Bilan au 11 Novembre 2026

| Bloc / ID | Statut au 11 Nov 2026 | Détail exact vérifié sur fichiers |
| :--- | :---: | :--- |
| **`[P0-ELEC-1]` Polarité `LIR3048`** | 🟡 **KiCad corrigé (vigilance `TP8`)** | `BT1` (ex-`C22`) latéral sur `+3V8`, `TP8` (`6×6 mm`) sur `GNDPWR`, `TP2` supprimé, `Batterie.SLDPRT` à `4.90 mm`. *Note : `TP8` est une pastille plate excentrée ($r=11.05\text{ mm}$) sans ressort central.* |
| **`[P0-ELEC-2]` Switch OLED `Q1/Q2` + `R13`** | 🟢 **RÉSOLU** | `Q2.2` à `GND`, `Q1.2` + `R15` sur `+3V8`, `Q1.3` sur `/Ecran/VBAT`, `R13 = 560 kΩ` (`C132339`). |
| **`[P0-ELEC-3]` `TCA9517D` (`U3`) inversé** | 🟢 **RÉSOLU** | Permutation Side A (`+1V8` MAX30101) / Side B (`+3V3` MCU) + re-routage `Carte_Capteur_V1` + DRC `0` ; `C182421` en stock (`5 738`). |
| **`[P0-RF-4]` Chaîne RF + Keepout + `Y1`** | 🟢 **CORRIGÉ (`⚠️ Y1.4 NC`)** | `R21/R24 = DNP` (`in_pos_files no`), Keepout `ANT1` sur 6 couches (`5.0×5.05 mm`), `Y1 = ABM13W 8 pF` (`C6455449`) + `C36/C50 = 12 pF C0G`. (`Y1.4` laissé `NC`). |
| **`[P0-MECA-5]` Hauteur aiguillage saphir** | 🟢 **RÉSOLU (09/11)** | Portée saphir relevée `+1.350 mm` (`Z=1003.313`, `+0.338 mm` minutes, `+0.138 mm` trotteuse), `0` contrainte en erreur, proto PLA lancé. |
| **`[P0-MECA-6]` Poche `J1` & lamage Qi (`Base`)** | 🟠 **À CONFIRMER EN 3D** | `Base.SLDPRT` édité le `09/11`, poche `12×4.5×0.95 mm` et lamage `0.55 mm` à re-contrôler sous `Montre.SLDASM` sur les `.STEP` ré-exportés. |
| **`[P0-MECA-7]` Composants `F.Cu` sous OLED** | 🔴 **OUVERT** | `Q1`, `Q2` (`TSOT-23`) et `C6/C7/C8/C20/C41/C42/C43` (`0603`) sont implantés sur `F.Cu` sous la zone dalle OLED (`dx=-4.75..+3.38, dy=+7.9..+15.7 mm`) ; contrôle/décision mécanique requis sur le nouveau `Carte_V2.3f.step`. |
| **`[P0-MECA-8]` Gorge O-ring & clipsage** | 🟠 **À MODIFIER (inverser clips)** | Gorge ISO 3601 (`0.75×1.30 mm`, 25 %) + `Batterie.SLDPRT` `4.90 mm` OK ; **clips `Base`/`carrure` à inverser** sous SolidWorks pour faciliter la fabrication. |
| **`[P1-MECA-9]` Poussoirs étanches** | 🟠 **OUVERT** | `Bouton.SLDASM` toujours daté du `24/09` ; poussoirs horlogers étanches (joint + circlip interne) à intégrer avant SLM 316L. |
| **`[P1-MECA-10]` `TPS61023/L1` & `Canal_lum`** | 🟠 **À CONFIRMER SUR NOUVEAU `.STEP`** | `Canal_lum.SLDPRT` mis à jour le `09/11 06:41` ; dégagement radial de `IC3 TPS61023` (`SOT-563`) + `L1 0603` à valider dans `Montre.SLDASM` avec le nouveau `Carte_Capteur_V2f.step`. |
| **`[P1-ELEC-11/12/13]` Alim, `VBAT`, Boost & BOM** | 🟡 **ROUTÉ (`⚠️ C21120`)** | Pont `2×1 MΩ + 100 nF` sur `PC4` (`U2.G4`), `IC4/IC6 = TPS7A0218P/33` (`tps7a02.pdf` EN `SBVS277C`), `R5 = 1.5 kΩ`, `R7 = DNP`, `C46 = 100 nF`, `R16 = 330 Ω`, `R17 = 2 kΩ`, `U4 = AP9101CAK-ATTRG1` (`C5588142`, *Auto-Wake-Up*, `stk=162`), `C0G2_50V1 = 220 nF` (`C21120` *25 V X7R*), `R27 = 10 kΩ`, `TH1 = 10k NTC` (`C91636`), Boost `TPS61023` + `L1 1.0 µH 0603` (`C882989`), `L1` Carte_V2 `SWPA252012S100MT` (`C37428`). |
| **`[P2-THERM-14]` Vias `TMP117`** | 🟢 **RÉSOLU** | `Pad 7` isolé + 2 vias + plage `B.Cu` `1.2×1.8 mm` ENIG + keepout `U2_Thermal_Iso`. |
| **`[P2-DRC-15]` DRC / ERC KiCad 10** | 🟢 **RÉSOLU** | `Carte_V2` : `ERC=0` (`62` w.), `DRC projet=0` / parité `0` *(20 `--severity-all` S1/S2 bord & IC2 connues)* ; `Carte_Capteur_V1` : `ERC=0` (`11` w.), `DRC=0` ; `Cadran` : `ERC=0`, `DRC=0` err (`55` w. bord). |
| **Stocks JLCPCB SMT (`12 Nov 2026`)** | 🟡 **PRÉ-COMMANDE `Carte_V2`** | **`Carte_Capteur_V1` : 100% JLC en stock** (`J1` main). **`Carte_V2` : 6 refs à `stk=0`** (`U2`, `FL1`, `ANT1`, `Y1`, `Q3` (`C17388236`), `M1`) + `IC1` (`2`), `IC8` (`4`), `TH1` (`68`) à pré-commander sur JLCPCB Parts (+ activer **VIPPO**). `U4` (`C5588142`) en stock (`162`). |
| **`[P2-PROC-17]` Livrables Usine** | 🟢 **GÉNÉRÉS (11-12 Nov)** | Packages `production/` (Gerbers `.zip`, Drill, IPC-356, `bom.csv`, `positions.csv`) des 3 cartes + `Carte_V2.csv`/`Carte_Capteur_V1.csv` + `3D/Carte_V2.3f.step` & `3D/Carte_Capteur_V2f.step` régénérés. *(Notice V3.9 à mettre à jour après validation méca finale).* |
| **`[P2-FW-16]` Firmware STM32** | ⏸️ **REPORTÉ** | Reporté après lancement fabrication matérielle (`SmileWatch1.ioc` à migrer vers `STM32WB55VGYx` ultérieurement). |

---

## 📅 Point d'Étape & Prochaines Actions — 12 Novembre 2026 (GO USINE & Optimisation Calendrier)

1. **Approvisionnement Électronique (En cours JLCPCB)** :
   - ✅ **Pré-commande Parts validée (12 Nov)** : `U2` (STM32WB55), `FL1` (Filtre RF), `ANT1` (Antenne Johanson), `Y1` (Quartz 32 MHz), `Q3` (`DMN2028UFU-13` `C17388236`), `M1` (Vibreur).
   - ⏳ **Attente transit / In Stock** : Délai fournisseur estimé ~4 à 7 jours ouvrés.
   - 🎯 **Action dès réception en stock** : Lancer la commande PCBA pour 2 cartes (`production/Carte_V2.zip` 6 couches 0.8mm avec **VIPPO** et `Carte_Capteur_V1.zip` 4 couches 0.8mm).
2. **Fabrication Mécanique en Temps Masqué (Pendant l'appro composants)** :
   - 🎯 **Impression 3D Résine SLA** : Lancer la fabrication de `Base.SLDPRT` (résine haute précision étanche) et de la cale de posage O4.
   - 🎯 **Usinage / SLM 316L** : Exporter les `.STEP` finaux de `carrure.SLDPRT` pour l'usineur.
   - 🔍 **Contrôles visuels en coupe** : Valider l'écrasement radial du joint torique et le dégagement des 10 broches Samtec.
3. **Firmware (Phase ultérieure)** : Créer le projet STM32CubeIDE sur **`STM32WB55VGYx` (WLCSP-100)** pendant la fabrication des cartes.
