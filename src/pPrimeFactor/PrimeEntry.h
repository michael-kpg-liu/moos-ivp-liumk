/************************************************************/
/*    NAME: Michael Liu                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.h                                        */
/*    DATE: 14 FEB 2020 to 23 FEB 2020                         */
/************************************************************/

#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <list>
#include <algorithm>
#include <sstream>
#include "/Users/liumk/moos-ivp/MOOS_Jul0519/MOOSCore/Core/libMOOS/Utils/include/MOOS/libMOOS/Utils/MOOSUtils.h"

#ifndef PRIME_ENTRY_HEADER
#define PRIME_ENTRY_HEADER

class PrimeEntry
{
 public:

  PrimeEntry();
  ~PrimeEntry() {};

  void setOriginalVal(unsigned long int v); //Set original value
  void setReceivedIndex(unsigned int v);
  void setCalculatedIndex(unsigned int v);
  void setDone(bool v);
  bool done();
  long unsigned int iter_calc();
  void factor(unsigned long int max_steps); //Factoring function for a certain number of steps
  std::string getReport(); // String to print out report

 protected:
  unsigned long int m_N; //Updated number to factor
  unsigned long int m_orig; //Original prime number
  bool m_done; //True or false if done with factoring
  unsigned long int m_received_index;
  unsigned long int m_calculated_index;
  unsigned long int m_finished_iter;
  unsigned long int m_maxiter;
  unsigned long int m_ii;
  unsigned long int m_k_start;
  unsigned long int m_k;
  double m_start_time;
  double m_finish_time;
  double m_solve_time;
  std::vector<unsigned long int> m_factors; //Vector of all the factors so far
};
#endif
