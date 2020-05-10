/************************************************************/
/*    NAME: ruic                                              */
/*    ORGN: MIT                                             */
/*    FILE: LinSVPComm.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef LinSVPComm_HEADER
#define LinSVPComm_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <cmath>
#include <vector>

class LinSVPComm : public CMOOSApp
{
 public:
   LinSVPComm();
   ~LinSVPComm();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   double	GetSVP(double z);
   double Deg2Rad(double ang){return(ang*PI/180);};
   double Rad2Deg(double ang){return(ang*180/PI);};
   double TLoss(double x1, double z1, double ang, double R, double x2, double z2);

 protected:
   void RegisterVariables();

 private: // Configuration variabless

 	double wdepth;
 	double c0;
 	double ssg;

 private: // State variables
 	std::string veh_name;
 	std::string collab_name;
 	double navx;
 	double navy;
 	double navdepth;
 	double cnavx;
 	double cnavy;
 	double cnavdepth;

 	double zcenter;
 	double R;
  double xydist;
  double midpointx;
  double midpointz;
  double xcenter; 
  double slope;
  double zmax;
  double t_ang;
  double newdepth;
  double tloss;

};

#endif 
