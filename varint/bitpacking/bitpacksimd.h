/**
 * This is code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov
 */

#ifndef BITPACKSIMD_H
#define BITPACKSIMD_H

#include "common.h"
#include <immintrin.h>

typedef unsigned __v4su __attribute__ ((__vector_size__ (16)));

void SIMD_fastunpack_32(const __m128i *  __restrict__ in, uint32_t *  __restrict__  out, const uint32_t bit);
void SIMD_fastpackwithoutmask_32(const uint32_t *  __restrict__ in, __m128i *  __restrict__  out, const uint32_t bit);
void SIMD_fastpack_32(const uint32_t *  __restrict__ in, __m128i *  __restrict__  out, const uint32_t bit);

using namespace std;

void SIMD_nullunpacker32(const __m128i *  __restrict__ , uint32_t *  __restrict__  out);
  
  
void __SIMD_fastpackwithoutmask1_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out) ;


  
void __SIMD_fastpack1_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


  
void __SIMD_fastpackwithoutmask2_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


  
void __SIMD_fastpack2_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);

  
void __SIMD_fastpackwithoutmask3_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);

  
void __SIMD_fastpack3_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);

  
void __SIMD_fastpackwithoutmask4_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


  
void __SIMD_fastpack4_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);

  
void __SIMD_fastpackwithoutmask5_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


  
void __SIMD_fastpack5_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


  
void __SIMD_fastpackwithoutmask6_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);

  
void __SIMD_fastpack6_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);
  
void __SIMD_fastpackwithoutmask7_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);

  
void __SIMD_fastpack7_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);
  
void __SIMD_fastpackwithoutmask8_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);

  
void __SIMD_fastpack8_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);

void __SIMD_fastpackwithoutmask9_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);

  
void __SIMD_fastpack9_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);

  
void __SIMD_fastpackwithoutmask10_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);

  
void __SIMD_fastpack10_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpackwithoutmask11_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


void __SIMD_fastpack11_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpackwithoutmask12_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out) ;


void __SIMD_fastpack12_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


void __SIMD_fastpackwithoutmask13_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpack13_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpackwithoutmask14_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


void __SIMD_fastpack14_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpackwithoutmask15_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpack15_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


void __SIMD_fastpackwithoutmask16_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpack16_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpackwithoutmask17_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpack17_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpackwithoutmask18_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpack18_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


void __SIMD_fastpackwithoutmask19_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


void __SIMD_fastpack19_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpackwithoutmask20_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpack20_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


void __SIMD_fastpackwithoutmask21_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


void __SIMD_fastpack21_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


void __SIMD_fastpackwithoutmask22_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpack22_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpackwithoutmask23_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpack23_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


void __SIMD_fastpackwithoutmask24_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


void __SIMD_fastpack24_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpackwithoutmask25_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpack25_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



void __SIMD_fastpackwithoutmask26_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


void __SIMD_fastpack26_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


void __SIMD_fastpackwithoutmask27_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);

  
void __SIMD_fastpack27_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);

  
void __SIMD_fastpackwithoutmask28_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);



  
void __SIMD_fastpack28_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);

void __SIMD_fastpackwithoutmask29_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);

void __SIMD_fastpack29_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);
  
void __SIMD_fastpackwithoutmask30_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);
  
void __SIMD_fastpack30_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);


  
void __SIMD_fastpackwithoutmask31_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);
  
void __SIMD_fastpack31_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);
  
void __SIMD_fastpackwithoutmask32_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);
void __SIMD_fastpack32_32(const uint32_t *  __restrict__ _in, __m128i *  __restrict__  out);
void __SIMD_fastunpack1_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);

void __SIMD_fastunpack2_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack3_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack4_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack5_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack6_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack7_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack8_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack9_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack10_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack11_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack12_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack13_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack14_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack15_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack16_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack17_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack18_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack19_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack20_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack21_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack22_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack23_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack24_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack25_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack26_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack27_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack28_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack29_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out) ;
void __SIMD_fastunpack30_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out) ;
void __SIMD_fastunpack31_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void __SIMD_fastunpack32_32(const  __m128i*  __restrict__ in, uint32_t *  __restrict__  _out);
void SIMD_fastunpack_32(const __m128i *  __restrict__ in, uint32_t *  __restrict__  out, const uint32_t bit);
void SIMD_fastpackwithoutmask_32(const uint32_t *  __restrict__ in, __m128i *  __restrict__  out, const uint32_t bit);
void SIMD_fastpack_32(const uint32_t *  __restrict__ in, __m128i *  __restrict__  out, const uint32_t bit);
#endif
