/************************************************************/
/*    NAME: appcasting                                              */
/*    ORGN: MIT                                             */
/*    FILE: DiscoverFrontAngle.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef DiscoverFrontAngle_HEADER
#define DiscoverFrontAngle_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class DiscoverFrontAngle : public AppCastingMOOSApp
{
 public:
   DiscoverFrontAngle();
   ~DiscoverFrontAngle();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables
   std::string m_index;
   int m_num_passes;
 private: // State variables
};

#endif 
