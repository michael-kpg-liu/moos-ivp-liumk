/************************************************************/
/*    NAME: Michael Liu                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.h                                          */
/*    DATE: 14 FEB 2020 to 23 FEB 2020                               */
/************************************************************/

#ifndef PrimeFactor_HEADER
#define PrimeFactor_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <iostream>
#include <sstream> // Library needed to convert numerical values into strings.
#include <string>
#include <list>
#include "PrimeEntry.h"

class PrimeFactor : public CMOOSApp
{
 public:
   PrimeFactor();
   ~PrimeFactor();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();

 private: // Configuration variables
  unsigned long int m_ival; // Initial value 
  unsigned long int m_max_iter; // Number of maximum iterations (100,000 per p. 11 of Lab 5)
  int m_index; //For keeping track of prime factors calculated.
  int m_factors; // Holder of prime factors found so far.
  std::string m_result_str; // String of results
  std::list<PrimeEntry> m_mail_list; // List of parameters 
  
 private: // State variables
};

#endif 
