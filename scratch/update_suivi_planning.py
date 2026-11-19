import sys
sys.stdout.reconfigure(encoding='utf-8')

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\suivi_projet.html", "r", encoding="utf-8") as f:
    html = f.read()

# 1. Update Inventory Table row for Taraud & Miyota Sparks
old_taraud = '''<tr>
                            <td><strong>Taraud & Mouvement spare</strong></td>
                            <td>Micro-taraud vis poussoirs + 1 Miyota 2035</td>
                            <td>Filetage boutons carrure + sécurisation montage</td>
                            <td><span class="status-badge status-todo">🛒 À commander Jeudi 29 Oct</span></td>
                        </tr>'''

new_taraud = '''<tr>
                            <td><strong>Taraud & 2 Mouvements Miyota</strong></td>
                            <td>Micro-tarauds + 2× Miyota 2035 (SparkFun / Sparks)</td>
                            <td>Filetage poussoirs carrure & motorisation analogique garantie</td>
                            <td><span class="status-badge status-ordered">🚚 Commandé (09 Nov) — Expédition Sparks en cours</span></td>
                        </tr>'''

if old_taraud in html:
    html = html.replace(old_taraud, new_taraud)
    print("Replaced Taraud row in inventory.")
else:
    print("Could not find exact old_taraud row, searching pattern...")
    import re
    html = re.sub(r'<tr>\s*<td><strong>Taraud & Mouvement spare</strong></td>[\s\S]*?</tr>', new_taraud, html)
    print("Replaced with regex.")

# Also update the Miyota 2035 row
old_miyota = '''<tr>
                            <td><strong>Mouvement Horloger Quartz</strong></td>
                            <td>Miyota 2035 (Modifié 2 fils)</td>
                            <td>Aiguilles analogiques (x2)</td>
                            <td><span class="status-badge status-stock">⚠️ 2 en stock opérationnels (1 raté, 1 spare à racheter)</span></td>
                        </tr>'''

new_miyota = '''<tr>
                            <td><strong>Mouvement Horloger Quartz</strong></td>
                            <td>Miyota 2035 (Modifié 2 fils)</td>
                            <td>Aiguilles analogiques (x2 montres)</td>
                            <td><span class="status-badge status-stock">✅ 2 en stock atelier + 2 neufs commandés chez Sparks (09 Nov)</span></td>
                        </tr>'''

if old_miyota in html:
    html = html.replace(old_miyota, new_miyota)
    print("Replaced Miyota row.")
else:
    import re
    html = re.sub(r'<tr>\s*<td><strong>Mouvement Horloger Quartz</strong></td>[\s\S]*?</tr>', new_miyota, html)
    print("Replaced Miyota with regex.")

# 2. Update Carrures row in inventory
old_carrure_inv = '''<tr>
                            <td><strong>Carrures Principales Inox 316L</strong></td>
                            <td>Impression 3D Métal SLM (JLCPCB 316L)</td>
                            <td>Structure mécanique, lunette et cornes (x2)</td>
                            <td><span class="status-badge status-todo">🚀 À commander Vendredi 30 Oct</span></td>
                        </tr>'''

new_carrure_inv = '''<tr>
                            <td><strong>Carrures Principales Inox 316L</strong></td>
                            <td>Impression 3D Métal SLM 316L (JLC3DP) + Finition alésage maison</td>
                            <td>Structure mécanique, lunette et cornes (x2)</td>
                            <td><span class="status-badge status-todo">🚀 Prêt à commander (Délai usine ~3 sem)</span></td>
                        </tr>'''

html = html.replace(old_carrure_inv, new_carrure_inv)

# 3. Update tab-roadmap with the realistic 3 weeks delivery
old_roadmap_pattern = r'<div id="tab-roadmap" class="tab-content">[\s\S]*?</div>\s*</div>\s*<!-- TAB 3: INVENTAIRE'

new_roadmap_content = '''<div id="tab-roadmap" class="tab-content">
            <div class="tasks-list">
                <div class="task-card">
                    <div class="task-info">
                        <div class="task-title">📅 S1 (26 Oct - 01 Nov) [Vacances Toussaint] : Figeage Initial Smilewatch V2</div>
                        <div class="task-desc">Corrections préliminaires KiCad V2, modélisation CAD Cadran & Carrure, identification des besoins appros.</div>
                        <div class="task-tags"><span class="tag-pill tag-kicad">Terminé</span></div>
                    </div>
                </div>
                <div class="task-card">
                    <div class="task-info">
                        <div class="task-title">🤖 S2 (02 - 08 Nov) : CdF Gros Robot &amp; Audit d'Ingénierie Global 360° (08 Nov)</div>
                        <div class="task-desc"><strong>Mardi 13h30 (FabLab CdF) :</strong> Coordination mécanique avec Alexis, lancement système de tir et rails.<br><strong>Week-end (06-08 Nov) :</strong> Détection des 8 bloquants P0 KiCad + SolidWorks avant commande.</div>
                        <div class="task-tags"><span class="tag-pill tag-kicad">Audit 360° Soldé</span></div>
                    </div>
                </div>
                <div class="task-card" style="border-color: rgba(0, 229, 255, 0.45); background: rgba(0, 229, 255, 0.06);">
                    <div class="task-info">
                        <div class="task-title">🚨 S3 (09 - 15 Nov) : Validation Méca OK, Passes Élec &amp; Lancement Commandes Usines (Délai 3 Semaines)</div>
                        <div class="task-desc"><strong>Lundi 09 Nov (Matin) :</strong> Chaîne de cotes SolidWorks validée (0 erreur, +1.35mm saphir), impression PLA test lancée.<br><strong>Lundi 09 Nov (Appros) :</strong> <strong>Commande passée chez Sparks : Taraud + 2 moteurs Miyota 2035</strong>.<br><strong>Journée / Soirée (Smilewatch) :</strong> Corrections schémas KiCad V2 (polarité batterie, switch OLED +3V8, RF DNP, diviseur ADC PA0). Validation Carrure SLM 316L (gorge femelle) &amp; Base SLA (ergot mâle).<br><strong>Jalon Critique (10-12 Nov) :</strong> <strong>Validation et envoi commandes JLCPCB PCBA + JLC3DP SLM 316L / SLA</strong> (Délai de fabrication + transit international : <strong>minimum 3 semaines</strong>).</div>
                        <div class="task-tags"><span class="tag-pill tag-urgent">Semaine En Cours — Figeage & Commandes</span></div>
                    </div>
                </div>
                <div class="task-card">
                    <div class="task-info">
                        <div class="task-title">🏭 S4 (16 - 22 Nov) : Production Industrielle Usines (Semaine 1/3) &amp; Firmware STM32WB sur Banc</div>
                        <div class="task-desc"><strong>JLCPCB / JLC3DP :</strong> Fabrication des PCB multicouches, approvisionnement CMS, assemblage SMT, impression laser SLM 316L et photopolymérisation résine SLA.<br><strong>CdF (Mardi 13h30) :</strong> Assemblage préhenseur ventouse et tests cinématique bras 4 axes.<br><strong>Smilewatch Firmware :</strong> Développement CubeIDE (drivers I2C SSD1315/SH1107, accéléromètre KX023, BLE stack, timer PWM moteur).</div>
                        <div class="task-tags"><span class="tag-pill tag-firmware">Production Usine 1/3 & Banc FW</span></div>
                    </div>
                </div>
                <div class="task-card">
                    <div class="task-info">
                        <div class="task-title">✈️ S5 (23 - 29 Nov) : Expédition Internationale (Semaine 2/3) &amp; Révisions Intensives Signal</div>
                        <div class="task-desc"><strong>Livraisons :</strong> Contrôle qualité en sortie d'usine, emballage et expédition aérienne DHL/FedEx des colis JLCPCB.<br><strong>Appros Sparks :</strong> Réception attendue du colis Sparks (taraud + 2 Miyota de rechange).<br><strong>Révisions Académiques :</strong> 10h de travail bloquées pour préparation intensive du Partiel Signal (examen le lundi 30 nov). <em>(WE Rennes 27-29 nov)</em>.</div>
                        <div class="task-tags"><span class="tag-pill tag-firmware">Transit Usine 2/3 & Révisions</span></div>
                    </div>
                </div>
                <div class="task-card" style="border-left: 4px solid #10b981;">
                    <div class="task-info">
                        <div class="task-title">📦 S6 (30 Nov - 06 Déc) : RÉCEPTION DES COLIS (Jalon 3 Semaines Atteint), Finition SLM &amp; 1er Flash</div>
                        <div class="task-desc"><strong>Lundi 30 Nov 13h30 :</strong> Examen Signal.<br><strong>Réception Colis (~01 - 04 Déc) :</strong> Réception des cartes PCBA Carte_V2 + Carrures inox SLM 316L + Bases résine SLA.<br><strong>Atelier :</strong> Reprise et toilage manuel maison de l'alésage de carrure SLM (papier 400 puis 800 à l'eau) pour portée de joint torique miroir.<br><strong>Électronique :</strong> 1er test sous tension, mesure consommations, flash initial Tag-Connect STM32WB55 (M4 + pile M0+ RF BLE).</div>
                        <div class="task-tags"><span class="tag-pill tag-urgent">🎉 Réception Colis & Finition SLM</span></div>
                    </div>
                </div>
                <div class="task-card">
                    <div class="task-info">
                        <div class="task-title">⏱️ S7 (07 - 13 Déc) : Intégration Complète Montre V2 &amp; Validation Autonomie</div>
                        <div class="task-desc"><strong>Lundi 07 Déc 13h30 :</strong> Examen Automatique.<br><strong>Montage Horloger :</strong> Emboîtement complet (Base SLA clipsée dans Carrure 316L avec joint torique lubrifié silicone), chassage des aiguilles Miyota, pose de la glace saphir.<br><strong>Essais globaux :</strong> Test d'étanchéité à l'air/poussière, synchronisation BLE avec smartphone, affichage micro-OLED, recharge sans fil Qi.</div>
                        <div class="task-tags"><span class="tag-pill tag-meca">Intégration Complète Montre</span></div>
                    </div>
                </div>
                <div class="task-card" style="border: 2px solid #3b82f6;">
                    <div class="task-info">
                        <div class="task-title">🎓 S8 (14 - 15 Déc) : SOUTENANCE PROJET S7 GROS ROBOT CDF &amp; MONTRE V2 AU POIGNET</div>
                        <div class="task-desc"><strong>Mardi 15 Décembre 13h30 :</strong> Soutenance finale devant le jury académique. Démonstration du gros robot autonome et présentation de la Smilewatch V2 finalisée et portée au poignet !</div>
                        <div class="task-tags"><span class="tag-pill tag-urgent">Jalon Final Soutenance</span></div>
                    </div>
                </div>
                <div class="task-card">
                    <div class="task-info">
                        <div class="task-title">🛡️ Semaine de Secours Optionnelle (04 - 09 Janvier 2027)</div>
                        <div class="task-desc">Semaine de réserve post-Noël si des ajustements firmware ou fiabilisation batterie sont nécessaires.</div>
                        <div class="task-tags"><span class="tag-pill tag-firmware">Réserve R&D</span></div>
                    </div>
                </div>
            </div>
        </div>
        <!-- TAB 3: INVENTAIRE'''

import re
html = re.sub(old_roadmap_pattern, new_roadmap_content, html)
print("Updated tab-roadmap successfully.")

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\suivi_projet.html", "w", encoding="utf-8") as f:
    f.write(html)
print("Saved suivi_projet.html.")
