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
#include <sstream>
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
   bool m_even;
   long unsigned int m_ival;
   long unsigned int m_max_iter;
   int m_index;// For keeping track of number of Prime #s to factor
   int m_calc; // For keeping track of number of Prime #s calculated.
   std::string m_result_str;
   std::list<PrimeEntry> m_mail_list;
  
 private: // State variables
};

#endif 
