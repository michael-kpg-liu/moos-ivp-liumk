/************************************************************/
/*    NAME: ruic                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.h                                          */
/*    DATE: 03/17/2020                                                  */
/************************************************************/

#ifndef GenPath_HEADER
#define GenPath_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <vector>

class GenPath : public AppCastingMOOSApp
{
 public:
   GenPath();
   ~GenPath();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   int MinDist(double x, double y, std::vector<double> xvec, std::vector<double> yvec);
   bool buildReport();

 protected:
   void RegisterVariables();

 private: // Configuration variables
 	double startx;
 	double starty;
 	double visit_radius = 5;

 private: // State variables

	 std::vector<double> v1_vpx;
	 std::vector<double> v1_vpy;
	 std::vector<double> xloclist;
	 std::vector<double> yloclist;
	 std::vector<double> not_visited_x;
	 std::vector<double> not_visited_y;
   int pt_rec;
   int pt_planned;
   int num_vp;

 	 std::string pointmsg1;
 	 std::string xpointmsg;
 	 std::string xpointprefix;
 	 double xpoint;
 	 std::string ypointmsg;
 	 std::string ypointprefix;
 	 double ypoint;

 	 std::string xlocmsg;
 	 std::string ylocmsg;
 	 double xloc;
 	 double yloc;
 	 bool regenpath;
 	 std::string regenpathmsg;
 	 std::string outstr="Not all points visited ...";

};

#endif 
