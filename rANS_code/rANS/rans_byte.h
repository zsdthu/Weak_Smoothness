// Simple byte-aligned rANS encoder/decoder - public domain - Fabian 'ryg' Giesen 2014
// Modified by *****
// Not intended to be "industrial strength"; just meant to illustrate the general
// idea.

#ifndef RANS_BYTE_H
#define RANS_BYTE_H

#include <stdint.h>

// READ ME FIRST:
//
// This is designed like a typical arithmetic coder API, but there's three
// twists you absolutely should be aware of before you start hacking:
//
// 1. You need to encode data in *reverse* - last symbol first. rANS works
//    like a stack: last in, first out.
// 2. Likewise, the encoder outputs bytes *in reverse* - that is, you give
//    it a pointer to the *end* of your buffer (exclusive), and it will
//    slowly move towards the beginning as more bytes are emitted.
// 3. Unlike basically any other entropy coder implementation you might
//    have used, you can interleave data from multiple independent rANS
//    encoders into the same bytestream without any extra signaling;
//    you can also just write some bytes by yourself in the middle if
//    you want to. This is in addition to the usual arithmetic encoder
//    property of being able to switch models on the fly. Writing raw
//    bytes can be useful when you have some data that you know is
//    incompressible, and is cheaper than going through the rANS encode
//    function. Using multiple rANS coders on the same byte stream wastes
//    a few bytes compared to using just one, but execution of two
//    independent encoders can happen in parallel on superscalar and
//    Out-of-Order CPUs, so this can be *much* faster in tight decoding
//    loops.
//
//    This is why all the rANS functions take the write pointer as an
//    argument instead of just storing it in some context struct.

// --------------------------------------------------------------------------

// State for a rANS encoder. Yep, that's all there is to it.
typedef uint32_t RansState;
typedef uint8_t RansSymbol;

// --------------------------------------------------------------------------

// That's all you need for a full encoder; below here are some utility
// functions with extra convenience or optimizations.

// Encoder symbol description
// This (admittedly odd) selection of parameters was chosen to make
// RansEncPutSymbol as cheap as possible.
typedef struct
{
    uint32_t x_max;     // (Exclusive) upper bound of pre-normalization interval
    uint32_t rcp_freq;  // Fixed-point reciprocal frequency
    uint32_t bias;      // Bias
    uint16_t cmpl_freq; // Complement of frequency: (1 << scale_bits) - freq
    uint16_t rcp_shift; // Reciprocal shift
} RansEncSymbol;

// Decoder symbols are straightforward.
typedef struct
{
    uint16_t start; // Start of range.
    uint16_t freq;  // Symbol frequency.
} RansDecSymbol;

void RansEncInit(RansState *const r);

void RansEncPutSymbol(RansState *const r, uint8_t **const pptr, const RansSymbol sym);

void RansEncFinalize(RansState *const r, uint8_t **const pptr);

int RansDecInit(RansState *const r, const uint8_t **const pptr);

RansSymbol RansDecGetSymbol(RansState *const r, const uint8_t **const pptr);

int RansDecFinalize(const RansState *r);

#endif // RANS_BYTE_H