// Simple byte-aligned rANS encoder/decoder - public domain - Fabian 'ryg' Giesen 2014
// Modified by ********
// Not intended to be "industrial strength"; just meant to illustrate the general
// idea.

#include "rans_byte.h"
#include "rans_params.h"

// L ('l' in the paper) is the lower bound of our normalization interval.
// Between this and our byte-aligned emission, we use 31 (not 32!) bits.
// This is done intentionally because exact reciprocals for 31-bit uints
// fit in 32-bit uints: this permits some optimizations during encoding.
#define PARAM_RANS_BYTE_L (1u << 23) // lower bound of our normalization interval
#define PARAM_RANS_SCALE_BITS_MASK ((1u << PARAM_RANS_SCALE_BITS) - 1)

// Initialize a rANS encoder.
// Fix the initial state for encoding
void RansEncInit(RansState *const r)
{
    *r = PARAM_RANS_BYTE_L;
}

// Encodes a given symbol.
void RansEncPutSymbol(RansState *const r, uint8_t **const pptr, const RansSymbol sym)
{
    const RansEncSymbol *const esym = esyms_sig + sym;

    // renormalize
    if ((*r) >= esym->x_max)
    {
        do
        {
            *(--(*pptr)) = (uint8_t)((*r) & 0xff);
            (*r) >>= 8;
        } while ((*r) >= esym->x_max);
    }

    // x = C(s,x)
    // NOTE: written this way so we get a 32-bit "multiply high" when
    // available. If you're on a 64-bit platform with cheap multiplies
    // (e.g. x64), just bake the +32 into rcp_shift.
    const uint32_t q = (uint32_t)(((uint64_t)(*r) * esym->rcp_freq) >> 32) >> esym->rcp_shift;
    *r = (*r) + esym->bias + q * esym->cmpl_freq;
}

// Flushes the rANS encoder.
void RansEncFinalize(RansState *const r, uint8_t **const pptr)
{
    *pptr -= 4;
    (*pptr)[0] = (uint8_t)((*r) >> 0);
    (*pptr)[1] = (uint8_t)((*r) >> 8);
    (*pptr)[2] = (uint8_t)((*r) >> 16);
    (*pptr)[3] = (uint8_t)((*r) >> 24);
}

// Initializes a rANS decoder.
// Return 0 if successful
int RansDecInit(RansState *const r, const uint8_t **const pptr)
{
    const RansState rans = (*pptr)[0] << 0 | (*pptr)[1] << 8 | (*pptr)[2] << 16 | (*pptr)[3] << 24;
    if (rans < PARAM_RANS_BYTE_L || rans >= (PARAM_RANS_BYTE_L << 8))
    {
        return -1; // The initial state is out of range
    }
    *r = rans;
    *pptr += 4;
    return 0;
}

// Decodes a given symbol.
RansSymbol RansDecGetSymbol(RansState *const r, const uint8_t **const pptr)
{
    // s, x = D(x)
    const RansState r_low = *r & PARAM_RANS_SCALE_BITS_MASK;
    const RansState r_high = *r >> PARAM_RANS_SCALE_BITS;
    const RansSymbol sym = symbol_sig[r_low];
    const RansDecSymbol *const dsym = dsyms_sig + sym;

    *r = dsym->freq * r_high + r_low - dsym->start;

    // renormalize
    if ((*r) < PARAM_RANS_BYTE_L)
    {
        do
            *r = ((*r) << 8) | *((*pptr)++);
        while ((*r) < PARAM_RANS_BYTE_L);
    }
    return sym;
}

// Finalize a rANS decoding process.
// Return 0 if decode is successful.
int RansDecFinalize(const RansState *const r)
{
    if (*r != PARAM_RANS_BYTE_L)
    {
        return -1; // the final state is inconsistent with the initial state
    }

    return 0;
}