
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
int main(){
	int numworkers = 4;
	
	//testthread(numworkers);
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

int hogcpu (void){
	double x = 0;
unsigned int seed = 45;
  while (1){
     	//sqrt(rand_r(&seed));
	sqrt(rand());
  		/*x = x * 1.65;
  		x = x / 89;
  		x = x * x;
  		sqrt(x);*/
  }
  return 0;
}





