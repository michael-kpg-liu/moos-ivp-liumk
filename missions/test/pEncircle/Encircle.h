/************************************************************/
/*    NAME: Mike Benjamin, Kyle Woerner                     */
/*    ORGN: MIT                                             */
/*    FILE: Encircle.h                                      */
/*    DATE: March 27th, 2017                                */
/************************************************************/

#ifndef ENCIRCLE_HEADER
#define ENCIRCLE_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "NodeRecord.h"

class Encircle : public AppCastingMOOSApp
{
 public:
  Encircle();
  ~Encircle() {};
  
 protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
 protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();
  
 protected:
  void checkForStaleContacts();
  bool updateTrajectoryStates();
  void registerVariables();
  bool handleMailNodeReport(std::string);
  bool handleMailNodeReportLocal(std::string);
  
 protected:

  bool   updateTrajectoryState(NodeRecord);
  double getRangeToCircle(NodeRecord);
  void   updateDirectRange(NodeRecord);
  void   updateArcDistance(NodeRecord);

  bool   updateGapState();
  bool   updateSpeed();
  bool   postNodeMessages();
  bool   postToGraph();

protected: // set utils for mail and config handling
  bool   setUpdateParameter(std::string);
  bool   setCirclePosition(std::string);
  bool   setAggression(std::string);
  bool   setConsiderThresh(std::string);
  bool   setMessageColor(std::string);
  bool   setMaxSpeedOS(std::string);
  bool   setActiveState(std::string);
  
 private: // Configuration variables
  double m_center_x;
  double m_center_y;
  double m_radius;
  
  double m_on_circle_thresh;
  double m_consider_thresh;
  
  double m_aggression;
  double m_speed_refresh_rate;

  double m_os_max_speed;
  double m_max_time;
  double m_desired_group_speed;

  double m_stale_thresh; // seconds

  std::string m_grp_filter;
  std::string m_msg_color;
  std::string m_update_parameter;

private: // State variables

  unsigned int m_total_node_reports;
  bool         m_encircle_active;

  NodeRecord   m_ownship;
  std::string  m_os_name;
  double       m_os_tstamp;
  double       m_os_stale_thresh;
    
  bool         m_os_clockwise;
  bool         m_os_ctr_clock;
  double       m_os_range_to_circle;
  double       m_os_new_speed;

  std::string  m_closest_fore_name;
  double       m_closest_fore_dist;
  double       m_closest_fore_gap_spd;

  std::string  m_closest_aft_name;
  double       m_closest_aft_dist;
  double       m_closest_aft_gap_spd;

  // Each map keyed on a contact name (no ownship)
  std::map<std::string, NodeRecord>   m_map_node_records;
  std::map<std::string, unsigned int> m_map_node_count;
  std::map<std::string, double>       m_map_node_tstamp;
  std::map<std::string, double>       m_map_node_rng_to_os;
  std::map<std::string, double>       m_map_node_rng_to_circ;
  std::map<std::string, bool>         m_map_node_on_circ;
  
  std::map<std::string, double>       m_map_node_arc_front;
  std::map<std::string, double>       m_map_node_arc_back;
  std::map<std::string, bool>         m_map_node_clockwise;
  std::map<std::string, bool>         m_map_node_ctr_clock;
};

#endif 

