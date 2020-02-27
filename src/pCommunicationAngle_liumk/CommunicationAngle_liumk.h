/************************************************************/
/*    NAME: liumk                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle_liumk.h                      */
/*    DATE: 25 FEB 2020                                                */
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
  std::string ACOUSTIC_PATH();
  std::string CONNECTIVITY_LOCATION();

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
  double m_sound_speed;
  double m_radius;
  double m_center_chord_depth_difference;
  double m_chord_angle;
  double m_arc_length;
  double m_r_projection;
  double m_depth_projection;
  double m_elevation_angle;
  double m_transmission_loss;
  double m_transmission_loss_i;
  double m_maximum_circular_arc_depth;
  std::string elevation_angle;
  std::string transmission_loss;
  std::string id;
  std::string acoustic_path_id;
  std::string acoustic_path;
  std::string m_x_position;
  std::string m_y_position;
  std::string m_depth_position;
  std::string m_connectivity_location_id;
  std::string m_connectivity_location;
  std::string m_new_x_position;
  std::string m_new_y_position;
  std::string m_new_depth_position;
  std::string m_new_connectivity_location_id;
  std::string m_new_connectivity_location;
  double m_neptune_sound_speed;
  double m_max_grazing_angle;
  double m_max_radius;
  double m_circle_center_depth;
  double m_revised_x;
  
 private: // State variables
  std::string m_VEHICLE_NAME;
  double m_NAV_X;
  double m_NAV_Y;
  double m_NAV_DEPTH;
  double m_NAV_HEADING;
  double m_NAV_SPEED;

  
  std::string m_COLLABORATOR_NAME;
  std::string neptune_NAV_X;
  std::string neptune_NAV_Y;
  std::string neptune_NAV_DEPTH;
  std::string neptune_NAV_HEADING;
  std::string neptune_NAV_SPEED;

  double m_neptune_NAV_X;
  double m_neptune_NAV_Y;
  double m_neptune_NAV_DEPTH;
  double m_neptune_NAV_HEADING;
  double m_neptune_NAV_SPEED;
};

#endif 
