#ifndef _STATISTICS_H
#define _STATISTICS_H

#include "MissedDeadline.h"
#include "Runtime.h"
#include "Trace.h"
#include "Enumerations.h"

#include <algorithm>
#include <semaphore.h>
#include <time.h>
#include <vector>

#define N_THREADS   100
#define N_TRACES    50000
#define N_DEADLINES 1000

/***************************************
 *        CLASS DECLARATION            * 
 ***************************************/

///This structure/function helps sort the runtime vector by ID
struct compareId {
  bool operator()(Runtime a, Runtime b) {
    if (a.getId() < b.getId() ) {
      return true;
    }
    else {
      return false;
    }
  }
};

class Statistics {

 private:
  static struct timespec tstart;
  static unsigned long tstart_us;
  static unsigned long tstart_ms;

  /*********** VARIABLES ***********/

  /**** VECTORS ****/


  ///This vector holds all of the missed deadlines
  static std::vector<MissedDeadline> missedDeadlines;

  ///This vector holds all of the simulation's runtime statistics
  static std::vector<Runtime> runtimes;

  ///This vector holds alll of the time traces
  static std::vector<Trace> traces;

  //This vector holds the thread id's of all workers
  static std::vector<unsigned int> workerId;

  ///This variable holds the state. When active, calling the ``add'' functions will add to statistics vector. Otherwise, traces will not be saved
  static int state;

  /**** SEMAPHORES ****/
  ///Semaphore to protect writing to the runtime vector
  static sem_t runtime_sem;

  ///Semaphore to protect writing to the trace vector
  static sem_t trace_sem;

  ///Semaphore to protect writing to the missedDealine vector
  static sem_t deadline_sem;

 public:
 
  /*********** MEMBER FUNCTIONS ***********/
  static void start();

  static unsigned long getRelativeTime();
  static unsigned long getRelativeTime_ms();
  
  static struct timespec getStart();
  
  ///This function adds a missed deadline trace to the vector
  static void addMissedDeadline(unsigned int t_id, unsigned long arrival_time, unsigned long deadline, 
    unsigned long finisht_time);

  static std::vector<std::string> getAllMissedDeadline();

  ///This function adds a runtime statistic to the vector
  static void addRuntime(enum _thread_type type, unsigned int t_id, struct timespec rt);

  ///This function adds a trace to the vector
  static void addTrace(enum _thread_type type, unsigned int t_id, enum _task_action act);

  ///This function should be called by the Worker constructor to 'register' its id
  static void addWorkerId(unsigned int id);

  ///This function enables the collection of statistics
  static void enable();

  ///This function disable the collection of statistics
  static void disable();

  ///This function initiates all of the variables
  static void initialize();

  ///This function saves all results to a custom csv file
  static void toFile(const std::string& filePrefix);
  

};
  
#endif
