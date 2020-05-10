/************************************************************/
/*    NAME: ruic                                              */
/*    ORGN: MIT                                             */
/*    FILE: LinSVPComm.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "LinSVPComm.h"
#include <cstdlib>
#include <string>
#include <math.h>

using namespace std;

//---------------------------------------------------------
// Constructor
LinSVPComm::LinSVPComm()

{
  navx = 0;
  navdepth = 0;
  cnavx = 0;
  cnavdepth = 0;
  zcenter = 0;
  R = 0;
  xydist = 0;
  midpointx = 0;
  midpointz = 0;
  xcenter = 0; 
  slope = 0;
  t_ang = 0;
  zmax = 0;
  newdepth = 0;
  tloss = 0;


}

//---------------------------------------------------------
// Destructor

LinSVPComm::~LinSVPComm()
{
}

//---------------------------------------------------------
// Procedure: GetSVP

double LinSVPComm::GetSVP(double z)
{
  return(c0 + ssg*z);
}

//---------------------------------------------------------
// Procedure: TLoss

double LinSVPComm::TLoss(double x1, double z1, double ang, double R, double x2, double z2)
{
  double dist = pow(pow((x2-x1),2)+pow((z2-z1),2),0.5);
  double S = R * (2*asin(dist/(2*R)));
  double ds = S/1000;
  double s_curr = 0;
  double th = ang;
  double J = 0;
  double r = x1;
  double z = z1;
  double dr = 0;
  double dz = 0;
  double P = 0;

  if(S==0){
    return(0);
  }

  while (s_curr < S){
    //cout << "r = " << r << endl;
    J = (r/sin(th))*(R*cos(ang)-R*cos(ds/R - ang)); // Try ds instead of s_curr
    P = (1/(4*PI))*pow(abs((GetSVP(z)*cos(ang))/(c0*J)),0.5);

    s_curr = s_curr + ds;
    dr = R*(sin(ang)+sin(s_curr/R-ang)) - r;
    dz = R*cos(s_curr/R-ang) - c0/ssg - z;

    th = -(atan2(dr,dz) - PI/2);
    r = r + dr;
    z = z + dz;

  }
  // cout << "th = " << Rad2Deg(th) << endl;
  // cout << "r = " << r << endl;
  // cout << "z = " << z << endl;
  // cout << "s = " << S << endl;
  // cout << "s_curr" << s_curr << endl;
  return(-20*log10(P));

}




//---------------------------------------------------------
// Procedure: OnNewMail

bool LinSVPComm::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey();
    
    if (key == "VEHICLE_NAME"){
      veh_name = msg.GetString();
    }

    if (key == "COLLABORATOR_NAME"){
      collab_name = msg.GetString();
    }

    if (key == "NAV_X"){
      navx = msg.GetDouble();
    }

    if (key == "NAV_Y"){
      navy = msg.GetDouble();
    }

    if (key == "NAV_DEPTH"){
      navdepth = msg.GetDouble();
    }

    if (key == "NEPTUNE_NAV_X"){
      cnavx = msg.GetDouble();
    }

    if (key == "NEPTUNE_NAV_Y"){
      cnavy = msg.GetDouble();
    }

    if (key == "NEPTUNE_NAV_DEPTH"){
      cnavdepth = msg.GetDouble();
    }
  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool LinSVPComm::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
// happens AppTick times per second

bool LinSVPComm::Iterate()
{
  xydist = pow(pow(cnavx - navx,2) + pow(cnavy - navy,2),0.5);
  cnavx = navx + xydist;
  cnavy = navy;

  zcenter = -c0/ssg;
  midpointx = (cnavx+navx)/2;
  midpointz = (cnavdepth+navdepth)/2;

  if((cnavdepth-navdepth)==0){
    xcenter = midpointx;
  }
  else{
    slope = -1/(((cnavdepth-navdepth)/(cnavx-navx)));
    xcenter = (zcenter-(midpointz-slope*midpointx))/slope;
  }

  R = pow(pow(xcenter - navx,2) + pow(zcenter - navdepth,2),0.5);
  // Notify("R",R);
  // Notify("Xcenter",xcenter);

  zmax = R+zcenter;

  if(zmax >= wdepth){

    if(navx<xcenter && cnavx<xcenter){
      t_ang = acos(GetSVP(navdepth)/(ssg*R));
      tloss = TLoss(navx,navdepth,t_ang,R,cnavx,cnavdepth);
      // Notify("ANG",Rad2Deg(t_ang));
      // Notify("TLOSS",tloss);
      Notify("ACOUSTIC_PATH","elev_ang="+to_string(Rad2Deg(t_ang))+", transmission_loss="+to_string(tloss)+", id=ruic@mit.edu");
    }

    else if(navx>xcenter && cnavx>xcenter){
      t_ang = acos(GetSVP(navdepth)/(ssg*R));
      tloss = TLoss(navx,navdepth,t_ang,R,cnavx,cnavdepth);
      // Notify("ANG",Rad2Deg(t_ang));
      // Notify("TLOSS",tloss);
      Notify("ACOUSTIC_PATH","elev_ang="+to_string(Rad2Deg(t_ang))+", transmission_loss="+to_string(tloss)+", id=ruic@mit.edu");
    }

    else{
      R = wdepth-zcenter-1;
      if(cnavx > xcenter){
        xcenter = cnavx-pow((pow(R,2)-pow((cnavdepth-zcenter),2)),0.5);
      }
      else if(cnavx <= xcenter){
        xcenter = cnavx+pow((pow(R,2)-pow((cnavdepth-zcenter),2)),0.5);
      }

      newdepth = zcenter+pow((pow(R,2)-pow((xcenter-navx),2)),0.5);
      t_ang = acos(GetSVP(newdepth)/(ssg*R));
      tloss = TLoss(navx,navdepth,t_ang,R,cnavx,cnavdepth);
      
      // Notify("TLOSS",tloss);
      // Notify("ANG",Rad2Deg(t_ang));
      Notify("ACOUSTIC_PATH","elev_ang="+to_string(Rad2Deg(t_ang))+", transmission_loss="+to_string(tloss)+", id=ruic@mit.edu");

      // Notify("newR",R);
      // Notify("newXcenter",xcenter);
      // Notify("newDepth",newdepth);
      if (newdepth >= 0 && newdepth < wdepth){
        Notify("CONNECTIVITY_LOCATION","x="+to_string(navx)+", y="+to_string(navy)+", z="+to_string(newdepth)+", id=ruic@mit.edu");
      }
      else{
        Notify("CONNECTIVITY_LOCATION","No new viable depth for communication, id=ruic@mit.edu");
      }
    }

    return(false);
  }

  else{
    t_ang = acos(GetSVP(navdepth)/(ssg*R));
    tloss = TLoss(navx,navdepth,t_ang,R,cnavx,cnavdepth);
    // Notify("TLOSS",tloss);
    // Notify("ANG",Rad2Deg(t_ang));
    Notify("ACOUSTIC_PATH","elev_ang="+to_string(Rad2Deg(t_ang))+", transmission_loss="+to_string(tloss)+", id=ruic@mit.edu");
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
// happens before connection is open

bool LinSVPComm::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
      if(param == "surface_sound_speed") {
        c0 = stod(value);
      }
      else if(param == "sound_speed_gradient") {
        ssg = stod(value);
      }
      else if(param == "water_depth") {
        wdepth = stod(value);
      }

    }
  }

  RegisterVariables();	

  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void LinSVPComm::RegisterVariables()
{

  Register("VEHICLE_NAME",0);
  Register("COLLABORATOR_NAME",0);
  Register("NAV_X",0);
  Register("NAV_Y",0);
  Register("NAV_DEPTH",0);
  Register("NAV_HEADING",0);
  Register("NAV_SPEED",0);
  Register("NEPTUNE_NAV_X",0);
  Register("NEPTUNE_NAV_Y",0);
  Register("NEPTUNE_NAV_DEPTH",0);
  Register("NEPTUNE_NAV_HEADING",0);
  Register("NEPTUNE_NAV_SPEED",0);

}

