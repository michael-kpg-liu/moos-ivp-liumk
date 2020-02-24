/************************************************************/
/*    NAME: Michael Liu                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.h                                        */
/*    DATE: 14 FEB 2020 to 23 FEB 2020                         */
/************************************************************/

#include <string>
#include <vector>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <list>
#include <algorithm>
#include <sstream> // Library needed to convert numerical values into strings.ss
#include "/Users/liumk/moos-ivp/MOOS_Jul0519/MOOSCore/Core/libMOOS/Utils/include/MOOS/libMOOS/Utils/MOOSUtils.h"
// Library needed for MOOSTime.  Thanks to the TAs for help on this one.
// It may be helpful to explicitly cite this in the lab.

#ifndef PRIME_ENTRY_HEADER
#define PRIME_ENTRY_HEADER

class PrimeEntry
{
 public:

  PrimeEntry();
  ~PrimeEntry() {};

  void setOriginalVal(unsigned long int v) {m_orig=v;}; // Set original value
  void setReceivedIndex(unsigned long int v) {m_received_index=v;}; // Index number received
  void setCalculatedIndex(unsigned long int v) {m_calculated_index=v;}; // Index number solved
  void setDone(bool v) {m_done=v;}; // Finished status using true or false signal
  bool done() {return(m_done);}; // Return finished status
  long unsigned int iter_calc() {return(m_finished_iter);}; // Return number of iterations to calculate prime factors
  void factor(unsigned long int max_steps); //Factoring function for a certain number of steps
  std::string getReport(); // String to print out report

 protected:
  uint64_t m_start_index; // Updated number to factor
  uint64_t m_orig; // Original prime number
  // uint64_t explicitly allocates space for a 64-bit integer.
  // unsigned long int may vary depending on system version.
  bool m_done; // True or false if done with factoring
  unsigned long int m_received_index; // Index number received
  unsigned long int m_calculated_index; // Index number solved
  unsigned long int m_finished_iter; // Finished iterations.
  unsigned long int m_ii; // Number tracks iterations.
  unsigned long int m_k; // Number used to factor.
  double m_start_time; // Stores start MOOSTime
  double m_finish_time; // Stores finish MOOSTime
  double m_solve_time; // Stores difference between start and finish MOOSTime
  std::vector<unsigned long int> m_factors; //Vector of all the factors so far
};
#endif
