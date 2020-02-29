/************************************************************/
/*    NAME: liumk                                              */
/*    ORGN: MIT                                             */
/*    FILE: AcousticPath.cpp                           */
/*    DATE: 25-28  FEB 2020                                                */
/************************************************************/

// Class for calculating acoustic path between receiver and source.
// Linear sound speed profile.
#include "AcousticPath.h"
#define PI_ML 3.14159265
#include <math.h>

// calcProj_r: Using x,y positions of src and rec.  Calculate r_rec, where r_src is at zero.
double AcousticPath::calcProj_r(double x_rec,double y_rec, double x_src, double y_src){
  double dist_0_rec = sqrt(pow(x_rec,2)+pow(y_rec,2));
  double dist_0_src = sqrt(pow(x_src,2)+pow(y_src,2));
  return (dist_0_rec-dist_0_src);
}

// calcProj_theta: Using x,y positions of src and rec.
// Calculate theta_rec to find "slice" of projected plane in r-direction.
double AcousticPath::calcProj_theta(double x_rec,double y_rec, double x_src, double y_src){
  double delta_x = x_rec-x_src;
  double delta_y = y_rec-y_src;
  return (atan(delta_y/delta_x)); 
}
// calcC(z): Calculate speed of sound, c, from depth z and speed of sound, c_0, at depth 0.
double AcousticPath::calcC(double z){
  return m_c_0+m_gradient*z;
}

// calcMidpt_r(r_1,r_2): Calculate midpoint between two points.
double AcousticPath::calcMidpt(double r_1,double r_2){
  return (r_1+r_2)/2;
}

// calcPerpSlope(r_1,z_1,r_2,z_2): Calculate perpendicular slope from two points.
double AcousticPath::calcPerpSlope(double r_1,double z_1, double r_2, double z_2){
  double slope=(z_2-z_1)/(r_2-r_1); 
  return -1/slope;
}

// calcPerpIntercept(slope, midpt_r, midpt_z): Calculate z-intercept for a given slope and r,z points.
// In this case, that's the perpendicular slope and midpt.
double AcousticPath::calcPerpIntercept(double slope, double midpt_r,double midpt_z){
  return  midpt_z-slope*midpt_r;
}

// calcCircCenter_r(z_center, int_slope, int_b): Given intercepting bisector line, find circle center
// z-coordinate of center already known.
double AcousticPath::calcCircCenter_r(double z_center, double int_slope, double int_b){
  double r_center=(z_center-int_b)/int_slope;
  return r_center;
}

// calcCircCenter_z(): Calculate circle center z-coordinate, funciton of surface sound speed and gradient
double AcousticPath::calcCircCenter_z(){
  return -m_c_0/m_gradient;
}

// calcRBisect: Calculate the radius from finding the circle with the bisection method.
double AcousticPath::calcRBisect(double r_1,double z_1,double r_center, double z_center){
  return sqrt(pow((r_center-r_1),2)+pow((z_center-z_1),2));

}

// calcThetaSrc(R,z_src): Calculate transmitter/source angle for a given radius and source depth.
double AcousticPath::calcThetaSrc(double R, double z_src){
  return -acos((calcC(z_src))/(R*m_gradient));
}
// convertRad2Degrees(angle): Convert radians to degrees.
double AcousticPath::convertRad2Degrees(double angle){
  return angle * 180 / PI_ML;
}

// checkValidR(R): Check if radius from circle is less than water depth.
bool AcousticPath::checkValidR(double R){
  if ((R-m_c_0/m_gradient) > m_water_depth){
    return false;
  }
  else{
    return true;
  }
}

// calcValidR(R_current): Calculate a valid radius by accounting for water depth.
double AcousticPath::calcValidR(double R_current){
  double R_new= R_current-(R_current-m_water_depth);
  return R_new;
}

// calcNewCircCenter_r: Recalculate circle center ,since R is invalid.
double AcousticPath::calcNewCircCenter_r(double z_rec, double r_rec, double R_new,double circ_z_center){
  double temp_term=sqrt(pow(R_new,2)-pow((z_rec-circ_z_center),2));
  double temp_r_new=-(temp_term-r_rec);
  return temp_r_new;  
  }

// calcPosOnCirc_r: Calculate r given circle center and z-pos of source.
double AcousticPath::calcPosOnCirc_r(double circ_z_center,double circ_r_center, double z_src, double R){
  double temp_term=sqrt(pow(R,2)-pow((z_src-circ_z_center),2));
  double r_pos=(temp_term+circ_r_center);
  return r_pos;
}

// calcArcLength: Calculate arc length, angle in radians.
double AcousticPath::calcArcLength(double R, double theta_src, double theta_rec){
  double s = (R*((2*PI_ML+theta_src)-theta_rec)); //Accounting for negative sign

  return s;
}

// calcThetaRec: Calculate receiving angle.
double AcousticPath::calcThetaRec(double z_src, double z_rec, double theta_src){
  double c_rec = calcC(z_rec);
  double c_src = calcC(z_src);
  return acos((c_rec/c_src)*cos(theta_src));

}

// calc_r: Calculte r from theta and arc length, r(s) = R(sin(theta_src)+sin(s/R - theta_src))
double AcousticPath::calc_r(double theta_src,double s, double R){
  //  return R*(sin(theta_src-PI_ML)+sin(s/R-theta_src-PI_ML));
  return R*(sin(theta_src)+sin(s/R-theta_src));

}

//calcJ: Calculate J(s)
double AcousticPath::calcJ(double s, double theta_src, double d_theta, double R){
  /*  double r_i=calc_r(theta_src-PI_ML,s,R);
  double r_i1=calc_r(theta_src-PI_ML+d_theta,s,R);
  return ((r_i)/sin(theta_src-PI_ML))*((r_i1-r_i)/d_theta);*/
  double r_i=calc_r(theta_src,s,R);
  double r_i1=calc_r(theta_src+d_theta,s,R);
  return ((r_i)/sin(theta_src))*((r_i1-r_i)/d_theta);
}

/*calcPfromArcLength
  Calculate pressure from arc length*/
double AcousticPath::calcPfromArcLength(double z_src, double z_rec, double J, double theta_src){
    double temp = std::abs((calcC(z_rec)*cos(theta_src))/(calcC(z_src)*J));

  return 1/(4*PI_ML)*sqrt(temp);

}

// calcTransmissionLoss: Calculate transmission loss between two points.
double AcousticPath::calcTransmissionLoss(double theta_src, double z_src, double z_rec, double R, double d_theta){
 
  double theta_rec = calcThetaRec(z_src, z_rec, theta_src);
  double s = calcArcLength(R,theta_src, theta_rec);
  double J = calcJ(s,theta_src,d_theta, R);
  double P_of_s = calcPfromArcLength(z_src, z_rec, J, theta_src);
  double P_1 = 1/(4*PI_ML);
  double TL = -20*log10(P_of_s/P_1);
  return TL;

}
