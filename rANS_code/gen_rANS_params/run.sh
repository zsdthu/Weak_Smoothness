echo gen rans params sec1
python gen_highbit_freq.py "1055.6" > highbit_freq_sec1.h &&
cp highbit_freq_sec1.h freq.h &&
gcc -Wall gen_rans_params.c -o gen_rans_params_sec1 &&
./gen_rans_params_sec1 > rans_params_sec1.h

echo gen rans params sec3
python gen_highbit_freq.py "1253.8" > highbit_freq_sec3.h &&
cp highbit_freq_sec3.h freq.h &&
gcc -Wall gen_rans_params.c -o gen_rans_params_sec3 &&
./gen_rans_params_sec3 > rans_params_sec3.h

echo gen rans params sec5
python gen_highbit_freq.py "1415.3" > highbit_freq_sec5.h &&
cp highbit_freq_sec5.h freq.h &&
gcc -Wall gen_rans_params.c -o gen_rans_params_sec5 &&
./gen_rans_params_sec5 > rans_params_sec5.h
