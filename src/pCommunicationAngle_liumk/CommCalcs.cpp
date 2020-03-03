/************************************************************/
/*    NAME: Michael Liu                                  */
/*    ORGN: MIT                                  */
/*    FILE: CommCalcs.cpp                                   */
/*    DATE: 25 FEB 2020 - 2 MAR 2020                               */          
/*                                                          */
/************************************************************/

#include "CommCalcs.h" 
#include "CommunicationAngle_liumk.h"

//Calculates c(0)/g
double calcCog(double co, double g){
  double co_g=(co/g);
  return(co_g);
}

//Calculates sound speed as a function of depth (c(z))
double calcSoundSpeed(double co, double g, double z){
  double sound_spd=co+g*z;
  return(sound_spd);
}

//Calculates 2D Distance
double calc2Distance(double x1, double y1, double x2, double y2){
  double two_dist=sqrt(pow((x2-x1),2)+pow((y2-y1),2));
  return(two_dist);
}

//Calculates 3D Distance
double calc3Distance(double x1, double y1, double z1, double x2, double y2, double z2){
  double three_dist=sqrt(pow((x2-x1),2)+pow((y2-y1),2)+pow((z2-z1),2));
  return(three_dist);
}

//Calculates bearing to target
double calcBearing(double x1, double y1, double x2, double y2){
  double bearing=atan((y2-y1)/(x2-x1));
  return(bearing);
}

//Calculates the radius of a ray path (R) as a funtion of ship position
double calcRadius(double z1, double z2, double cog, double dist){
  double dist_to_center=(pow((z2+cog),2)-pow((z1+cog),2)+pow((dist),2))/(2*dist);
  double radius=sqrt(pow(dist_to_center,2)+pow((z1+cog),2));
  return(radius);
}

//Calculates arc length
double calcArcLength(double r, double dist){
  double arc_length=2*r*asin(dist/(2*r));
  return(arc_length);
}

//Calculates elevation angle (theta_0) as a function of sound speed 
double calcThetaO(double c, double g, double r){
  double theta_o=acos(c/(g*r));
  return(theta_o);
}

//Calculates grazing angle (theta) as a function of theta_0, s, and R
double calcTheta(double thetao, double s, double r){
  double theta=thetao-(s/r);
  return(theta);
}

//Calculate required water depth as a function of c(z), cog, g, and theta_0
double calcMaxDepth(double cz, double cog, double g, double ang){
  double zmax=(cz/(g*cos(ang)))-cog;
  return(zmax);
}

//Calculates r(s)
double calcRs(double r, double theta, double s){
  double r_s=r*(sin(theta)+sin((s/r)-theta));
  return(r_s);
}

//Calculates z(s)
double calcZs(double r, double thetao, double s, double cog){
  double z_s=r*cos((s/r)-thetao)-cog;
  return(z_s);
}

//Calculates J(s)
double calcJs(double cz, double g, double thetao, double s, double theta){
  double r=cz/(g*cos(thetao));
  double r1=cz/(g*cos(thetao+0.0000001));  
  double r_s=r*(sin(thetao)+sin((s/r)-thetao));
  double r_s1=r1*(sin(thetao+0.0000001)+sin((s/r1)-thetao-0.0000001));
  double j_s=(r_s/sin(theta))*((r_s1-r_s)/0.0000001);
  return(j_s);
}

//Calculate Transmission loss
double calcTransLoss(double czs, double thetao, double cz, double js){
  double ps=sqrt(abs((czs*cos(thetao))/(cz*js)));
  double tl=-20*log10(ps);
  return(tl);
}

//Produces ACOUSTIC_PATH report if path exists
string calcAcousticPath(double ang, double loss){
  stringstream ss; //creates string stream for output
  ss<<"elev_angle="<<doubleToString((-1*ang),1)<<","; //inserts elevation angle
  ss<<"transmission_loss="<<doubleToString(loss,1)<<","; //inserts transmision loss
  ss<<"id=liumk@mit.edu"; //inserts id
  string output=ss.str();
  return(output);
}

//Produces ACOUSTIC_PATH report if path does not exists
string calcAcousticNoPath(){
  stringstream ss; //creates string stream for output
  ss<<"elev_angle=NaN,"; //inserts elevation angle
  ss<<"transmission_loss=NaN,"; //inserts transmision loss
  ss<<"id=liumk@mit.edu"; //inserts id
  string output=ss.str();
  return(output);
}

//Produces CONNECTIVITY_LOCATION report if path exists
string calcConnectivityLocation(double xx, double yy, double zz){
  stringstream ss; //creates string stream for output
  ss<<"x="<<doubleToString(xx,3)<<","; //inserts x coordinate
  ss<<"y="<<doubleToString(yy,3)<<","; //inserts y coordinate
  ss<<"depth="<<doubleToString(zz,1)<<","; //inserts depth 
  ss<<"id=liumk@mit.edu"; //inserts id
  string output=ss.str();
  return(output);
}

//Converts deg to rad
double calcDeg2Rad(double deg){
  double rad=deg*M_PI/180;
  return(rad);
}

//Converts rad to deg
double calcRad2Deg(double rad){
  double deg=rad*180*M_1_PI;
  return(deg);
}
