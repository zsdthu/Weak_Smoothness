## Codes for GPV with weak smoothness

## 1. Security Estimate
In this folder, we provide the exploitable codes, which are involved in the estimation results of Table 4 and Table 5 in Section 5.
```
$ cd Security_estimate/
$ python3 sec_estimate.py
``` 

## 2. Theta function division
We implement the Theta function division (see Algorithm 4) by portable C codes based on the reference implementation of Falcon. Furthermore, we also test the speed of the Theta function division via cycle counts.  
```
$ cd Theta_Function_Division/
$ make clean
$ make 
$ cd build/
$ ./speed
```

## 3. NTRU trapdoor generation
We also implement the NTRU trapdoor generation, which is shown in Algorithm 5. Our codes are based on the [Python implementation of Falcon](https://github.com/tprest/falcon.py) (written by Thomas Prest).
```
$ cd NTRU_trapdoor_generation/
$ python3 generate_NTRUbasis.py
```


## 4. rANS
We implement the rANS algorithm involved in Section 6. For specific operating rules, see the README.md in the file rANS_code.