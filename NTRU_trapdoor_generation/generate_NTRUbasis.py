import sys
sys.path.insert(0, './falcon')

import numpy as np
from math import sqrt
from numpy import linalg
from fft import mul, add, adj, fft, sub, div, ifft
from ntt import ntt
from ntrugen import reduce, karamul,galois_conjugate,lift



def xgcd(b, n, q):
    """
    Compute the extended GCD of two integers b and n.
    Return d, u, v such that d = u * b + v * n, and d is the GCD of b, n.
    """
    x0, x1, y0, y1 = 1, 0, 0, 1
    while n != 0:
        q, b, n = b // n, n, b % n
        x0, x1 = x1, x0 - q * x1
        y0, y1 = y1, y0 - q * y1
    return b, x0, y0

def field_norm(a):
    """
    Project an element a of Q[x] / (x ** n + 1) onto Q[x] / (x ** (n // 2) + 1).
    Only works if n is a power-of-two.
    """
    n2 = len(a) // 2
    ae = [a[2 * i] for i in range(n2)]
    ao = [a[2 * i + 1] for i in range(n2)]
    ae_squared = karamul(ae, ae)
    ao_squared = karamul(ao, ao)
    res = ae_squared[:]
    for i in range(n2 - 1):
        res[i + 1] -= ao_squared[i]
    res[0] += ao_squared[n2 - 1]
    return res

def ntru_solve(f, g, q):
    """
    Solve the NTRU equation for f and g.
    Corresponds to NTRUSolve in Falcon's documentation.
    """
    n = len(f)
    if n == 1:
        f0 = f[0]
        g0 = g[0]
        d, u, v = xgcd(f0, g0, q)
        if d != 1:
            # print("error____________________")
            raise ValueError
        else:
            return [- q * v], [q * u]
    else:
        fp = field_norm(f)
        gp = field_norm(g)
        Fp, Gp = ntru_solve(fp, gp,q)
        F = karamul(lift(Fp), galois_conjugate(g))
        G = karamul(lift(Gp), galois_conjugate(f))
        F, G = reduce(f, g, F, G)
        return F, G

def antrag_genfg(n, q, r, R, alpha, beta):
    check = 0
    number = 0
    qhigh = R * q 
    qlow = r * q 

    while(check == 0):
        number = number + 1
        # print(number)
        rand1 = [np.random.rand() for i in range(n//2)]
        rand2 = [np.random.rand() for i in range(n//2)]
        rand3 = [np.random.rand() for i in range(n//2)]
        rand4 = [np.random.rand() for i in range(n//2)]
        # print(rand1)
        # print(rand2)
        z = [sqrt(qlow + (qhigh - qlow)* rand1[i]) for i in range(n//2)]
        af = [z[i] * np.cos(np.pi / 2 * rand2[i]) for i in range(n//2)]
        ag = [z[i] * np.sin(np.pi / 2 * rand2[i]) for i in range(n//2)]
        f1 = [af[i] * np.cos(2 * np.pi * rand3[i]) + (af[i] * np.sin(2 * np.pi * rand3[i]))* 1j for i in range(n//2)]
        f2 = [af[i] * np.cos(2 * np.pi * rand3[i]) - (af[i] * np.sin(2 * np.pi * rand3[i]))* 1j for i in range(n//2)]
        f_fftinv = f1 + f2
        g1 = [ag[i] * np.cos(2 * np.pi * rand4[i]) + (ag[i] * np.sin(2 * np.pi * rand4[i])) * 1j for i in range(n//2)]
        g2 = [ag[i] * np.cos(2 * np.pi * rand4[i]) - (ag[i] * np.sin(2 * np.pi * rand4[i])) * 1j for i in range(n//2)]
        g_fftinv = g1 + g2
        # print(f_fftinv)
        f_double = ifft(f_fftinv)
        # print(f_double)
        g_double = ifft(g_fftinv)
        ffgg_double =  add(mul(f_double,adj(f_double)),mul(g_double,adj(g_double)))
        ffgg_double_mid = fft(ffgg_double)
        ffgg_double_fft  = [ffgg_double_mid[l].real for l in range(n)]
        # print("ffgg_double_fft", max(ffgg_double_fft),ffgg_double_fft.index(max(ffgg_double_fft)),min(ffgg_double_fft),ffgg_double_fft.index(min(ffgg_double_fft)))          
        f = [round(f_double[i]) for i in range(n)]
        g = [round(g_double[i]) for i in range(n)]
        ffgg =  add(mul(f,adj(f)),mul(g,adj(g)))
        ffgg_mid = fft(ffgg)
        ffgg_fft  = [ffgg_mid[l].real for l in range(n)]  
        if((max(ffgg_fft) < beta * q) and (min(ffgg_fft) > alpha * q)):
            check = 1     
    return f,g

    
def gen_basis(n, q, r, R, alpha, beta,tau):
    while 1:
        f, g, F, G = ntru_gen(n, q, r, R, alpha, beta)
        ffgg = add(mul(f,adj(f)),mul(g,adj(g)))
        FFGG =  add(mul(F,adj(F)),mul(G,adj(G)))
        fgFG_mid = fft(add(ffgg, FFGG))
        fgFG = [fgFG_mid[l].real for l in range(n)]
        lam1 = [sqrt((fgFG[i] + sqrt(fgFG[i] * fgFG[i] - (4*q*q))) / 2) for i in range(n)]
        quality = max(lam1) / sqrt(q)
        if max(lam1) < tau * sqrt(q):
            print("basis quality = %f" % quality)
            return f,g,F,G
        print("basis quality = %f , big than %d ,restart" % (quality , tau))

def ntru_gen(n, q, r, R, alpha, beta):
    """
    Implement the algorithm 5 (NTRUGen) of Falcon's documentation.
    At the end of the function, polynomials f, g, F, G in Z[x]/(x ** n + 1)
    are output, which verify f * G - g * F = q mod (x ** n + 1).
    """
    while True:
        f,g = antrag_genfg(n, q, r, R, alpha, beta)
        f_ntt = ntt(f)
        if any((elem == 0) for elem in f_ntt):
            continue
        try:
            F, G = ntru_solve(f, g, q)
            F = [int(coef) for coef in F]
            G = [int(coef) for coef in G]
            return f, g, F, G
        # If the NTRU equation cannot be solved, a ValueError is raised
        # In this case, we start again
        except ValueError:
            continue


def main_antrag_final_512():
    n = 512
    q = 20000
    r = 1.0 / 14
    R = 1.0/ 12
    alpha = 1.0 / 25
    beta = 3.0 / 20
    tau = 6

    f,g,F,G = gen_basis(n, q, r, R, alpha, beta, tau)
    print("f:",f)
    print("g:",f)
    print("F:",F)
    print("G:",G)

def main_antrag_final_1024():
    n = 1024
    q = 80000
    r = 1.0 / 19
    R = 1.0/ 17
    alpha = 3.0 / 100
    beta = 1.0 / 10
    tau = 8

    f,g,F,G = gen_basis(n, q, r, R, alpha, beta, tau)
    print("f:",f)
    print("g:",f)
    print("F:",F)
    print("G:",G)

if __name__ == "__main__":

    print("The Peikert sampler NTRU trapdoor generation with n=512")
    main_antrag_final_512()

    print("\n")

    print("The Peikert sampler NTRU trapdoor generation with n=1024")
    main_antrag_final_1024()
    