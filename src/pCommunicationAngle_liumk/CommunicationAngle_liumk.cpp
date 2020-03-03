/************************************************************/
/*    NAME: liumk                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle_liumk.cpp                           */
/*    DATE: 25 FEB 2020 - 2 MAR 2020                              */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "CommunicationAngle_liumk.h"

using namespace std;

//---------------------------------------------------------
// Constructor

CommunicationAngle_liumk::CommunicationAngle_liumk()
{
  m_surface_sound_speed=1480;//meters per second
  m_sound_speed_gradient=0.016;//meters per second per meter
  m_water_depth=6000;//meters
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
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

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
      m_c_name_nav_heading=m_collab_name + "_NAV_HEADING";
      m_c_name_nav_speed=m_collab_name + "_NAV_SPEED";

      //Register for collaborator nav data
      Register(m_c_name_nav_x,0);
      Register(m_c_name_nav_y,0);
      Register(m_c_name_nav_depth,0);
      Register(m_c_name_nav_heading,0);
      Register(m_c_name_nav_speed,0); 
    }
    
    // Parse messages with "NAV_X"
    if (key=="NAV_X"){
      m_nav_x=msg.GetDouble();
    }
    
    // Parse messages with "NAV_Y"
    if (key=="NAV_Y"){
      m_nav_y=msg.GetDouble();
    }
    
    // Parse messages with "NAV_DEPTH"
    if (key=="NAV_DEPTH"){
      m_nav_depth=msg.GetDouble();
    }
    
    // Parse messages with "NAV_HEADING"
    if (key=="NAV_HEADING"){
      m_nav_heading=msg.GetDouble();
    }
    
    // Parse messages with "NAV_SPEED"
    if (key=="NAV_SPEED"){
      m_nav_speed=msg.GetDouble();
    }
    
    // Parse messages with 'collaborator' "_NAV_X"
    if (key==m_c_name_nav_x){
      m_collab_nav_x=msg.GetDouble();
    }
    
    // Parse messages with 'collaborator' "_NAV_Y"
    if (key==m_c_name_nav_y){
      m_collab_nav_y=msg.GetDouble();    
    }
    
    // Parse messages with 'collaborator' "_NAV_DEPTH"
    if (key==m_c_name_nav_depth){
      m_collab_nav_depth=msg.GetDouble();
    }
    
    // Parse messages with 'collaborator' "_NAV_HEADING"
    if (key==m_c_name_nav_heading){
      m_collab_nav_heading=msg.GetDouble();
    }
    
    // Parse messages with 'collaborator' "_NAV_SPEED" 
    if (key==m_c_name_nav_speed){
      m_collab_nav_speed=msg.GetDouble();
    }

    
#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

     if(key == "FOO") 
       cout << "great!";

     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CommunicationAngle_liumk::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CommunicationAngle_liumk::Iterate()
{
  AppCastingMOOSApp::Iterate();

  //ACOUSTIC_PATH calculations
  
  //Find elevation angle assuming adequate water depth
  
  //Find c(0)/g
  m_co_g=calcCog(m_surface_sound_speed,m_sound_speed_gradient);
  //cout<<"m_co_g="<<m_co_g<<endl;
  
  //Find distance between vessels
  m_2d_dist=calc2Distance(m_nav_x,m_nav_y,m_collab_nav_x,m_collab_nav_y);
  //cout<<"m_2d_dist="<<m_2d_dist<<endl;
  
  //Find sound speed at own ship
  m_c_z=calcSoundSpeed(m_surface_sound_speed,m_sound_speed_gradient,m_nav_depth);
  //cout<<"m_c_z="<<m_c_z<<endl;
  
  //Find sound speed at collaborator
  m_c_zc=calcSoundSpeed(m_surface_sound_speed,m_sound_speed_gradient,m_collab_nav_depth); 
  //cout<<"m_c_zc="<<m_c_zc<<endl;
  
  //Find ray path radius
  m_r=calcRadius(m_nav_depth,m_collab_nav_depth,m_co_g,m_2d_dist);
  //cout<<"m_r="<<m_r<<endl;
  
  //Find arc length of ray path
  m_s=calcArcLength(m_r,m_2d_dist);
  //cout<<"m_s="<<m_s<<endl;
  
  //Find elevation angle
  m_theta_o=calcThetaO(m_c_z,m_sound_speed_gradient,m_r);
  //cout<<"m_theta_o="<<m_theta_o<<endl;
  
  //Find water depth required for transmission.
  m_max_depth=calcMaxDepth(m_c_z,m_co_g,m_sound_speed_gradient,m_theta_o);
  //cout<<"m_max_depth="<<m_max_depth<<endl;

  //Verify ray path is clear, calc transmission losses, then publish reports
  if(m_max_depth<m_water_depth){

    //Find theta
    m_theta=calcTheta(m_theta_o,m_s,m_r);
    
    //Find J(s)
    m_js=calcJs(m_c_z,m_sound_speed_gradient,m_theta_o,m_s,m_theta);

    //Find c(z(s))
    m_z_s=calcZs(m_r,m_theta_o,m_s,m_co_g);
    m_c_z_s=calcSoundSpeed(m_surface_sound_speed,m_sound_speed_gradient,m_z_s);
        
    //Find Transmission loss
    m_trans_loss=calcTransLoss(m_c_z_s,m_theta_o,m_c_z,m_js);
    
    //Publish reports
    Notify("ACOUSTIC_PATH",calcAcousticPath(calcRad2Deg(m_theta_o),m_trans_loss));
    Notify("CONNECTIVITY_LOCATION",calcConnectivityLocation(m_nav_x,m_nav_y,m_nav_depth));
  }

  else{
    //Publish NaN report
    Notify("ACOUSTIC_PATH",calcAcousticNoPath());
  
    //Calculate connectivity location

    //Find sound speed at max water depth
    m_c_max=calcSoundSpeed(m_surface_sound_speed,m_sound_speed_gradient,m_water_depth);

    //Find maximum possible radius of ray
    m_r_max=m_c_max/m_sound_speed_gradient;

    //Calculate elevation angle for max range from collaborator
    m_theta_o_max=acos(m_c_zc/m_c_max);
    
    //Calculate bearing to collaborator
    m_bearing=calcBearing(m_nav_x,m_nav_y,m_collab_nav_x,m_collab_nav_y);

    //Calculate center of ray path
    m_x_center=m_collab_nav_x-m_r_max*sin(m_theta_o_max)*cos(m_bearing);
    m_y_center=m_collab_nav_y-m_r_max*sin(m_theta_o_max)*sin(m_bearing);
    m_z_center=-m_co_g;

    //Find graze angle of arrival path at own ship
    m_2d_max=calc2Distance(m_nav_x,m_nav_y,m_x_center,m_y_center);
    m_3d_max=calc3Distance(m_nav_x,m_nav_y,m_nav_depth,m_x_center,m_y_center,m_z_center);
    m_theta_max=asin(m_2d_max/m_3d_max);

    //Find target coordinates
    m_x_target=m_collab_nav_x-m_r_max*(sin(m_theta_o_max)+sin(m_theta_max))*cos(m_bearing);
    m_y_target=m_collab_nav_y-m_r_max*(sin(m_theta_o_max)+sin(m_theta_max))*sin(m_bearing);
    m_z_target=m_r_max*cos(m_theta_max)-m_co_g;

    //Check to ensure z is in water then adjust or publish

    //Check z is in water
    if(m_z_target>=0){
      Notify("CONNECTIVITY_LOCATION",calcConnectivityLocation(m_x_target,m_y_target,m_z_target));
    }
    
    else{
      m_theta_max=acos(m_co_g/m_r_max);
      m_x_target=m_collab_nav_x-m_r_max*(sin(m_theta_o_max)+sin(m_theta_max))*cos(m_bearing);
      m_y_target=m_collab_nav_y-m_r_max*(sin(m_theta_o_max)+sin(m_theta_max))*sin(m_bearing);
      m_z_target=m_r_max*cos(m_theta_max)-m_co_g;
      Notify("CONNECTIVITY_LOCATION",calcConnectivityLocation(m_x_target,m_y_target,m_z_target));
    }
  }
  AppCastingMOOSApp::PostReport();
  return(true);
}
  
//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

  bool CommunicationAngle_liumk::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "foo") {
      handled = true;
    }
    else if(param == "bar") {
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void CommunicationAngle_liumk::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("VEHICLE_NAME", 0);
  Register("COLLABORATOR_NAME", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("NAV_DEPTH", 0);
  Register("NAV_HEADING", 0);
  Register("NAV_SPEED", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool CommunicationAngle_liumk::buildReport() 
{
  //  m_msgs << "============================================" << endl;
  //  m_msgs << "File:                                       " << endl;
  //  m_msgs << "============================================" << endl;

  //  ACTable actab(4);
  //  actab << "Alpha | Bravo | Charlie | Delta";
  //  actab.addHeaderLines();
  //  actab << "one" << "two" << "three" << "four";
  //  m_msgs << actab.getFormattedString();

  return(true);
}

