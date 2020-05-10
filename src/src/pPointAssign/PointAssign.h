/************************************************************/
/*    NAME: ruic                                              */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.h                                          */
/*    DATE: 03/17/2020                                                  */
/************************************************************/

#ifndef PointAssign_HEADER
#define PointAssign_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <vector>

class PointAssign : public AppCastingMOOSApp
{
 public:
   PointAssign();
   ~PointAssign();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void postViewPoint(double x, double y, std::string label, std::string color);
   bool buildReport();

 protected:
   void RegisterVariables();

 private: // Configuration variables

 	std::string veh_name;
 	bool assign_by_reg;

 private: // State variables

 	int pt_rec;
 	int pt_bc;
 	std::string ptmsg;
 	std::string pointmsg;
 	std::string xpointmsg;
 	std::string xpointprefix;
 	double xpoint;
 	std::string ypointmsg;
 	std::string ypointprefix;
 	double ypoint;
 	std::string idmsg;
 	std::string idprefix;
 	int id;

 	std::vector<std::string> msg_vec;
 	std::string v1out;
 	std::string v2out;
 	std::string out;

 	std::vector<std::string> veh_names;
 	int num_vehs;
 	std::vector<std::string> colors = {"yellow","red"};

};

#endif 
