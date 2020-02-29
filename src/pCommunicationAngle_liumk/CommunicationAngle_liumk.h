/************************************************************/
/*    NAME: liumk                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle_liumk.h                      */
/*    DATE: 25 FEB 2020                                                */
/************************************************************/

#ifndef CommunicationAngle_liumk_HEADER
#define CommunicationAngle_liumk_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "AcousticPath.h"
#include <iostream>

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
  
 private: // State variables
  AcousticPath m_acoustic_path;
  double m_x_src, m_y_src, m_r_src, m_z_src, m_x_rec, m_y_rec, m_r_rec, m_z_rec, m_r_theta, m_x_new, m_y_new;
  double m_midpt_r, m_midpt_z, m_int_slope, m_int_b, m_circ_z_center, m_circ_r_center, m_R_bisect, m_theta_src, m_theta_src_deg, m_R_new, m_r_src_new, m_circ_r_center_new;
  bool m_valid_R, m_nav_x, m_nav_y, m_nav_depth,  m_collab_nav_x, m_collab_nav_y, m_collab_nav_depth;
  // TL variables
  double m_d_theta, m_TL;
};

#endif 
