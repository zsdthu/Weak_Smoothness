rm -f test_compress
gcc ./rANS/compress.c ./rANS/rans_byte.c ./test_compress.c -o test_compress
./test_compress