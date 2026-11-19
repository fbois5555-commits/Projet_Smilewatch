import sys
sys.stdout.reconfigure(encoding='utf-8')

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\REFLEXIONS.md", "r", encoding="utf-8") as f:
    text = f.read()

appendix = '''
---

## 📅 Point d'Étape & Réalignement Planning — 09 Novembre 2026

### 1. Appros & Commandes Recommandées (09 Nov)
- **Moteurs Miyota 2035 & Micro-taraud** : Recommandés chez SparkFun / Sparks ce jour. Sécurise 2 moteurs analogiques neufs en stock + l'outillage de taraudage pour les poussoirs et finitions d'atelier.

### 2. Choix de Fabrication : Carrure SLM 316L + Base SLA
- **Rejet du Binder Jetting (BJ-316L)** : Tolérance officielle JLCPCB de $\pm 0.3\\text{ mm}$, détails mini $1.0\\text{ mm}$, retrait au four de 15 à 20 %, rugosité $Ra > 4\\ \\mu\\text{m}$ impropre à l'étanchéité d'un joint torique.
- **Sélection du SLM 316L (Fusion Laser Directe)** : Tolérance $\pm 0.1\\text{ mm}$, densité $> 99.5\\%$, pas de retrait au four. Solution économique (~20-25 €) vs CNC unitaire (70-120 €).
- **Architecture de clipsage & reprise maison** :
  - **Ergot mâle (0.25 mm)** porté par la **Base en résine SLA** (flexibilité élastique, précision $\pm 0.05\\text{ mm}$).
  - **Gorge femelle / cran intérieur** sur la **Carrure SLM 316L** (facile à ébavurer, aucun ergot métallique fin risquant de casser).
  - **Étanchéité O-ring** : Reprise et toilage manuel maison de l'alésage intérieur de la carrure (papier carrosserie à l'eau 400 puis 800) pour portée miroir.
  - **Glace saphir** : Jeu de $+0.15\\text{ mm}$ sur le diamètre de feuillure pour pose facile joint I / colle UV.

### 3. Feuille de Route Actualisée (Délai Usines : 3 Semaines Minimum)
* **S3 (09 - 15 Nov) [En cours]** : Finalisation correctifs KiCad Carte_V2 (polarité LIR3048, switch OLED +3V8, RF shunts DNP, ADC PA0). Validation CAO carrure SLM / base SLA. **Déclenchement des commandes PCBA & 3D JLC d'ici le 11-12 Nov**.
* **S4 (16 - 22 Nov)** : Fabrication industrielle usines (Semaine 1/3). Développement firmware STM32WB55 sur banc. CdF : bras 4 axes et ventouse.
* **S5 (23 - 29 Nov)** : Expédition internationale et transit aérien (Semaine 2/3). Réception colis Sparks (taraud + 2 Miyota). Révisions intensives Partiel Signal (examen lun 30 nov).
* **S6 (30 Nov - 06 Déc)** : **RÉCEPTION DES COLIS JLCPCB (PCBA V2 + SLM 316L + SLA)** (Jalon 3 semaines atteint ~ 01-04 Déc). Toilage alésage carrure 316L, 1er flash Tag-Connect STM32WB55. Examen Signal (30 nov).
* **S7 (07 - 13 Déc)** : Assemblage complet Smilewatch V2 (clipsage Base SLA dans Carrure 316L, aiguillage Miyota, pose saphir) + banc test autonomie & BLE. Examen Automatique (07 déc).
* **S8 (14 - 15 Déc)** : **Soutenance finale R&D Smilewatch & Projet Gros Robot CdF (Mardi 15 Décembre)** avec montre fonctionnelle portée au poignet.
'''

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\REFLEXIONS.md", "a", encoding="utf-8") as f:
    f.write(appendix)

print("Updated REFLEXIONS.md successfully.")
