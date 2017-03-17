
#include "testlib.h"
#include "TimeUtil.h"

#include <vector>
// #include <cstdint>

int32_t opt_sequential = DEFAULT_SEQUENTIAL;	/* Number of sequential workers */
int32_t opt_all = 0;				/* Number of concurrent workers */
uint64_t opt_timeout = 0;			/* timeout in seconds */
uint64_t opt_flags = PR_ERROR | PR_INFO | OPT_FLAGS_MMAP_MADVISE;
volatile bool opt_do_run = true;		/* false to exit stressor */
volatile bool opt_do_wait = true;		/* false to exit run waiter loop */
volatile bool opt_sigint = false;		/* true if stopped by SIGINT */

mwc_t __mwc = {
	MWC_SEED_W,
	MWC_SEED_Z
};

/*
 *  mwc_reseed()
 *	dirty mwc reseed
 */
void mwc_reseed(void)
{
	if (opt_flags & OPT_FLAGS_NO_RAND_SEED) {
		__mwc.w = MWC_SEED_W;
		__mwc.z = MWC_SEED_Z;
	} else {
		struct timeval tv;
		int i, n;

		__mwc.z = 0;
		if (gettimeofday(&tv, NULL) == 0)
			__mwc.z = (uint64_t)tv.tv_sec ^ (uint64_t)tv.tv_usec;
		__mwc.z += ~((unsigned char *)&__mwc.z - (unsigned char *)&tv);
		__mwc.w = (uint64_t)getpid() ^ (uint64_t)getppid()<<12;

		n = (int)__mwc.z % 1733;
		for (i = 0; i < n; i++) {
			(void)mwc32();
		}
	}
}

const char *app_name = "stress-ng";	

int main(){
	const char* name = "testfunc";
	
	

	int stress_func_number;
	for (int i = 0; i < 100; ++i)
	{
		if(!cpu_methods[i].func){
			stress_func_number = i-1;
			break;
		}
	}

	std::vector<unsigned long> exe_times(100, 0);

	int num_loops = 20;
	for (int j = 0; j < num_loops; ++j)
	{
		unsigned long timein, timeout;
		for (int i = 0; i < stress_func_number; ++i)
		{
			timein = TimeUtil::convert_us(TimeUtil::getTime());
			stress_cpu_all(name);
			timeout = TimeUtil::convert_us(TimeUtil::getTime());
			exe_times[i] = exe_times[i] + timeout - timein;
		}
	}
	std::cout << (int) (112*1.3) << std::endl;
	unsigned long alltime = 0;
	for (int i = 0; i < (int)exe_times.size(); ++i)
	{
		alltime = alltime + exe_times[i];
	}

	for (int i = 0; i < stress_func_number; ++i)
	{
		std::cout << cpu_methods[i+1].name << "\t\t" 
		<< ((double)exe_times[i])/ num_loops << "\t\t" << 
		((double)exe_times[i])/alltime * 100 << std::endl;
	}
	
// 	stress_cpu_queens(name);
// 	stress_cpu_sqrt(name);
// 	stress_cpu_loop(name);
// 	stress_cpu_gcd(name);
// 	stress_cpu_bitops(name);
// 	stress_cpu_trig(name);

// 	stress_cpu_hyperbolic(name);


//  *  stress_cpu_rand48()
//  *	generate random values using rand48 family of functions
 
// 	stress_cpu_rand48(name);

// /*
//  *  stress_cpu_nsqrt()
//  *	iterative Newton–Raphson square root
//  */
// 	stress_cpu_nsqrt(name);

// /*
//  *  stress_cpu_phi()
//  *	compute the Golden Ratio
//  */
// 	stress_cpu_phi(name);

	
}

