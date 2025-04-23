# author    Dubsky Tomas

import itertools

def print_macro(l:list, x, y, z, w):
    str = ""
    for dim in l:
        if dim == 0:
            str += x
        elif dim == 1:
            str += y
        elif dim == 2:
            str += z
        elif dim == 3:
            str += w
    print(f"#define {str} {str}()")

macros = {}

for w in range(0, 5):
    for z in range(0, 5 - w):
        for y in range(0, 5 - w - z):
            for x in range(0, 5 - w - z - y):
                l = list()
                l.extend([3] * w) 
                l.extend([2] * z) 
                l.extend([1] * y) 
                l.extend([0] * x) 
                if len(l) >= 2:
                    perms = list(itertools.permutations(l))
                    for perm in perms:
                        macros[perm] = None

print("// xyzw swizzles")
for macro in macros.keys():
    print_macro(macro, 'x', 'y', 'z', 'w')
print()
print("// rgba swizzles")
for macro in macros.keys():
    print_macro(macro, 'r', 'g', 'b', 'a')
print()
print("// stpq swizzles")
for macro in macros.keys():
    print_macro(macro, 's', 't', 'p', 'q')