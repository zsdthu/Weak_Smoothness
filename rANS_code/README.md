# rANS Compression
## Compile & Run
```
chmod +x run.sh
./run.sh
```
This script encodes 1584 (PARAM_M + PARAM_N) `int16` elements sampled from a Gaussian distribution with mean $\mu = 0$ and standard deviation $\sigma = 1055.6$. The data is compressed into 2410 bytes and then decoded back to `int16` to verify exact reconstruction.

The rANS codes are placed in `rANS` folder.

## Recomputing rANS Parameters
If your input data follows a different probability distribution, you can regenerate the rANS parameters using the codes in `gen_rANS_params`. Please refer to `gen_rANS_params/run.sh`.