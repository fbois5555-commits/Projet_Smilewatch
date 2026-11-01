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
- [ ] **Bruit d'alimentation sur le MAX30101** : Le capteur optique PPG utilise le rail +5V généré par le boost nanoPower MAX17222. Vérifier que l'ondulation résiduelle (ripple) du boost ne vient pas bruiter les mesures PPG (envisager un filtrage LC ou RC supplémentaire si nécessaire).
- [ ] **Consommation en veille (Quiescent Current)** : Vérifier que le convertisseur boost et les LDOs AP2127 peuvent être éteints via une broche `EN` (Enable) contrôlée par le STM32WB lorsque les capteurs et l'écran ne sont pas sollicités.
- [ ] **Mesure de tension batterie LIR3048** : Vérifier la présence d'un pont diviseur commutable (via un MOSFET type FDN338P) pour mesurer la tension de batterie sur un ADC du STM32WB sans fuite de courant permanente.
- [ ] **Sécurité & Isolation Batterie LIR3048** :
  - Sur une pile bouton, le corps extérieur (tranche et fond) est généralement au pôle **POSITIF (+)**.
  - S'assurer d'une zone d'isolation stricte (masque de soudure + disque Kapton) sous la batterie sur le PCB pour éviter tout court-circuit franc entre le boîtier de la batterie et des vias/pistes sous-jacentes.
  - Valider la compression axiale via mousse Poron pour éviter tout décollement lors des chocs.

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

### 🏁 Statut Carte Mère Carte_V2 : Figeage & Prête pour Revue par les Pairs (Peer Review)
- **Routage 6 couches complet** : Stackup JLCPCB JLC06161H-3313 (Top, In1.Cu, In2.Cu, In3.Cu, In4.Cu, Bottom).
- **Règles d'isolation DRC** : Isolation de contour fixée et respectée à **$\ge 0{,}20	ext{ mm}$** (règle standard d'usinage mécanique JLCPCB).
- **Intégrité RF** : Ligne microruban 50 Ω calculée et validée entre le filtre MLPF-WB-02D3 et l'antenne céramique 2450AT.
- **Dossier figé** : Aucune modification structurelle supplémentaire sur `Carte_V2` avant la relecture croisée par les pairs.
- [ ] **Résistances de tirage I2C (Pull-ups)** : Vérifier la valeur résultante des pull-ups sur le bus partagé (OLED, TMP117, MAX30101, LSM6DSOX, VEML6030, DRV2605L).

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

## 🔍 10. Audit Détaillé des Pinouts KiCad vs Datasheets (Octobre 2026)

### 🚨 ANOMALIE CRITIQUE DÉTECTÉE & À CORRIGER :
* **`IC8` (Driver Haptique TI DRV2605L - Fichier `Moteur.kicad_sch`)** :
  * **Problème constaté** : La broche **A2 (`REG`)** est connectée au rail d'alimentation global **`+1V8`**.
  * **Ce que dit la datasheet officielle Texas Instruments (SLOS854D)** :  
    > *"REG: 1.8-V regulator output. Connect a 1-µF capacitor between REG and GND. This pin is not meant to supply power to external circuits, **nor should an external power supply be connected to it**."*
  * **Risque matériel** : Court-circuiter le régulateur interne du DRV2605L avec le LDO externe AP2127 crée un conflit entre deux sources d'alimentation, générant un courant de fuite continu, un échauffement et un dysfonctionnement de la logique haptique interne.
  * **Action corrective immédiate sous KiCad** : Supprimer le symbole `+1V8` sur la broche A2. Laisser la broche A2 reliée **uniquement à un condensateur de 1 µF vers GND**.

---

### ✅ VÉRIFICATIONS COMPLÈTES CONFORMES (100% VALIDÉES) :

1. **Microcontrôleur STM32WB55VGYx (Boîtier WLCSP-100)** :
   * **Ligne RF** : Broche K4 (`RF1`) connectée à la broche A3 (`IN`) du filtre ST `MLPF-WB-02D3`. Broche A1 (`OUT`) reliée à la broche 1 de l'antenne `2450AT07A0100`. Bumps A2, B1, B2, B3 à GND. **Parfait**.
   * **Connecteur SWD Tag-Connect TC2030 (J1)** : 1=VCC (+3V3), 2=SWDIO (PA13), 3=NRST, 4=SWCLK (PA14), 5=GND, 6=SWO (PB3). **Brochage rigoureusement identique au standard Tag-Connect**.
   * **Oscillateurs** : Quartz 32 MHz `ECS-320` sur OSC_IN (J1) / OSC_OUT (J2). Quartz 32.768 kHz `ECS-.327` sur PC14 (D10) / PC15 (D9). **Conforme**.
   * **Bus I2C1** : PB8 (`SCL`) et PB9 (`SDA`). **Conforme aux fonctions alternatives STM32WB**.
   * **VDDUSB à GND** : Conforme aux recommandations ST pour désactiver le contrôleur USB physique et économiser le courant.

2. **Capteurs & Module Déporté (`Carte_Capteur_V2`)** :
   * **LSM6DSOX (IMU 6 axes)** : CS à +3V3 (active le mode I2C). SDO/SA0 à GND (adresse I2C = 0x6A). INT1 et INT2 reliés au MCU. **Parfait**.
   * **VEML6030 (Luminosité)** : ADD à GND (adresse I2C = 0x10). INT vers MCU (PA12). **Parfait**.
   * **TMP117 (Température clinique)** : ADD0 à GND (adresse I2C = 0x48). ALERT vers MCU (PA9). **Parfait**.
   * **MAX30101 (Cardio/SpO2)** : VLED+ sur rail +5V dédié, VDD sur +1V8, bus I2C isolé via le TCA9517. **Parfait**.
   * **MAX17222 (Boost nanoPower 5V)** : Broche RSEL/SEL reliée directement à GND. Selon la datasheet Maxim/ADI, RSEL = 0 $\Omega$ sélectionne **exactement 5.0V** en sortie sans aucune résistance externe. **Excellent choix de compacité**.
   * **TCA9517 (Répéteur I2C)** : VCCA = 3.3V, VCCB = 3.3V, EN contrôlé par MCU (PA4). **Parfait**.

3. **Alimentation & Charge Qi (`Alimentation_sch.kicad_sch`)** :
   * **Récepteur Qi BQ51003** : Réseau de condensateurs résonants complet (470nF série, 2.2nF parallèle, 22nF clamp, 10nF comm). EN1 et EN2 à GND (activation sortie). AD à GND (mode sans fil prioritaire). Sortie OUT 5V vers le chargeur. **Parfait**.
   * **Chargeur BQ24040** :
     * $R_{ISET} = 4.53\text{ k}\Omega \implies I_{CHG} = 540 / 4530 \approx 119\text{ mA}$ (~0.43C pour la batterie 280 mAh, idéal pour la longévité de la cellule).
     * $R_{PRETERM} = 2.0\text{ k}\Omega \implies I_{TERM} \approx 12\text{ mA}$.
     * Broche `~CHG` connectée à PB2 du MCU pour surveiller l'état de charge. **Parfait**.
   * **Protection AP9101CK & Double MOSFET DMN2028UFU** : VDD au pôle positif, VSS au pôle négatif (GNDPWR), grilles DO et CO pilotant les deux canaux N. **Parfait**.

## 📦 11. Bilan d'Approvisionnement & Inventaire Réel (Objectif : 2 Montres SmileWatch)
*Mis à jour le 26 Octobre 2026 suite au retour d'inventaire.*

### Synthèse de Disponibilité pour 2 Exemplaires

| Désignation | Référence Technique / Détails | Quantité / Statut | Action Requise |
| :--- | :--- | :--- | :--- |
| **Écran micro-OLED 0.5"** | Topwin `QG-8848TSWYG02` (Contrôleur CH1115, 88×48) | ✅ **Reçu (x2)** | Datasheet V2 analysée : 14 broches au pas de 0.62 mm. Soudure directe confirmée. |
| **Batterie Rechargeable** | LIR3048 Li-ion (3.7V - 280 mAh) | ✅ **Reçu (x2)** | Stock OK. Prête pour l'intégration mécanique. |
| **Mouvement Quartz** | Miyota 2035 (2 fils de bobine à souder) | ⚠️ **2 OK en stock** (3 achetés au départ, 1 raté lors des tests) | **Recommander 1 Miyota 2035** en spare de sécurité en même temps que le taraud. |
| **Aiguilles Horlogères** | Aiguilles Heures & Minutes pour Miyota 2035 | ✅ **Reçu (x2 sets)** | Stock OK. Pose au Bergeon 6767 lors de la phase 1. |
| **Verre Principal Supérieur** | Glace Saphir plat haute résistance | ✅ **Reçu** | Stock OK pour le chassage avec posage O4. |
| **Joint en I supérieur** | Joint Téflon / Hytrel profil en I | ✅ **Reçu** | Stock OK pour l'étanchéité du verre supérieur. |
| **Verre Fond (Bottom) 12 mm** | Glace minérale trempée Ø12 mm | ✅ **Reçu** | Fenêtre optique capteur cardio MAX30101 & TMP117. |
| **Boutons Poussoirs** | 4 boutons poussoirs physiques latéraux | ✅ **Reçu (4 pièces)** | 2 boutons par montre. Prêts pour montage dans la carrure. |
| **Colle UV Optique** | Adhésif acrylique polymérisable UV | ✅ **En Stock Atelier** | Prête pour le scellement étanche du verre minéral 12 mm. |
| **Joint Torique Fond-Carrure** | O-ring Nitrile/Viton étanchéité radiale | 🚚 **En transit** | Commande en cours d'acheminement. |
| **Mousses Poron & Adhésifs** | Poron microcellulaire (vibreur 4.5mm, calage batterie) | 🔍 **À vérifier** | À fouiller dans les placards à la rentrée (si manquant, commander Jeudi 29 Oct). |
| **Micro-taraud Poussoirs** | Taraud micro-mécanique pour vis de poussoirs | 🛒 **À commander** | À commander Jeudi 29 Oct avec le mouvement Miyota spare. |
| **Composants Électroniques CMS** | STM32WB55, BQ24040, TMP117, Bobine Qi WE 760308101309 | 📋 **Panier Mouser / SMT** | Vérifier la commande groupée / intégration SMT JLCPCB. |
| **Carrures 316L & Bases Résine** | Impression 3D métal SLM (carrure) + Résine (base + cale O4) | 🚀 **À lancer** | Figeage CAO / Envoi en fabrication le 30 Octobre. |

---


---

## 🩺 12. Audit & Contrôle de la Carte Capteurs (Carte_Capteur_V2)

La carte mère `Carte_V2` étant figée pour revue par les pairs, la priorité passe au **contrôle rigoureux et à la mise à niveau de la carte fille capteurs** (`Carte_Capteur_V2`), située dans `Kicad/Carte_V2/Carte_Capteur_V2/`.

### 🎯 Objectifs de la Carte Capteurs
Logée dans la base inférieure de la montre, au contact direct du poignet à travers le fond de boîte, cette carte regroupe :
1. **Mesure optique PPG / SpO2** : Capteur Maxim Integrated `MAX30101EFD+T` (LEDs Rouge, IR, Verte + photodétecteur).
2. **Alimentation optique nanoPower Boost +5V** : Convertisseur élévateur `MAX17222ELT+T` (inductance 2.2 µH) alimentant la chaîne LED du MAX30101.
3. **Thermomètre médical haute précision** : Capteur Texas Instruments `TMP117xxDRV` (résolution 0.0078°C, précision ±0.1°C pour mesure cutanée).
4. **Interface & Buffer I2C** : Translateur de niveau et buffer de bus bidirectionnel `TCA9517D`.
5. **Connecteur d'interconnexion** : Connecteur mezzanine Samtec `TW-05-02-G-D-157-075` (2×5 broches au pas de 2.0 mm) assurant l'alimentation et la liaison numérique avec la carte mère.

---

### 📋 Checklist d'Audit Technique & Points d'Attention

#### 1. Schéma Électrique & Nettoyage des Annotations
- [ ] **Homogénéisation des références (Nettoyage critique)** : 
  - Présence dans `Carte_Capteur_V1.kicad_sch` de plusieurs symboles génériques non indexés (`C`, `IC`, `J`, `L`, `R`, `U`) en doublon avec des symboles annotés (`C1`, `IC3`, `J1`, `L1`, `R10`, `U3`).
  - **Action** : Ré-annoter proprement le schéma et synchroniser avec le PCB sans écraser les empreintes déjà routées.
- [ ] **Valeurs des composants passifs** :
  - Découplage MAX30101 : 10 µF tantale/céramique X5R faible ESR + 100 nF au plus près de `VDD` (+1.8V) et `VLED` (+5V).
  - Découplage Boost MAX17222 : 10 µF en entrée (`IN`) et 10 µF en sortie (`OUT`), inductance blindée 2.2 µH à faible résistance série ($R_{DC}$).

#### 2. Intégrité du Bus I2C & Translateur TCA9517D
- [ ] **Conflit d'offset statique bas niveau ($V_{OL}$)** :
  - Le `TCA9517D` intègre un buffer avec un décalage de niveau sur sa face B : la tension de sortie basse y est régulée à $pprox 0{,}5	ext{V}$ (pour éviter le verrouillage mutuel en boucle).
  - **Règle absolue** : La face B du TCA9517 **ne doit jamais être connectée à un autre buffer à offset**. Vérifier que le STM32WB ou les capteurs acceptent bien ce seuil $V_{IL} > 0{,}5	ext{V}$.
  - Vérifier les résistances de tirage (*pull-ups*) sur `SDA_A`/`SCL_A` (côté 1.8V/3.3V) et `SDA_B`/`SCL_B` (côté 3.3V/5V) : 4.7 kΩ recommandés.

#### 3. Chaîne d'Alimentation & Commande du Boost MAX17222
- [ ] **Broche `EN` (Enable)** : Vérifier que le convertisseur boost +5V peut être coupé à 100% via une broche de commande `EN` (pilotée par le STM32WB) pour garantir la consommation nanoampère en veille lorsque le cardio n'est pas actif.
- [ ] **Filtrage de l'ondulation (Ripple)** : L'ondulation du boost ne doit pas parasiter les mesures optiques du MAX30101 (vérifier la présence d'un filtre RC ou ferrite sur le rail `+5V`).

#### 4. Thermique & Contact Cutané (TMP117)
- [ ] **Dissipation & Précision** :
  - Le pad thermique central du TMP117 doit être relié par des vias thermiques à une pastille de cuivre exposée (dorée ENIG) face inférieure pour capter la température du poignet.
  - S'assurer que le TMP117 est physiquement éloigné du convertisseur boost MAX17222 et des pistes d'alimentation de puissance pour ne pas fausser la mesure par auto-échauffement.

#### 5. Connecteur Inter-PCB `J1` & Intégration Mécanique
- [x] **Brochage croisé (Pinout Mapping) — 100% VALIDÉ** : Vérification broche par broche entre `Carte_V2` (J2) et `Carte_Capteur_V1` (J1) (Samtec `TW-05-02-G-D-157-075` à -125° en face arrière B.Cu) :
  - Broche 1 : `I2C1_SDA`
  - Broche 2 : `I2C1_SCL`
  - Broche 3 : `/EN_TCA`
  - Broche 4 : `NC`
  - Broche 5 : `/HR_INT`
  - Broche 6 : `/EN_5V_HR`
  - Broche 7 : `+1V8` (sécurisé avec pastille Ø1.15 mm pour dégagement $\ge 0.20	ext{ mm}$ du bord)
  - Broche 8 : `/TEMP_ALERT`
  - Broche 9 : `GND`
  - Broche 10 : `+3V3`
- [x] **Encombrement 3D & Berceau Batterie LIR3048** :
  - Rayon intérieur de découpe du PCB : $R = 15.30	ext{ mm}$ (dégagement de 0.30 mm autour du corps métallique Ø 30 mm de la cellule LIR3048).
  - Sécurisation absolue contre le court-circuit `VBAT` (4.2V corps de pile) $\leftrightarrow$ `+1V8` (broche 7).

#### 6. Règles PCB & DRC Carte Capteurs — 100% SUCCÈS (27 Octobre 2026)
- [x] **Contraintes de fabrication JLCPCB** :
  - Vias : Ø 0.35 mm / perçage 0.20 mm (annulaire 0.075 mm $\ge 0.05	ext{ mm}$ JLCPCB standard).
  - Isolement cuivre-contour : $\ge 0.20	ext{ mm}$ CNC milling.
  - Finition : **ENIG (Electroless Nickel Immersion Gold)** indispensable pour contact cutané et dorure des pastilles.
- [x] **Résolution intégrale du DRC (`kicad-cli pcb drc`)** :
  - **Erreurs : 0** (109 initialement).
  - **Éléments non connectés : 0** (3 initialement).
  - Câblage d'alimentation et découplage C23 / TMP117 corrigé en parallèle (+3V3 et GND).
  - Alimentation V_LED+ Pad 10 du MAX30101 reliée au Pad 9 (+5V).
  - Isolement piste /LX sur le boost MAX17222 et piste +3V3 sur In2.Cu validés.
  - Carte fille capteurs validée et prête pour le dossier de fabrication !
