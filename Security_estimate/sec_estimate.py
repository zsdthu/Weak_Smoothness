""" Security estimates."""
from math import *

def delta_beta(b):
    """ Compute root Hermite factor delta on input BKZ block size b """
    return (((pi * b) ** (1. / b) * b) / (2 * pi * exp(1))) ** (0.5 / (b - 1))
    
def classic_core_svp(b):
    """ Compute classical cost of SVP in dim-b """
    return 0.292 * b

def quantum_core_svp(b):
    """ Compute quantum cost of SVP in dim-b """
    return 0.265 * b
    # return 0.257 * b

def smooth(eps,n):
    return sqrt(log(2.0 * n * (1.0 + (1.0 / eps)))) / sqrt(2 * pi * pi)

def sis_attack(m, n, Q, sig_norm):  
    """ The cost of forgery attack
    Args:
        m: row number
        n: column number of A'
        Q: modulus
        sig_norm: length of the preimage (z0, z_1)
    Returns:
        bit-security in the classical and quantum setting
    """
    Dim = 2 * m + n
    the_k = 0
    min_beta = 3000
    for k in range(n + m):
        dim = Dim - k
        for beta in range(200, 2000):
            delta = delta_beta(beta)
            Vol_root = Q ** (1. * m / dim)
            LHS = (delta ** dim) * Vol_root
            RHS = sig_norm
            if RHS > LHS:
                if min_beta > beta:
                    min_beta = beta
                    the_k = k
                break
    print("SIS:", min_beta)
    return (round(classic_core_svp(min_beta)), round(quantum_core_svp(min_beta)))

def forgery_attack(m, n, Q, sig_norm):  
    """ The cost of forgery attack
    Args:
        m: row number
        n: column number of A'
        Q: modulus
        sig_norm: length of the preimage (z0, z_1)
    Returns:
        bit-security in the classical and quantum setting
    """
    Dim = m + n
    the_k = 0
    min_beta = 3000
    for k in range(m//2):
        dim = Dim - k
        for beta in range(200, 2000):
            delta = delta_beta(beta)
            Vol_root = Q ** (1. * m / dim)
            LHS = (delta ** dim) * Vol_root
            RHS = sig_norm
            if RHS > LHS:
                if min_beta > beta:
                    min_beta = beta
                    the_k = k
                break
    print("SIS:", min_beta)
    return (round(classic_core_svp(min_beta)), round(quantum_core_svp(min_beta)))

def lwe_attack(m, n, Q, sigma_f): 
    """ The cost of key-recovery attack 
    Args:
        m: number of samples
        n: dimension of secrets
        Q: modulus
    Returns:
        bit-security in the classical/quantum setting
    """
    classical_cost = []
    quantum_cost = []
    for l in range(m): # Use only (m-l) LWE samples
        dim = m + n - l
        beta = 200
        while (1):
            delta = delta_beta(beta)
            LHS = sqrt(beta) * sigma_f * sqrt(3. / 4)  # the faoctor sqrt(3/4) for a more conservative security
            # LHS = sqrt(beta) * sigma_f
            RHS = Q ** (1. * (m - l) / (dim + 1)) * delta ** (2 * beta - dim - 1)
            if RHS > LHS: break
            else: beta += 1
        classical_cost.append(classic_core_svp(beta))
        quantum_cost.append(quantum_core_svp(beta))
    print("LWE:", min(classical_cost) / 0.292)
    return round(min(classical_cost)), round(min(quantum_cost))

def preimage_size(n, std):
    """ Compute the storage size (in byte) of dim-n Gaussian vector with standard deviation std
    Args:
        n: dimension
        std: standard deviation of Gaussian
    Returns:
        number of bytes to store dim-n Gaussian vector
    """
    entropy = ceil((0.5 + log(sqrt(2 * pi) * std)) / log(2) * n)
    return entropy / 8


def main_KGPV_512():
    n = 512
    eps = 1.0 / (2 * n)   #epslion
    q = 953      # 512
    # q = 1601    #1024
    print("pk", n * (ceil(log(q) / log(2))) / 8)
    r = smooth(eps,1)   #smooth parameter
    print("smooth",r)
    c = 1.38#for 512
    sigma_pre = c * sqrt(q) * r   #signature sigma 
    print("signature sigma ",sigma_pre)   
    sig_size = ceil(preimage_size(n, sigma_pre) + 40)
    print("sig_size",sig_size)
    tau = 1.1
    sig_norm = tau * sigma_pre * sqrt(2 * n) #signature norm 1.1*sigma*sqrt(2n)
    print("sig_norm", sig_norm)
    print("sig_norm / q",sig_norm / q)


    sigma_f = c * sqrt(q) / sqrt(2 * n) #f,g sigma
    print("sigma_f = %s" % sigma_f)
    classical_key_rec_sec, quantum_key_rec_sec = lwe_attack(n, n, q, sigma_f) # Key Recovery attack
    # print(" Key Recovery")
    print("Key Recovery", classical_key_rec_sec,quantum_key_rec_sec)

    classical_forg_sec, quantum_forg_sec = forgery_attack(n, n, q, sig_norm)  # Forgery attack
    # print("Forgery attack")
    print("Forgery attack", classical_forg_sec,quantum_forg_sec)
   
def main_KGPV_1024():
    n = 1024
    eps = 1.0 / (2 * n)   #epslion
    q = 1949    #1024
    print("pk", n * (ceil(log(q) / log(2))) / 8)
    r = smooth(eps,1)   #smooth parameter
    print("smooth",r)
    c = 1.364 #for 1024
    sigma_pre = c * sqrt(q) * r   #signature sigma 
    print("signature sigma ",sigma_pre)   
    sig_size = ceil(preimage_size(n, sigma_pre) + 40)
    print("sig_size",sig_size)
    tau = 1.1
    sig_norm = tau * sigma_pre * sqrt(2 * n) #signature norm 1.1*sigma*sqrt(2n)
    print("sig_norm", sig_norm)
    print("sig_norm / q",sig_norm / q)

    sigma_f = c * sqrt(q) / sqrt(2 * n) #f,g sigma
    print("sigma_f = %s" % sigma_f)
    classical_key_rec_sec, quantum_key_rec_sec = lwe_attack(n, n, q, sigma_f) # Key Recovery attack
    # print(" Key Recovery")
    print("Key Recovery", classical_key_rec_sec,quantum_key_rec_sec)

    classical_forg_sec, quantum_forg_sec = forgery_attack(n, n, q, sig_norm)  # Forgery attack
    # print("Forgery attack")
    print("Forgery attack", classical_forg_sec,quantum_forg_sec)

def main_pei_1024():
    n = 1024
    eps = 1/4
    q = 80000      # for 1024
    print("pk", n * (ceil(log(q) / log(2))) / 8)
    r = smooth(eps, 2 * n)   #smooth parameter
    c = 8
    sigma_pre = c * sqrt(q) * r   #signature sigma 
    print("signature sigma ",sigma_pre)   
    sig_size = ceil(preimage_size(n, sigma_pre) + 40)
    print("sig_size",sig_size)

    tau = 1.05
    sig_norm = tau * sigma_pre * sqrt(2 * n) #signature norm 1.1*sigma*sqrt(2n)
    print("sig_norm", sig_norm)
    print("sig_norm / q",sig_norm / q)

    norm_fg = 68
    sigma_f = norm_fg / sqrt(2 * n)
    classical_key_rec_sec, quantum_key_rec_sec = lwe_attack(n, n, q, sigma_f) # Key Recovery attack
    # print("Key Recovery")
    print("Key Recovery", classical_key_rec_sec,quantum_key_rec_sec)

    classical_forg_sec, quantum_forg_sec = forgery_attack(n, n, q, sig_norm)  # Forgery attack
    # print("Forgery attack")
    print("Forgery attack", classical_forg_sec,quantum_forg_sec)
def main_pei_512():
    n = 512
    eps = 1/4
    q = 20000 #for 512
    print("pk", n * (ceil(log(q) / log(2))) / 8)
    r = smooth(eps, 2 * n)   #smooth parameter
    c = 6 #for 512
    sigma_pre = c * sqrt(q) * r   #signature sigma 
    print("signature sigma ",sigma_pre)   
    sig_size = ceil(preimage_size(n, sigma_pre) + 40)
    print("sig_size",sig_size)
    tau = 1.05
    sig_norm = tau * sigma_pre * sqrt(2 * n) #signature norm 1.1*sigma*sqrt(2n)
    print("sig_norm", sig_norm)
    print("sig_norm / q",sig_norm / q)

    norm_fg = 41
    sigma_f = norm_fg / sqrt(2 * n)
    classical_key_rec_sec, quantum_key_rec_sec = lwe_attack(n, n, q, sigma_f) # Key Recovery attack
    print("Key Recovery", classical_key_rec_sec,quantum_key_rec_sec)

    classical_forg_sec, quantum_forg_sec = forgery_attack(n, n, q, sig_norm)  # Forgery attack
    # print("Forgery attack")
    print("Forgery attack",classical_forg_sec,quantum_forg_sec)

def main_KGPV_1024_5_plus():
    n = 1024
    eps = 1.0 / (2 * n)   #epslion
    q = 3329    #1024
    print("pk", n * (ceil(log(q) / log(2))) / 8)
    r = smooth(eps,1)   #smooth parameter
    print("smooth",r)
    c = 1.7
    sigma_pre = c * sqrt(q) * r   #signature sigma 
    print("signature sigma ",sigma_pre)   
    sig_size = ceil(preimage_size(n, sigma_pre) + 40)
    print("sig_size",sig_size)
    tau = 1.1
    sig_norm = tau * sigma_pre * sqrt(2 * n) #signature norm 1.1*sigma*sqrt(2n)
    print("sig_norm", sig_norm)
    print("sig_norm / q",sig_norm / q)

    sigma_f = c * sqrt(q) / sqrt(2 * n) #f,g sigma
    print("sigma_f = %s" % sigma_f)
    classical_key_rec_sec, quantum_key_rec_sec = lwe_attack(n, n, q, sigma_f) # Key Recovery attack
    # print(" Key Recovery")
    print("Key Recovery", classical_key_rec_sec,quantum_key_rec_sec)

    classical_forg_sec, quantum_forg_sec = forgery_attack(n, n, q, sig_norm)  # Forgery attack
    # print("Forgery attack")
    print("Forgery attack", classical_forg_sec,quantum_forg_sec)
    
if __name__ == '__main__':

    print("parameter for Falcon variant")
    print("NIST level 1 in Table 4")
    main_KGPV_512()
    print("\n\n")
    print("NIST level 5 in Table 4")
    main_KGPV_1024()
    print("\n\n")
    print("NIST level 5+ in Table 4")
    main_KGPV_1024_5_plus()
    print("\n\n")

    print("parameter for GPV with modified Peikert")
    print("n=512 in Table 5")
    main_pei_512()
    print("\n\n")
    print("n=1024 in Table 5")
    main_pei_1024()
    print("\n\n")

