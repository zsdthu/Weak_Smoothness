/*
Copyright (c) **************
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freq.h"
#include "rans_byte.h"

#define PARAM_RANS_SCALE (1u << PARAM_RANS_SCALE_BITS)

static RansEncSymbol esyms_sig[PARAM_RANS_SYM_COUNT];
static RansSymbol symbol_sig[PARAM_RANS_SCALE] = {0};
static RansDecSymbol dsyms_sig[PARAM_RANS_SYM_COUNT];

void symbol_table(RansSymbol *symbol, const uint32_t *freq, size_t alphabet_size)
{
    int pos = 0;
    for (size_t sym = 0; sym < alphabet_size; sym++)
    {
        for (uint32_t i = 0; i < freq[sym]; i++)
            symbol[pos++] = sym;
    }
}

void cum_freq_table(uint32_t *cum_freq, const uint32_t *freq, size_t alphabet_size)
{
    cum_freq[0] = 0;
    for (size_t i = 1; i < alphabet_size; i++)
    {
        cum_freq[i] = cum_freq[i - 1] + freq[i - 1];
    }
}

void encode_symbols(RansEncSymbol *esyms, const uint32_t *freq, size_t alphabet_size)
{
    uint32_t cum_freq[alphabet_size];
    cum_freq_table(cum_freq, freq, alphabet_size);

    for (size_t i = 0; i < alphabet_size; i++)
    {
        RansEncSymbolInit(&esyms[i], cum_freq[i], freq[i], PARAM_RANS_SCALE_BITS);
    }
}

void decode_symbols(RansDecSymbol *dsyms, RansSymbol *symbol, const uint32_t *freq, size_t alphabet_size)
{
    uint32_t cum_freq[alphabet_size];
    cum_freq_table(cum_freq, freq, alphabet_size);

    symbol_table(symbol, freq, alphabet_size);

    for (size_t i = 0; i < alphabet_size; i++)
    {
        RansDecSymbolInit(&dsyms[i], cum_freq[i], freq[i]);
    }
}

void precomputations_rans()
{
    encode_symbols(esyms_sig, f_sig, PARAM_RANS_SYM_COUNT);
    decode_symbols(dsyms_sig, symbol_sig, f_sig, PARAM_RANS_SYM_COUNT);
}

void print_esym(RansEncSymbol x)
{
    printf("{%uu,%uu,%uu,%uu,%uu}", x.x_max, x.rcp_freq, x.bias, x.cmpl_freq, x.rcp_shift);
}

void print_dsym(RansDecSymbol x)
{
    printf("{%uu,%uu}", x.start, x.freq);
}

int main()
{
    precomputations_rans();
    printf("#ifndef RANS_PARAMS_H\n");
    printf("#define RANS_PARAMS_H\n");

    printf("// clang-format off\n");
    printf("\n");

    printf("// table info: %s\n", TABLE_INFO);
    size_t esyms_sig_size = sizeof(esyms_sig);
    size_t symbol_sig_size = sizeof(symbol_sig);
    size_t dsyms_sig_size = sizeof(dsyms_sig);
    printf("// table size: esyms_sig: %ldB dsyms_sig: %ldB symbol_sig: %ldB total: %ldB\n", esyms_sig_size,
           dsyms_sig_size, symbol_sig_size, esyms_sig_size + symbol_sig_size + dsyms_sig_size);
    printf("\n");

    printf("#include \"rans_byte.h\"\n");
    printf("\n");

    printf("#define PARAM_RANS_SCALE_BITS %u\n", PARAM_RANS_SCALE_BITS);
    printf("\n");

    printf("static RansEncSymbol esyms_sig[] = {");
    for (int i = 0; i < PARAM_RANS_SYM_COUNT; i++)
    {
        print_esym(esyms_sig[i]);
        printf(",");
    }
    printf("};\n");

    printf("static RansDecSymbol dsyms_sig[] = {");
    for (int i = 0; i < PARAM_RANS_SYM_COUNT; i++)
    {
        print_dsym(dsyms_sig[i]);
        printf(",");
    }
    printf("};\n");

    printf("static RansSymbol symbol_sig[] = {");
    for (int i = 0; i < PARAM_RANS_SCALE; i++)
    {
        printf("%uu,", symbol_sig[i]);
    }
    printf("};\n");

    printf("\n");
    printf("// clang-format off\n");

    printf("#endif\n");
}
