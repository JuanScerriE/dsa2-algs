#     |
#     |
# X = aaaaaaabbbbbbbbb
# Y = dsfbbbbbbbbdsfaa
#     |
#     |
   
def lcs(x, y):
    lcs = ""
    cs = ""
    i = 0
    j = 0
    k = 0
    n = len(x)
    m = len(y)

    while i < n:
        cs = ""
        j = 0
        k = 0
        print("cs = ", cs)
        print("i = ", i)
        print("j = ", j)
        print("k = ", k)

        while k < m:
            if x[j] == y[k]:
                cs = f"{cs}{y[k]}"

                j += 1

            k += 1
            print("cs = ", cs)
            print("i = ", i)
            print("j = ", j)
            print("k = ", k)

        i += 1

        if len(cs) > len(lcs):
            lcs = cs

        print("lcs ", lcs)

    return lcs   
