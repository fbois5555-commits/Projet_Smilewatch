import sys
sys.stdout.reconfigure(encoding='utf-8')

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\suivi_projet.html", "r", encoding="utf-8") as f:
    html = f.read()

target = "<!-- BILAN SESSION SOLIDWORKS & PROTO PLA LANCÉ -->"
if target in html:
    checklist = '''<!-- CHECKLIST ACTIONS DU JOUR (09 NOV) -->
                <div style="background: rgba(59, 130, 246, 0.08); border: 1px solid rgba(59, 130, 246, 0.35); border-radius: 12px; padding: 1.1rem 1.25rem; margin-bottom: 1rem;">
                    <div style="display:flex; justify-content:space-between; align-items:center; margin-bottom:0.6rem;">
                        <span style="font-weight:700; color:#60a5fa; font-size:0.92rem;">🎯 Tâches du Jour — Figeage V2.1 &amp; Préparation Commandes (09 Nov 2026)</span>
                        <span style="font-size:0.75rem; background:rgba(59, 130, 246, 0.2); color:#93c5fd; padding:0.25rem 0.55rem; border-radius:6px; font-family:'JetBrains Mono',monospace;">Délai usines : 3 sem min</span>
                    </div>
                    <div style="display:grid; grid-template-columns: 1fr 1fr; gap:0.9rem; font-size:0.83rem;">
                        <div style="background:rgba(15,23,42,0.6); padding:0.75rem; border-radius:8px; border:1px solid rgba(255,255,255,0.06);">
                            <div style="font-weight:600; color:#cbd5e1; margin-bottom:0.35rem;">⚡ Électronique Carte_V2 (KiCad)</div>
                            <ul style="margin:0; padding-left:1.1rem; color:var(--text-secondary); line-height:1.5;">
                                <li>Inverser polarité LIR3048 (C22/BT1 au <code>+3V8</code>, plots centraux au <code>GNDPWR</code>).</li>
                                <li>Switch OLED : Source Q2 au <code>GND</code>, Source Q1 au <code>+3V8</code>, R13 = <code>560 kΩ</code>.</li>
                                <li>RF : Shunts R21/R24 = <code>DNP</code>, quartz Y1 = <code>ECS-320-08</code> (8 pF).</li>
                                <li>Monitoring : Pont ADC 2&times;1 MΩ + 100 nF sur <code>PA0</code> (Ball G7).</li>
                            </ul>
                        </div>
                        <div style="background:rgba(15,23,42,0.6); padding:0.75rem; border-radius:8px; border:1px solid rgba(255,255,255,0.06);">
                            <div style="font-weight:600; color:#cbd5e1; margin-bottom:0.35rem;">🛠️ Mécanique &amp; Fabrication Carrure</div>
                            <ul style="margin:0; padding-left:1.1rem; color:var(--text-secondary); line-height:1.5;">
                                <li>Carrure SLM 316L : gorge femelle intérieure (pas d'ergot fin saillant en métal).</li>
                                <li>Base SLA Résine : porte l'ergot mâle (0.25 mm) flexible pour clipsage.</li>
                                <li>Feuillure saphir : prévoir jeu +0.15 mm pour montage joint I ou colle UV.</li>
                                <li><strong style="color:#34d399;">Appros OK :</strong> Taraud &amp; 2&times; Miyota Sparks commandés le 09 Nov ✔</li>
                            </ul>
                        </div>
                    </div>
                </div>

                '''
    html = html.replace(target, checklist + target)
    print("Checklist inserted before banner.")
else:
    print("Target not found.")

with open(r"c:\Users\fbois\Documents\Perso\Projet_Smilewatch\suivi_projet.html", "w", encoding="utf-8") as f:
    f.write(html)
print("Saved suivi_projet.html.")
