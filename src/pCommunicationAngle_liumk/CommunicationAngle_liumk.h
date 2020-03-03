/************************************************************/
/*    NAME: liumk                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle_liumk.h                      */
/*    DATE: 25 FEB 2020 - 2 MAR 2020                             */
/************************************************************/
#ifndef CommunicationAngle_liumk_HEADER
#define CommunicationAngle_liumk_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "MOOS/libMOOS/MOOSLib.h"
#include "CommCalcs.h"
#include "MBUtils.h"
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <list>
#include <sstream>
#include <math.h>

using namespace std;

class CommunicationAngle_liumk : public AppCastingMOOSApp
{
 public:
   CommunicationAngle_liumk();
   ~CommunicationAngle_liumk();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

  //Enviromental parameters
  double m_surface_sound_speed;
  double m_sound_speed_gradient;
  double m_water_depth;

  //Own Ship Data
  string m_own_name;
  double m_nav_x;
  double m_nav_y;
  double m_nav_depth;
  double m_nav_heading;
  double m_nav_speed;

  //Collaborator ship key strings
  string m_collab_name;
  string m_c_name_nav_x;
  string m_c_name_nav_y;
  string m_c_name_nav_depth;
  string m_c_name_nav_heading;
  string m_c_name_nav_speed;

  //Collaborator ship data
  double m_collab_nav_x;
  double m_collab_nav_y;    
  double m_collab_nav_depth;
  double m_collab_nav_heading;
  double m_collab_nav_speed;

  //Variables used in calculations
  double m_co_g;
  double m_2d_dist;
  double m_c_z;
  double m_c_zc;
  double m_r;
  double m_s;
  double m_theta_o;
  double m_max_depth;
  double m_theta;
  double m_js;
  double m_z_s;
  double m_c_z_s;
  double m_trans_loss;
  double m_c_max;
  double m_r_max;
  double m_theta_o_max;
  double m_bearing;
  double m_x_center;
  double m_y_center;
  double m_z_center;
  double m_2d_max;
  double m_3d_max;
  double m_theta_max;
  double m_x_target;
  double m_y_target;
  double m_z_target;  

 private: // Configuration variables

 private: // State variables
};

#endif 
