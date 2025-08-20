#include <stdint.h>

#include "../params.h"

// compress sig to a PARAM_COMPRESSED_SIG_BYTES sized buf
// return 0 if compression is successful
int compress_sig(uint8_t buf[PARAM_COMPRESSED_SIG_BYTES], const int16_t sig[PARAM_SIG_SIZE]);

// decompress sig from buf
// buf should point the first used bytes
// return 0 if decompress is successful.
int decompress_sig(int16_t sig[PARAM_SIG_SIZE], const uint8_t buf[PARAM_COMPRESSED_SIG_BYTES]);