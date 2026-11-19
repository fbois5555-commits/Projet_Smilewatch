import re

with open('Kicad/Carte_V2/Ecran.kicad_sch', 'r', encoding='utf-8') as f:
    c = f.read()

# Let's verify all connections for Q1, Q2, R15, R14
# We will do a full graph connectivity of wires and pins in Ecran.kicad_sch
wires = re.findall(r'\(wire\s+\(pts\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s+\(xy\s+([\d\.-]+)\s+([\d\.-]+)\)\s*\)', c)

from collections import defaultdict
adj = defaultdict(set)
for w in wires:
    p1 = (round(float(w[0]), 2), round(float(w[1]), 2))
    p2 = (round(float(w[2]), 2), round(float(w[3]), 2))
    adj[p1].add(p2)
    adj[p2].add(p1)

def get_net(pt):
    pt = (round(pt[0], 2), round(pt[1], 2))
    visited = set()
    queue = [pt]
    while queue:
        curr = queue.pop(0)
        if curr in visited:
            continue
        visited.add(curr)
        for nxt in adj[curr]:
            if nxt not in visited:
                queue.append(nxt)
    return visited

# Labels and power symbols
labels = {}
for m in re.finditer(r'\((?:label|hierarchical_label|global_label)\s+\"([^\"]+)\"[\s\S]*?\(at\s+([\d\.-]+)\s+([\d\.-]+)', c):
    name, x, y = m.group(1), round(float(m.group(2)), 2), round(float(m.group(3)), 2)
    labels[(x, y)] = f"Label:{name}"

# Power symbols
for m in re.finditer(r'\(symbol\s+\(lib_id\s+\"power:([^\"]+)\"\)\s+\(at\s+([\d\.-]+)\s+([\d\.-]+)', c):
    name, x, y = m.group(1), round(float(m.group(2)), 2), round(float(m.group(3)), 2)
    labels[(x, y)] = f"Power:{name}"

def identify_net(points):
    found = set()
    for p in points:
        for lp, lval in labels.items():
            if abs(p[0] - lp[0]) < 0.1 and abs(p[1] - lp[1]) < 0.1:
                found.add(lval)
    return found

# Test points:
# Q1: FDN338P at (208.28, 55.88)
# Pin 1 (G): (208.28, 55.88)
# Pin 2 (S): (215.90, 50.80)
# Pin 3 (D): (215.90, 66.04)
print("=== Q1 (FDN338P) ===")
net_q1_g = get_net((208.28, 55.88))
print("Pin 1 (G):", identify_net(net_q1_g), "at (208.28, 55.88)")

net_q1_s = get_net((215.90, 50.80))
print("Pin 2 (S):", identify_net(net_q1_s), "at (215.90, 50.80)")

net_q1_d = get_net((215.90, 66.04))
print("Pin 3 (D):", identify_net(net_q1_d), "at (215.90, 66.04)")

# Q2: FDN335N at (173.99, 67.31)
# Pin 1 (G): (173.99, 67.31)
# Pin 2 (S): (181.61, 72.39)
# Pin 3 (D): (181.61, 57.15)
print("\n=== Q2 (FDN335N) ===")
net_q2_g = get_net((173.99, 67.31))
print("Pin 1 (G):", identify_net(net_q2_g), "at (173.99, 67.31)")

net_q2_s = get_net((181.61, 72.39))
print("Pin 2 (S):", identify_net(net_q2_s), "at (181.61, 72.39)")

net_q2_d = get_net((181.61, 57.15))
print("Pin 3 (D):", identify_net(net_q2_d), "at (181.61, 57.15)")

# R15: at (171.45, 52.07) rot 90
# Pin 1: (168.91, 52.07), Pin 2: (173.99, 52.07)
print("\n=== R15 (47k) ===")
net_r15_1 = get_net((168.91, 52.07))
print("Pin 1:", identify_net(net_r15_1), "at (168.91, 52.07)")
net_r15_2 = get_net((173.99, 52.07))
print("Pin 2:", identify_net(net_r15_2), "at (173.99, 52.07)")

# R14: at (165.10, 80.01) rot 0
# Pin 1: (165.10, 77.47), Pin 2: (165.10, 82.55)
print("\n=== R14 (47k) ===")
net_r14_1 = get_net((165.10, 77.47))
print("Pin 1:", identify_net(net_r14_1), "at (165.10, 77.47)")
net_r14_2 = get_net((165.10, 82.55))
print("Pin 2:", identify_net(net_r14_2), "at (165.10, 82.55)")
