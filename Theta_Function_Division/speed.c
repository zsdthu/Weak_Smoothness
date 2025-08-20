#include<stdio.h>
#include<stdint.h>
#include"inner.h"
#include "cpucycles.h"

/**
 * * The version of ThetaDiv function without precomputation.
 * * The ThetaDiv function with sigma=0.62 in our Falcon-512 variant.
 * * For Falcon-1024 variant with sigma=1.2, we only run the ThetaDiv function with only one iteration.
 * *
 * */
//fpr ThetaDiv(fpr mu, fpr isigma)
//{
//	size_t i;
//    fpr r1 = fpr_costwopi_2(mu);
//	r1 = fpr_add(fpr_double(r1), fpr_minustwo); // 2r_1-2
//	fpr r2 = fpr_expm(fpr_mul(fpr_twopipi, fpr_sqr(fpr_inv(isigma))));
//	fpr delta = fpr_add(fpr_one, fpr_div(r1, fpr_add(fpr_add(r2, fpr_two), fpr_inv(r2))));
//    fpr r2_sqr = fpr_sqr(r2);
//     for (i = 0; i <= 1; i++)
//     {
//        r2 = fpr_mul(r2, r2_sqr);
//        delta = fpr_mul(delta, fpr_add(fpr_of(1), fpr_div(r1, fpr_add(fpr_add(r2, fpr_two), fpr_inv(r2)))));
//
//     }
//	return delta;
//}

// The version of ThetaDiv function with precomputation only for Falcon parameters
fpr ThetaDiv(fpr mu, fpr deno)
{
    fpr r1 = fpr_costwopi_2(mu);
    r1 = fpr_add(r1, fpr_minusone);
    fpr delta = fpr_add(fpr_one, fpr_mul(r1, deno));
    return delta;
}

int main(){
	S a,b,c;
    fpr r2;
    fpr deno_arr[256];
    fpr isigma_arr[256];
    double fixed_center, fixed_sigma;
    double res;

    inner_shake256_context rng;
    sampler_context sc;

    // the number of iterations for testing speed
    int iteration = 100 * 1000;
    int dim = 512;
    
    inner_shake256_init(&rng);
    inner_shake256_inject(&rng, (const void *)"test sampler", 12);
    inner_shake256_flip(&rng);
    Zf(prng_init)(&sc.p, &rng);

    uint64_t begin, end, tt;
    double total_cycles = 0.0;

    // precomputation
    for(int ii = 0; ii < dim/2; ii++){
//        fixed_sigma = 0.62 + (0.8768124086713189 - 0.62) * (ii * 1.0) / (dim/2);
        fixed_sigma = 1.2 + (1.697056274847714 - 1.2) * (ii * 1.0) / (dim/2);
        b.f = 1./fixed_sigma;
        isigma_arr[ii] = b.n;
        r2 = fpr_expm(fpr_mul(fpr_twopipi, fpr_sqr(fpr_inv(b.n))));
        deno_arr[ii] = fpr_div(fpr_two, fpr_add(fpr_add(r2, fpr_two), fpr_inv(r2)));

    }

    for(int ii = 0; ii < iteration; ii++){
        res = 1.0;
        // cycle counts for 1024 times
        for(int jj = 0; jj < dim / 2; jj++){
            for(int kk = 0; kk < 4; kk++){
                // randomly generate the center belong to [0,1)
                fixed_center = (prng_get_u64(&sc.p) * 1.0) / (4294967296 * 4294967296);
                a.f = fixed_center;
                fpr trans_center = a.n;
                begin = cpucycles();
                c.n = ThetaDiv(trans_center, deno_arr[jj]);
                res = res * c.f;
                end = cpucycles();
                tt = end - begin;
                total_cycles += (double)tt;
            }

        }
//        printf("res=%.20f\n", res);
    }

    // average cycles
    printf("The average cycles for ThetaDiv: %.6f\n", total_cycles / iteration);

    return 0;
}
