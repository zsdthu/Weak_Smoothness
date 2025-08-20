#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../params.h"
#include "compress.h"
#include "rans_byte.h"

#define INVALID_VALUE -1
#define OUT_OF_BUFFER -2
#define INVALID_LENGTH -3
#define INVALID_STATE -4

int compress_sig(uint8_t buf[PARAM_COMPRESSED_SIG_BYTES], const int16_t sig[PARAM_SIG_SIZE])
{
    RansState rans;
    uint8_t *ptr_lowbits = buf + PARAM_COMPRESSED_SIG_BYTES;
    uint8_t *ptr = buf + (PARAM_COMPRESSED_SIG_BYTES - PARAM_SIG_SIZE);
    uint16_t sig_i_abs;

    RansEncInit(&rans);

    for (size_t i = PARAM_SIG_SIZE - 1; i < PARAM_SIG_SIZE; --i)
    {
        if (sig[i] <= -PARAM_SIG_TRUNC || sig[i] >= PARAM_SIG_TRUNC)
        {
            return INVALID_VALUE;
        }

        if (sig[i] < 0)
        {
            sig_i_abs = (uint16_t)(-sig[i]);
            *(--ptr_lowbits) = 0x80 | (sig_i_abs & 0x7f);
        }
        else
        {
            sig_i_abs = (uint16_t)(sig[i]);
            *(--ptr_lowbits) = sig_i_abs & 0x7f;
        }

        RansEncPutSymbol(&rans, &ptr, sig_i_abs >> 7);

        // reserve at least 5 bytes: 1 byte 0x80 || 4 bytes final state
        // RansEncPutSymbol will emit at most 2 bytes
        if (ptr < buf + 5)
        {
            return OUT_OF_BUFFER;
        }
    }

    RansEncFinalize(&rans, &ptr);

    *(--ptr) = 0x80;

    // printf("comsig+1:\t%lu\n", buf + PARAM_COMPRESSED_SIG_BYTES - ptr);

    memset(buf, 0x00, ptr - buf); // padding

    return 0;
}

int decompress_sig(int16_t sig[PARAM_SIG_SIZE], const uint8_t buf[PARAM_COMPRESSED_SIG_BYTES])
{
    RansState rans;
    const uint8_t *const ptr_lowbits_start = buf + (PARAM_COMPRESSED_SIG_BYTES - PARAM_SIG_SIZE);
    const uint8_t *ptr_lowbits = ptr_lowbits_start;
    const uint8_t *ptr = buf;
    RansSymbol sym;

    while (*ptr != 0x80)
    {
        if (*(ptr++) != 0x00 || ptr > ptr_lowbits_start - 4)
        {
            return INVALID_LENGTH;
        }
    }

    ++ptr;

    if (RansDecInit(&rans, &ptr) != 0)
    {
        return INVALID_STATE; // The initial state is out of range
    }

    for (size_t i = 0; i < PARAM_SIG_SIZE; i++)
    {
        sym = RansDecGetSymbol(&rans, &ptr);

        sig[i] = (((uint16_t)sym) << 7) | ((*(ptr_lowbits)) & 0x7f);

        if (sig[i] >= PARAM_SIG_TRUNC)
        {
            return INVALID_VALUE;
        }

        if ((*(ptr_lowbits++)) & 0x80)
        {
            if (sig[i] == 0)
            {
                return INVALID_VALUE; // -0
            }
            sig[i] = -sig[i];
        }

        if (ptr > ptr_lowbits_start)
        {
            return OUT_OF_BUFFER; // The coding is too short
        }
    }

    if (RansDecFinalize(&rans) != 0)
    {
        return INVALID_STATE; // The final state for decoding does not match the initial state for encoding
    }

    if (ptr != ptr_lowbits_start)
    {
        return INVALID_LENGTH; // The coding has redundant parts
    }

    return 0; // success
}