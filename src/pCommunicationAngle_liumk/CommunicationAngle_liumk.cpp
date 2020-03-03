/************************************************************/
/*    NAME: liumk                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle_liumk.cpp                           */
/*    DATE: 25-28  FEB 2020                                                */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "CommunicationAngle_liumk.h"
#include <sstream>
using namespace std;

//---------------------------------------------------------
// Constructor

CommunicationAngle_liumk::CommunicationAngle_liumk()
{
  m_nav_x = false;
  m_nav_y = false;
  m_nav_depth = false;
  m_collab_nav_x = false;
  m_collab_nav_y = false;
  m_collab_nav_depth = false;
}

//---------------------------------------------------------
// Destructor

CommunicationAngle_liumk::~CommunicationAngle_liumk()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool CommunicationAngle_liumk::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key = msg.GetKey();
    double dval = msg.GetDouble();
    string sval = msg.GetString();

    if (key == "NAV_X"){
      m_x_src = dval;
      m_nav_x = true;
    }

    else if (key == "NAV_Y"){
      m_y_src = dval;
      m_nav_y = true;
    }

    else if (key == "NAV_DEPTH"){
      m_z_src = dval;
      m_nav_depth = true;
    }

    // Parse messages with "VEHICLE_NAME"
    if (key=="VEHICLE_NAME"){
      m_own_name=msg.GetString();
    }
    
    // Parse messages with "COLLABORATOR_NAME"
    if (key=="COLLABORATOR_NAME"){
      m_collab_name=msg.GetString();

      //Make collaborator nav data names
      m_c_name_nav_x=m_collab_name + "_NAV_X";
      m_c_name_nav_y=m_collab_name + "_NAV_Y";
      m_c_name_nav_depth=m_collab_name + "_NAV_DEPTH";
      //m_c_name_nav_heading=m_collab_name + "_NAV_HEADING";
      //m_c_name_nav_speed=m_collab_name + "_NAV_SPEED";
    }

    else if (key == " m_c_name_nav_x"){
      m_x_rec = dval;
      m_collab_nav_x = true;
    }

    else if (key == " m_c_name_nav_y"){
      m_y_rec = dval;
      m_collab_nav_y = true;
    }

    else if (key == " m_c_name_nav_depth"){
      m_z_rec = dval;
      m_collab_nav_depth = true;
    }

#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
  }
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CommunicationAngle_liumk::OnConnectToServer()
{
  // Register for variables here.
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CommunicationAngle_liumk::Iterate()
{
  // ACOUSTIC_PROFILE

  if (m_nav_x && m_nav_y && m_nav_depth && m_collab_nav_x && m_collab_nav_y && m_collab_nav_depth){
    //Calculate r_src, r_rec
    m_r_src = 0.0;
    m_r_rec = m_acoustic_path.calcProj_r(m_x_rec,m_y_rec,m_x_src,m_y_src);
    m_r_theta = m_acoustic_path.calcProj_theta(m_x_rec,m_y_rec, m_x_src, m_y_src);

   //Find circle equation, given source and receiver position
    m_midpt_r=m_acoustic_path.calcMidpt(m_r_src,m_r_rec);
    m_midpt_z=m_acoustic_path.calcMidpt(m_z_src,m_z_rec);
    m_int_slope =m_acoustic_path.calcPerpSlope(m_r_src,m_z_src, m_r_rec, m_z_rec);
    m_int_b=m_acoustic_path.calcPerpIntercept(m_int_slope, m_midpt_r,m_midpt_z);
    //m_circ_z_center=m_acoustic_path.calcCircCenter_z();
    m_circ_z_center = - 1480/0.016;
    m_circ_r_center = m_acoustic_path.calcCircCenter_r(m_circ_z_center, m_int_slope, m_int_b);
    //cout << m_circ_z_center << "m_circ_z_center" << endl;
    //cout << m_int_slope << "m_int_slope" << endl;
    m_R_bisect = m_acoustic_path.calcRBisect(m_r_src,m_z_src,m_circ_r_center,m_circ_z_center);
  
    //Check if valid R.   
    m_valid_R=m_acoustic_path.checkValidR(m_R_bisect);


    if (m_valid_R){

      // I had issues with reading between files, so I had to bypass some calculations by writing the code in directly.
      
      //Calculate steering angle
      //m_theta_src = m_acoustic_path.calcThetaSrc(m_R_bisect,m_z_src);
      m_theta_src =  -acos((1480+0.016*m_z_src)/(m_R_bisect*0.016));
      cout << "m_R_bisect" << m_R_bisect << endl;
      cout << "m_theta_src" << m_theta_src << endl;
      m_theta_src_deg = m_acoustic_path.convertRad2Degrees(m_theta_src);
      cout << "m_theta_src_deg" << m_theta_src_deg << endl;

      // Calculate TL
      m_d_theta = 0.0000001;
      //m_TL = m_acoustic_path.calcTransmissionLoss(m_theta_src,m_z_src,m_z_rec,m_R_bisect,m_d_theta);
      double c_rec = 1480+0.016*m_z_rec;
      double c_src = 1480+0.016*m_z_src;
      double theta_rec =- acos((c_rec/c_src)*cos(m_theta_src));
      double s =(m_R_bisect*(m_theta_src-theta_rec));
      double r_i = m_R_bisect*(sin(m_theta_src)+sin(s/m_R_bisect-m_theta_src));
      double r_i1 =  m_R_bisect*(sin(m_theta_src+m_d_theta)+sin(s/m_R_bisect-m_theta_src-m_d_theta));
      double J = ((r_i)/sin(theta_rec))*((r_i1-r_i)/m_d_theta);
      double P_of_s = sqrt(abs(((0.016*(m_R_bisect*cos(theta_rec)))*cos(m_theta_src))/(c_src*J)));
      double P_1 = 1/(4*M_PI);
      double m_TL = -20*log10(P_of_s);
      cout << "m_z_src" << m_z_src << endl;
      cout << "m_z_rec" << m_z_rec << endl;
      cout << "m_R_bisect" << m_R_bisect << endl;
      cout << "m_d_theta" << m_d_theta << endl;
      cout << "m_TL" << m_TL << endl;

      // Notify angle and TL with ACOUSTIC_PATH
      stringstream ss;
      ss << "elev_angle =" << m_theta_src_deg << ", transmission loss:" << m_TL << ", id=liumk@mit.edu";
      Notify("ACOUSTIC_PATH",ss.str());

      // Elevation angle
      // Not sure why ELEV_ANGLE continues to read "0" in the ELEV_ANGLE and ELEV_ANGLE_REF comparison.
      // System is calculating and outputing correctly for ACOUSTIC_PATH and CONNECTIVITY_LOCATION.
      stringstream elev_angle;
      elev_angle << m_theta_src_deg;
      Notify("ELEV_ANGLE",elev_angle.str());

      // Notify position
      stringstream position;
      position  << "x=" << m_x_src << ",y=" << m_y_src << ",depth=" << m_z_src << ",id=liumk@mit.edu";
      Notify("CONNECTIVITY_LOCATION",position.str());
    }
    else{
      //Calc new R and cirlce (r) center
      m_R_new = m_acoustic_path.calcValidR(m_R_bisect);
      m_circ_r_center_new = m_acoustic_path.calcNewCircCenter_r(m_z_rec,m_r_rec,m_R_new,m_circ_z_center);
      // Recalculate position, keep depth (z_src)
      m_r_src_new = m_acoustic_path.calcPosOnCirc_r(m_circ_z_center,m_circ_r_center_new,m_z_src,m_R_new);
      m_x_new = m_r_src_new * cos(m_r_theta);
      m_y_new = m_r_src_new * sin(m_r_theta);

      //Notify new position
      stringstream newposition;
      newposition << "x=" << m_x_new << ",y=" << m_y_new << ",depth=" << m_z_src << ",id=liumk@mit.edu";
      Notify("CONNECTIVITY_LOCATION",newposition.str());
      Notify("ACOUSTIC_PATH","NaN");
      // x = xxx.xxx, y = yyy.yyy, depth = ddd.d, id=user@mit.edu

      // Elevation angle
      stringstream elev_angle;
      elev_angle << m_theta_src_deg;
      Notify("ELEV_ANGLE",elev_angle.str());
      }
    }
    else {
    }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool CommunicationAngle_liumk::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;

      // Convert value to double
      stringstream temp(value);
      double fvalue = 0;
      temp >> fvalue;

      if(param == "SURFACE_SOUND_SPEED") {
	m_acoustic_path.setSurfaceSoundSpeed(fvalue);
      }

      else if(param == "SOUND_SPEED_GRADIENT") {
	m_acoustic_path.setSoundSpeedGradient(fvalue);
      }
      
      else if(param == "WATER_DEPTH") {
	m_acoustic_path.setWaterDepth(fvalue);
      }

      else if(param == "TIME_INTERVAL"){
	m_acoustic_path.setTimeInterval(fvalue);
      }
      
      if(param == "foo") {
        //handled
      }
      else if(param == "bar") {
        //handled
      }
    }
  }
  RegisterVariables();	
  return(true);
}
 
//---------------------------------------------------------
// Procedure: RegisterVariables

void CommunicationAngle_liumk::RegisterVariables()
{
  // Register ("FOOBAR", 0);
  Register("VEHICLE_NAME", 0);
  Register("COLLABORATOR_NAME", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("NAV_DEPTH", 0);
  Register("NAV_HEADING", 0);
  Register("NAV_SPEED", 0);
      Register(m_c_name_nav_x,0);
      Register(m_c_name_nav_y,0);
      Register(m_c_name_nav_depth,0);
      Register(m_c_name_nav_heading,0);
      Register(m_c_name_nav_speed,0); 
}
