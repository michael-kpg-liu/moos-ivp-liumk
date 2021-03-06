/************************************************************/
/*    NAME: Michael Liu                                              */
/*    ORGN: MIT                                             */
/*    FILE: Odometry.cpp                                        */
/*    DATE: 13 FEB 2020                                                */
/************************************************************/

//pOdometry Lab #4:  Calculates the distance traveled by the UUV.

#include <iterator>
#include "MBUtils.h"
#include "Odometry.h"

using namespace std;


//---------------------------------------------------------
// Constructor

Odometry::Odometry()
{
  m_first_reading = true;
  m_current_x = 0;
  m_current_y = 0;
  m_previous_x = 0;
  m_previous_y = 0;
  m_total_distance = 0;
}

//---------------------------------------------------------
// Destructor

Odometry::~Odometry()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Odometry::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail); //Appcast
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    
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

    string key = msg.GetKey();
    double dval = msg.GetDouble();

    m_previous_x = m_current_x;
    m_previous_y = m_current_y;
    
    if (key == "NAV_X") {
      m_current_x = dval; }
    if (key == "NAV_Y") {
      m_current_y = dval; }
    
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Odometry::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Odometry::Iterate()
{
  AppCastingMOOSApp::Iterate(); // Appcast
  double distance;
  distance = sqrt((m_current_x-m_previous_x)*(m_current_x-m_previous_x)+(m_current_y-m_previous_y)*(m_current_y-m_previous_y));

  // Odometry distance outputs once the vehicle is running.
  if (m_first_reading == true) {
    m_total_distance = distance;
    m_first_reading = false;
  }
  if (m_first_reading != true) {
    m_total_distance = m_total_distance + distance;
    Notify("ODOMETRY_DIST",m_total_distance); // Publishes ODOMETRY_DIST to MOOSDB
    
  }
  AppCastingMOOSApp::PostReport(); // Appcast
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Odometry::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp(); // Appcast
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
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

void Odometry::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables(); //Appcast
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  // Register("FOOBAR", 0);
}

bool Odometry::buildReport()
{
    m_msgs << "Total Distance Traveled: " << m_total_distance << endl;
 
    return(true);
}
