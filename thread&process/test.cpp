
#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

int hogcpu (void);
void testprocess(int numworkers);
void testthread(int numworkers);
void * thredcpu(void* arg);


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


void  stress_cpu_rand48()
{
	int i;
	double d = 0;
	long int l = 0;



	srand48(0x0defaced);
	// for (i = 0; i < 16384; i++) {
	while(1) {
		d += drand48();
		l += lrand48();
	}

}

int hogcpu (void){
	double x = 0;
unsigned int seed = 45;
  /*while (1){
     	//sqrt(rand_r(&seed));
	sqrt(rand());
  		
  }*/
stress_cpu_rand48();
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







