/************************************************************/
/*    NAME: liumk                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle_liumk.h                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef CommunicationAngle_liumk_HEADER
#define CommunicationAngle_liumk_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class CommunicationAngle_liumk : public CMOOSApp
{
 public:
   CommunicationAngle_liumk();
   ~CommunicationAngle_liumk();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();

 private: // Configuration variables
  double m_surface_sound_speed;
  double m_sound_speed_gradient;
  double m_water_depth;
  double m_time_interval;
  double m_chord;
  double m_depth_difference;
  double m_grazing_angle;
  double m_radius;
  double m_center_chord_depth_difference;
  double m_chord_angle;
  double m_arc_length;

 private: // State variables
  std::string m_VEHICLE_NAME;
  std::string m_COLLABORATOR_NAME;
  double m_NAV_X;
  double m_NAV_Y;
  double m_NAV_DEPTH;
  double m_NAV_HEADING;
  double m_NAV_SPEED;
  double m_COLLABORATOR_NAV_X;
  double m_COLLABORATOR_NAV_Y;
  double m_COLLABORATOR_NAV_DEPTH;
  double m_COLLABORATOR_NAV_HEADING;
  double m_COLLABORATOR_NAV_SPEED;
};

#endif 
