/************************************************************/
/*    NAME: Michael Liu                                     */
/*    ORGN: MIT                                             */
/*    FILE: CommCalcs.h                                     */
/*    DATE: 25 FEB 2020 - 2 MAR 2020                        */
/*                                                          */
/************************************************************/

#define _USE_MATH_DEFINES

#include "MOOS/libMOOS/MOOSLib.h"
//#include "/home/liumk/moos-ivp/MOOS/MOOSCore/Core/libMOOS/include/MOOS/libMOOS/MOOSLib.h"
#include <string>
#include <math.h> 
#include <cstdint>
#include <iostream>
#include <list>
#include <sstream>

using namespace std;


//Calculates c(0)/g
double calcCog(double co, double g);

//Calculates sound speed as a function of depth (c(z))
double calcSoundSpeed(double co, double g, double z);

//Calculates 2D Distance
double calc2Distance(double x1, double y1, double x2, double y2);

//Calculates 3D Distance
double calc3Distance(double x1, double y1, double z1, double x2, double y2, double z2);

//Calculates bearing to target
double calcBearing(double x1, double y1, double x2, double y2);

//Calculates the radius of a ray path (R) as a funtion of ship position
double calcRadius(double z1, double z2, double cog, double dist);

//Calculates arc length (s)
double calcArcLength(double r, double dist);

//Calculates elevation angle (theta_0) as a function of sound speed 
double calcThetaO(double c, double g, double r);

//Calculates grazing angle (theta) as a function of theta_0, s, and R
double calcTheta(double thetao, double s, double r);

//Calculate required water depth as a function of c(z), c(0), g, and theta_0
double calcMaxDepth(double cz, double co, double g, double ang);

//Calculates r(s)
double calcRs(double r, double theta, double s);

//Calculates z(s)
double calcZs(double r, double thetao, double s, double cog);

//Calculates J(s)
double calcJs(double cz, double g, double thetao, double s, double theta);

//Calculate Transmission loss
double calcTransLoss(double czs, double thetao, double cz, double js);

//Produces ACOUSTIC_PATH report if path exists
string calcAcousticPath(double ang, double loss);

//Produces ACOUSTIC_PATH report if path does not exists
string calcAcousticNoPath();

//Produces CONNECTIVITY_LOCATION report if path exists
string calcConnectivityLocation(double x, double y, double z);

//Converts deg to rad
double calcDeg2Rad(double deg);

//Converts rad to deg
double calcRad2Deg(double rad);
