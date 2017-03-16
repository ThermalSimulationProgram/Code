#ifndef _MISSEDDEADLINE_H
#define _MISSEDDEADLINE_H

#include "Enumerations.h"

#include <string>
#include <vector>

// using namespace std;

/***************************************
 *        CLASS DECLARATION            * 
 ***************************************/

class MissedDeadline {

 private:

  /*********** VARIABLES ***********/

  //Variables will be traced in the following order:

  ///This variable stores the id of the thread 
  unsigned int thread_id;

  ///This variable stores the job's arrival time
  struct timespec arrivalTime;

  ///This variable stores the job's deadline
  struct timespec deadline;

  ///This variable stores the job's finish time
  struct timespec finishTime;

 public:

  /*********** MEMBER FUNCTIONS ***********/

  ///This function sets the attributes of the object
  void setMissedDeadline(unsigned int ti, struct timespec at, struct timespec dl, struct timespec ft);

  ///This function converts the trace to string for file output
  std::string toString();

  /*********** GETTER FUNCTIONS ***********/

  ///This function returns the job's arrival time
  struct timespec getArrivalTime();

  ///This function returns the job's deadline
  struct timespec getDeadline();

  ///This function returns the thread ID
  unsigned int getId();

  ///This function returns the job's finish time
  struct timespec getFinishTime();
};

#endif
