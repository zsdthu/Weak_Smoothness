# %%
from utils import *
import sys

from sage.all_cmdline import *  # import sage library

set_random_seed(42)

SIGMA = float(sys.argv[1])
SAMPLE_COUNT_POWER = 28
KEEP_BITS = 7
QUANT_SCALE = 12

SAMPLE_COUNT = 2**SAMPLE_COUNT_POWER
TRUNC = int(SIGMA * 10)
SYM_COUNT = TRUNC >> KEEP_BITS
# %%
print("// clang-format off")
print()

print(
    '#define TABLE_INFO "SIGMA:',
    SIGMA,
    "TRUNC:",
    TRUNC,
    "QUANT_SCALE:",
    QUANT_SCALE,
    "SAMPLE_COUNT_POWER:",
    SAMPLE_COUNT_POWER,
    "KEEP_BITS:",
    KEEP_BITS,
    "TABLE_SIZE:",
    SYM_COUNT,
    '"',
)
A = dgg(SAMPLE_COUNT, SIGMA)
# %%
_, TO_COMP_TRAIN = split_keep_to_comp(A, KEEP_BITS)
# %%
freq_to_comp = calc_freq(TO_COMP_TRAIN, SYM_COUNT, 0)
freq_quat_to_comp = quat_freq(freq_to_comp, QUANT_SCALE)
# %%
check_quat_freq(freq_quat_to_comp, QUANT_SCALE)
# %%
print("#define PARAM_RANS_SCALE_BITS", QUANT_SCALE)
print("#define PARAM_RANS_TRUNC", len(freq_quat_to_comp) << KEEP_BITS)
print_freq_quat(freq_quat_to_comp)
# %%
print()
print("// clang-format on")