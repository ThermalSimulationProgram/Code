#ifndef TESTLIB_H
#define TESTLIB_H 


#include "stress-ng.h"
#include <complex.h>

#define LOOPFACTOR (1.2)

/*
 *  the CPU stress test has different classes of cpu stressor
 */
typedef void (*stress_cpu_func)(const char *name);

typedef struct {
	const char		*name;	// human readable form of stressor 
	const stress_cpu_func	func;	/* the stressor function */
} stress_cpu_stressor_info_t;

int pr_msg(
	FILE *fp,
	const uint64_t flag,
	const char *const fmt, ...);

/*
 *  stress_cpu_queens
 *	solve the queens problem for sizes 1..12
 */
void stress_cpu_queens(const char *name);


static uint32_t queens_try(
	uint32_t left_diag,
	uint32_t cols,
	uint32_t right_diag,
	uint32_t all);


/*
 *  stress_cpu_sqrt()
 *	stress CPU on square roots
 */
void HOT stress_cpu_sqrt(const char *name);

/*
 *  stress_cpu_loop()
 *	simple CPU busy loop
 */
void stress_cpu_loop(const char *name);

/*
 *  stress_cpu_gcd()
 *	compute Greatest Common Divisor
 */
void HOT OPTIMIZE3 stress_cpu_gcd(const char *name);

/*
 *  stress_cpu_bitops()
 *	various bit manipulation hacks from bithacks
 *	https://graphics.stanford.edu/~seander/bithacks.html
 */
void HOT OPTIMIZE3 stress_cpu_bitops(const char *name);

/*
 *  stress_cpu_trig()
 *	simple sin, cos trig functions
 */
void HOT stress_cpu_trig(const char *name);


/*
 *  stress_cpu_hyperbolic()
 *	simple hyperbolic sinh, cosh functions
 */
void HOT stress_cpu_hyperbolic(const char *name);

/*
 *  stress_cpu_rand48()
 *	generate random values using rand48 family of functions
 */
void HOT OPTIMIZE3 stress_cpu_rand48(const char *name);

/*
 *  stress_cpu_nsqrt()
 *	iterative Newton–Raphson square root
 */
void HOT OPTIMIZE3 stress_cpu_nsqrt(const char *name);

/*
 *  stress_cpu_phi()
 *	compute the Golden Ratio
 */
void HOT OPTIMIZE3 stress_cpu_phi(const char *name);

/*
 *  fft_partial()
 *  	partial Fast Fourier Transform
 */
void HOT OPTIMIZE3 fft_partial(
	std::complex<double> *data,
	std::complex<double> *tmp,
	const int n,
	const int m);


/*
 *  stress_cpu_fft()
 *	Fast Fourier Transform
 */
void HOT stress_cpu_fft(const char *name);

/*
 *   stress_cpu_euler()
 *	compute e using series
 */
void HOT OPTIMIZE3 stress_cpu_euler(const char *name);

/*
 *  random_buffer()
 *	fill a uint8_t buffer full of random data
 *	buffer *must* be multiple of 4 bytes in size
 */
void random_buffer(uint8_t *data, const size_t len);

/*
 *  stress_cpu_hash_generic()
 *	stress test generic string hash function
 */
void stress_cpu_hash_generic(
	const char *name,
	const char *hash_name,
	uint32_t (*hash_func)(const char *str),
	const uint32_t result);

/*
 *  jenkin()
 *	Jenkin's hash on random data
 *	http://www.burtleburtle.net/bob/hash/doobs.html
 */
uint32_t HOT OPTIMIZE3 jenkin(const uint8_t *data, const size_t len);

/*
 *  stress_cpu_jenkin()
 *	multiple iterations on jenkin hash
 */
void stress_cpu_jenkin(const char *name);

/*
 *  pjw()
 *	Hash a string, from Aho, Sethi, Ullman, Compiling Techniques.
 */
uint32_t HOT OPTIMIZE3 pjw(const char *str);

/*
 *  stress_cpu_pjw()
 *	stress test hash pjw
 */
void stress_cpu_pjw(const char *name);

/*
 *  djb2a()
 *	Hash a string, from Dan Bernstein comp.lang.c (xor version)
 */
uint32_t HOT OPTIMIZE3 djb2a(const char *str);

/*
 *  stress_cpu_djb2a()
 *	stress test hash djb2a
 */
void stress_cpu_djb2a(const char *name);

/*
 *  fnv1a()
 *	Hash a string, using the improved 32 bit FNV-1a hash
 */
uint32_t HOT OPTIMIZE3 fnv1a(const char *str);


/*
 *  stress_cpu_fnv1a()
 *	stress test hash fnv1a
 */
void HOT stress_cpu_fnv1a(const char *name);

/*
 *  sdbm()
 *	Hash a string, using the sdbm data base hash and also
 *	apparently used in GNU awk.
 */
uint32_t OPTIMIZE3 sdbm(const char *str);

/*
 *  stress_cpu_sdbm()
 *	stress test hash sdbm
 */
void stress_cpu_sdbm(const char *name);

/*
 *  stress_cpu_idct()
 *	compute 8x8 Inverse Discrete Cosine Transform
 */
void HOT OPTIMIZE3 stress_cpu_idct(const char *name);

#define int_ops(a, b, c1, c2, c3)	\
	do {				\
		a += b;			\
		b ^= a;			\
		a >>= 1;		\
		b <<= 2;		\
		b -= a;			\
		a ^= ~0;		\
		b ^= ~(c1);		\
		a *= 3;			\
		b *= 7;			\
		a += 2;			\
		b -= 3;			\
		a /= 77;		\
		b /= 3;			\
		a <<= 1;		\
		b <<= 2;		\
		a |= 1;			\
		b |= 3;			\
		a *= mwc32();		\
		b ^= mwc32();		\
		a += mwc32();		\
		b -= mwc32();		\
		a /= 7;			\
		b /= 9;			\
		a |= (c2);		\
		b &= (c3);		\
	} while (0);

#define C1 	(0xf0f0f0f0f0f0f0f0ULL)
#define C2	(0x1000100010001000ULL)
#define C3	(0xffeffffefebefffeULL)



template<typename T>
void HOT OPTIMIZE3 stress_cpu_int(T _a, T _b, T _c1, T _c2, T _c3){
	const T mask = ~0;					
	const T a_final = _a;				
	const T b_final = _b;				
	const T c1 = _c1 & mask;				
	const T c2 = _c2 & mask;				
	const T c3 = _c3 & mask;				
	register T a, b;					
	int i;							
								
	MWC_SEED();						
	a = mwc32();						
	b = mwc32();						
								
	for (i = 0; i < 1000; i++) {				
		int_ops(a, b, c1, c2, c3)		
	}							
							
}
template<typename T>
void float_ops(T&  a, T&  b, T&  c, T&  d){
	a = a + b;				
	b = a * c;				
	c = a - b;				
	d = a / b;				
	a = c / (T)0.1923;			
	b = c + a;				
	c = b * (T)3.12;			
	d = d + b + (T)sin(a);		
	a = (b + c) / c;			
	b = b * c;				
	c = c + (T)1.0;			
	d = d - (T)sin(c);			
	a = a * (T)cos(b);			
	b = b + (T)cos(c);			
	c = (T)sin(a) / (T)2.344;	
	b = d - (T)1.0;
}

template<typename T>
void stress_cpu_fp (char* name){
	int i;						
	T a = 0.18728, b = mwc32(), c = mwc32(), d;	
							
	(void)name;					
							
	for (i = 0; i < 1000; i++) {			
		float_ops<T>( a, b, c, d);			
	}						
	double_put(a + b + c + d);
}
	
void HOT OPTIMIZE3 stress_cpu_matrix_prod(const char *name);

/*
 *   stress_cpu_fibonacci()
 *	compute fibonacci series
 */
void HOT stress_cpu_fibonacci(const char *name);

/*
 *  stress_cpu_psi
 *	compute the constant psi,
 * 	the reciprocal Fibonacci constant
 */
void HOT OPTIMIZE3 stress_cpu_psi(const char *name);


/*
 *   stress_cpu_ln2
 *	compute ln(2) using series
 */
void HOT OPTIMIZE3 stress_cpu_ln2(const char *name);

/*
 *  ackermann()
 *	a naive/simple implementation of the ackermann function
 */
uint32_t HOT ackermann(const uint32_t m, const uint32_t n);

/*
 *   stress_cpu_ackermann
 *	compute ackermann function
 */
void stress_cpu_ackermann(const char *name);

/*
 *   stress_cpu_explog
 *	compute exp(log(n))
 */
void HOT stress_cpu_explog(const char *name);

/*
 *  Undocumented gcc-ism, force -O0 optimisation
 */
#if __GNUC__ && NEED_GNUC(4, 4, 0) && !defined(__clang__)
void stress_cpu_jmp(const char *name)  __attribute__((optimize("-O0")));
#else
void stress_cpu_jmp(const char *name);
#endif


/*
 *  ccitt_crc16()
 *	perform naive CCITT CRC16
 */
uint16_t HOT OPTIMIZE3 ccitt_crc16(const uint8_t *data, size_t n);

/*
 *   stress_cpu_crc16
 *	compute 1024 rounds of CCITT CRC16
 */
void stress_cpu_crc16(const char *name);

/*
 * stress_cpu_correlate()
 *
 *  Introduction to Signal Processing,
 *  Prentice-Hall, 1995, ISBN: 0-13-209172-0.
 */
void HOT OPTIMIZE3 stress_cpu_correlate(const char *name);

/*
 *  is_prime()
 *	return true if n is prime
 *	http://en.wikipedia.org/wiki/Primality_test
 */
static inline HOT OPTIMIZE3 bool is_prime(uint32_t n)
{
	register uint32_t i, max;

	if (n <= 3)
		return n >= 2;
	if ((n % 2 == 0) || (n % 3 == 0))
		return false;
	max = sqrt(n) + 1;
	for (i = 5; i < max; i+= 6)
		if ((n % i == 0) || (n % (i + 2) == 0))
			return false;
	return true;
}

/*
 *  stress_cpu_prime()
 *
 */
void stress_cpu_prime(const char *name);

/*
 *  factorial()
 *	compute n!
 */
long double HOT OPTIMIZE3 factorial(int n);

/*
 *  stress_cpu_pi()
 *	compute pi using the Srinivasa Ramanujan
 *	fast convergence algorithm
 */
void HOT OPTIMIZE3 stress_cpu_pi(const char *name);

/*
 *  stress_cpu_omega()
 *	compute the constant omega
 *	See http://en.wikipedia.org/wiki/Omega_constant
 */
void HOT OPTIMIZE3 stress_cpu_omega(const char *name);

/*
 *  hamming84()
 *	compute Hamming (8,4) codes
 */
uint8_t HOT OPTIMIZE3 hamming84(const uint8_t nybble);

/*
 *  stress_cpu_hamming()
 *	compute hamming code on 65536 x 4 nybbles
 */
void HOT OPTIMIZE3 stress_cpu_hamming(const char *name);

/*
 *  stress_cpu_union
 *	perform bit field operations on a packed union
 */
void stress_cpu_union(const char *name);

/*
 *  stress_cpu_all()
 *	iterate over all cpu stressors
 */
HOT OPTIMIZE3 void stress_cpu_all(const char *name);

/*
 * Table of cpu stress methods
 */
const stress_cpu_stressor_info_t cpu_methods[100] = {
	{ "all",		stress_cpu_all },	/* Special "all test */

	{ "ackermann",		stress_cpu_ackermann },
	{ "bitops",		stress_cpu_bitops },
	{ "correlate",		stress_cpu_correlate },
	{ "crc16",		stress_cpu_crc16 },
	{ "djb2a",		stress_cpu_djb2a },
	{ "euler",		stress_cpu_euler },
	{ "explog",		stress_cpu_explog },
	{ "fft",		stress_cpu_fft },
	{ "fibonacci",		stress_cpu_fibonacci },
	{ "fnv1a",		stress_cpu_fnv1a },
	{ "gcd",		stress_cpu_gcd },
	{ "hamming",		stress_cpu_hamming },
	{ "hyperbolic",		stress_cpu_hyperbolic },
	{ "idct",		stress_cpu_idct },
	{ "jenkin",		stress_cpu_jenkin },
	{ "jmp",		stress_cpu_jmp },
	{ "ln2",		stress_cpu_ln2 },
	{ "loop",		stress_cpu_loop },
	{ "matrixprod",		stress_cpu_matrix_prod },
	{ "nsqrt",		stress_cpu_nsqrt },
	{ "omega",		stress_cpu_omega },
	{ "phi",		stress_cpu_phi },
	{ "pi",			stress_cpu_pi },
	{ "pjw",		stress_cpu_pjw },
	{ "prime",		stress_cpu_prime },
	{ "psi",		stress_cpu_psi },
	{ "queens",		stress_cpu_queens },
	{ "rand48",		stress_cpu_rand48 },
	{ "sdbm",		stress_cpu_sdbm },
	{ "sqrt", 		stress_cpu_sqrt },
	{ "trig",		stress_cpu_trig },
	{ "union",		stress_cpu_union },
	{ NULL,			NULL }
};



#endif