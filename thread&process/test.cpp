
#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#include <random>


int hogcpu (void);
void testprocess(int numworkers);
void testthread(int numworkers);
void * thredcpu(void* arg);

int doubleRand() {
     	std::random_device rd;  //Will be used to obtain a seed for the random number engine
    	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    	std::uniform_int_distribution<> dis(1, 600000);

    	while(1){
    		dis(gen);
    	}
}


void random_r_loop(){
	struct random_data rand_states;
	int r;
	unsigned int seed = 89;
	char *  s = (char*)calloc(64, 8);
	initstate_r(seed, s, 8, &rand_states);

	while(1){
		random_r(&rand_states, &r);
	}
}



void stress_cpu_trig()
{
	 double i = 0;
	long double d_sum = 0.0;
	long double theta = 0;

	while (1) {
		i++;
		//theta = (2.0 * (double)i)/1500.0;
		d_sum += (cos(i) * sin(i));
		/*{
			d_sum += (cosl(theta) * sinl(theta));
			d_sum += (cos(theta) * sin(theta));
			d_sum += (cosf(theta) * sinf(theta));
		}
		{
			long double theta2 = theta * 2.0;

			d_sum += cosl(theta2);
			d_sum += cos(theta2);
			d_sum += cosf(theta2);
		}
		{
			long double theta3 = theta * 3.0;

			d_sum += sinl(theta3);
			d_sum += sin(theta3);
			d_sum += sinf(theta3);
		}*/
	}
}


void  stress_cpu_nsqrt()
{
	int i;
	const long double precision = 1.0e-12;
	const int max_iter = 56;

	// for (i = 0; i < 16384; i++) 
	while (1) {
		i++;
		long double n = (double)i;
		long double lo = (n < 1.0) ? n : 1.0;
		long double hi = (n < 1.0) ? 1.0 : n;
		long double rt;
		int j = 0;

		while ((j++ < max_iter) && ((hi - lo) > precision)) {
			long double g = (lo + hi) / 2.0;
			if ((g * g) > n)
				hi = g;
			else
				lo = g;
		}
		rt = (lo + hi) / 2.0;
		
	}
	
}

int hogcpu (void){
	double x = 0;
unsigned int seed = 45;
  /*while (1){
     	//sqrt(rand_r(&seed));
	sqrt(rand());
  		
  }*/
random_r_loop();
  return 0;
}

int main(){
	int numworkers = 4;
	
	// testthread(numworkers);
	testprocess(numworkers);
}

void testthread(int numworkers){
///The actual pthread variable
	pthread_t thread[numworkers];

///Auxiliary variable to set the thread's attributes
	pthread_attr_t thread_attr[numworkers];

///Auxiliary variable for the cpu affinity
  	cpu_set_t set;

	int i = 0;
	for (i = 0; i < numworkers; ++i)
	{
		CPU_ZERO (&set);
		CPU_SET (i, &set);
		pthread_attr_init(&thread_attr[i]);

//creating the pthread
		if (pthread_create(&thread[i], &thread_attr[i], thredcpu, (void*)&i)) {
			exit(-1);
		}
		pthread_setaffinity_np(thread[i], sizeof(cpu_set_t), &set);
	}

	for ( i = 0; i < numworkers; ++i)
	{
		pthread_join(thread[i], NULL);
	}
}

void testprocess(int numworkers){
	int numberprocess = numworkers;
	int pid;
	int children = 0;

	while (numberprocess){
		switch (pid = fork()){
			case 0:
				exit(hogcpu());
			case -1:
				break;
			default:
				children++;
		}
		numberprocess--;
	}

	while (children){
		int status, ret;

		if ((pid = wait(&status)) > 0){
			children--;
		}
	}
}

void * thredcpu(void* arg){
	hogcpu();
}







