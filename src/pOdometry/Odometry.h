/************************************************************/
/*    NAME: Michael Liu                                              */
/*    ORGN: MIT                                             */
/*    FILE: Odometry.h                                          */
/*    DATE: 13 FEB 2020                                              */
/************************************************************/

#ifndef Odometry_HEADER
#define Odometry_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/Appcasting/AppCastingMOOSApp.h"

class Odometry : public AppCastingMOOSApp // Replace CMOOSApp to enable AppCasting
{
 public:
   Odometry();
   ~Odometry();

 protected: // Standard MOOSApp functions to overload.
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   bool buildReport(); // Specific for pMarineViewer

 protected:
   void RegisterVariables();

 private: // Configuration variables

 private: // State variables
  bool m_first_reading;
  double m_current_x;
  double m_current_y;
  double m_previous_x;
  double m_previous_y;
  double m_total_distance;
};

#endif 
