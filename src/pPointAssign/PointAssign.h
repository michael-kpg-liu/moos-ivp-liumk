/************************************************************/
/*    NAME: Michael Liu                                              */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.h                                          */
/*    DATE: 12 MAR 2020                                        */
/************************************************************/
#ifndef PointAssign_HEADER
#define PointAssign_HEADER
#include <iostream>
#include <string>
#include "MOOS/libMOOS/MOOSLib.h"
#include <vector>
#include <sstream>
#include "XYObject.h"
#include "ColorPack.h"
#include "MBUtils.h"



class PointAssign : public CMOOSApp
{
 public:
   PointAssign();
   ~PointAssign();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();
   bool PointRegionIsEast(double x_val);
   void postViewPoint(double x, double y, std::string label, std::string color);


 private: // Configuration variables
   std::vector<std::string> m_vname_list;
   std::vector<std::string> m_visit_points;
   bool m_assign_by_region;
   std::string m_vname_str;
   bool m_reached_first_point;
   bool m_reached_last_point;
   bool m_notified_all;
 private: // State variables
};

#endif 
