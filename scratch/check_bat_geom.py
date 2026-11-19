import math

# Battery diameter = 30.0 mm. Negative circular cap = 17.0 mm diameter (radius 8.5 mm).
# C22 is at (99.057, 66.752)
# In CAD assembly, let's find the exact center of Batterie.SLDPRT
# Let's inspect the mate in Montre.SLDASM if possible or compute from C22
# S1921 touches the outer rim (R = 15.0 mm).
# Angle of S1921 is 22 deg.
# If S1921 touches at (99.057, 66.752) with rotation 22 deg:
# Normal vector at 22 deg or 112 deg:
# Let's check TP8 at (111.506, 89.535)
print("Pad TP8 center:", 111.506, 89.535)
print("Pad size: 6.0 mm diameter")
