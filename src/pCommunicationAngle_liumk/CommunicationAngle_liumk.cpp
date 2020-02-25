/************************************************************/
/*    NAME: liumk                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle_liumk.cpp                           */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cmath>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include "MBUtils.h"
#include "CommunicationAngle_liumk.h"

using namespace std;

//---------------------------------------------------------
// Constructor

CommunicationAngle_liumk::CommunicationAngle_liumk()
{
  m_surface_sound_speed = 0; // Units: m/s
  m_sound_speed_gradient = 0; // Units: (m/s)/m
  m_water_depth = 0; // Units: m
  m_COLLABORATOR_NAME = "";
  m_time_interval = 0; // Units: s
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

  string sval = msg.GetString();
  string key = msg.GetKey();
  
  if (sval == "COLLABORATOR_NAME"){
    m_COLLABORATOR_NAME = msg.GetString();
  }
  else if (key == "NAV_X"){
    m_NAV_X = msg.GetDouble();
  }
  else if (key == "NAV_Y"){
    m_NAV_Y = msg.GetDouble();
  }
  else if (key == "NAV_DEPTH"){
    m_NAV_DEPTH = msg.GetDouble();
  }
   else if (key == "NAV_HEADING"){
    m_NAV_HEADING = msg.GetDouble();
  }
   else if (key == "NAV_SPEED"){
    m_NAV_SPEED = msg.GetDouble();
  }
  else if (key == "COLLABORATOR_NAV_X"){
    m_COLLABORATOR_NAV_X = msg.GetDouble();
  }
  else if (key == "COLLABORATOR_NAV_Y"){
    m_COLLABORATOR_NAV_Y = msg.GetDouble();
  }
  else if (key == "COLLABORATOR_NAV_DEPTH"){
    m_COLLABORATOR_NAV_DEPTH = msg.GetDouble();
  }
  else if (key == "COLLABORATOR_NAV_HEADING"){
    m_COLLABORATOR_NAV_HEADING = msg.GetDouble();
  }
  else if (key == "COLLABORATOR_NAV_SPEED"){
    m_COLLABORATOR_NAV_SPEED = msg.GetDouble();
  }
  }
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CommunicationAngle_liumk::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CommunicationAngle_liumk::Iterate()
{
  m_chord = sqrt((m_NAV_X-m_COLLABORATOR_NAV_X)*(m_NAV_X-m_COLLABORATOR_NAV_X)+(m_NAV_Y-m_COLLABORATOR_NAV_Y)*(m_NAV_Y-m_COLLABORATOR_NAV_Y));
  m_depth_difference = (m_NAV_DEPTH-m_COLLABORATOR_NAV_DEPTH);
  // Grazing angle assumed to be same as sind(theta) of depth difference/chord distance
  m_grazing_angle = asin(m_depth_difference/m_chord);
  m_radius = m_surface_sound_speed/(m_sound_speed_gradient*cos(m_grazing_angle));
  m_center_chord_depth_difference = m_radius-m_depth_difference;
  
  // Intersecting Chords Theorem https://en.wikipedia.org/wiki/Circular_segment
  // m_chord_angle translates to triangle angle across chord
  // Arc Length = m_theta*m_radius
  
  m_chord_angle = asin(m_chord/(m_depth_difference+m_chord*m_chord/(4*m_depth_difference)));
  m_arc_length = m_chord_angle*(m_depth_difference + m_chord*m_chord/(4*m_depth_difference));

  m_sound_speed = m_surface_sound_speed + m_sound_speed_gardient*(m_NAV_DEPTH);
  double m_transmission_loss = 0;
  
  for (int i = 0; i <= m_depth_difference; i = i+abs(m_depth_difference/100)) {
    double m_sound_speed_i = m_surface_sound_speed + m_sound_speed_gradient*(m_NAV_DEPTH+i);
    double m_Jacobian = (m_NAV_X-m_COLLABORATOR_NAV_X)*m_arc_length*(1-i/m_depth_difference);
    double m_pressure = sqrt(((m_sound_speed_i*cos(m_grazing_angle))/(m_sound_speed*m_Jacobian)));
    double transmission_loss_i = -20*log10(m_pressure);
    m_transmission_loss = m_transmission_loss + m_transmission_loss_i;
    cout << "Transmission Loss: " <<  m_transmission_loss << endl;
  }



  
  if (2*m_radius < m_chord){
    Notify("ACOUSTIC_PATH","NaN");
    Notify("CONNECTIVITY_LOCATION",CONNECTIVITY_LOCATION());
    double m_y = m_NAV_Y; // Assume same for both vehicles.  2D Approximation given by Professor Schmidt during Lab 6.
  }
  if (2*m_radius >= m_chord){
    Notify("ACOUSTIC_PATH",ACOUSTIC_PATH());
    double m_x = m_NAV_X;
    double m_y = m_NAV_Y;
    double m_depth = m_NAV_DEPTH;
    Notify("CONNECTIVITY_LOCATION",CONNECTIVITY_LOCATION());
  
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
      bool handle = false;
      
      if(param == "surface_sound_speed") {
	string str_surface_sound_speed = value;
	stringstream ss;
	ss << str_surface_sound_speed;
	ss >> m_surface_sound_speed;

	handle = true;
      }

      else if(param == "sound_speed_gradient") {
	string str_sound_speed_gradient = value;
	stringstream ss;
	ss << str_sound_speed_gradient;
	ss >> m_sound_speed_gradient;

	handle = true;
      }
      
      else if(param == "water_depth") {
	string str_water_depth = value;
	stringstream ss;
	ss << str_water_depth;
	ss >> m_water_depth;

	handle = true;
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
// Purpose:  ACOUSTIC_PATH and CONNECTIVITY_LOCATION Output

string CommunicationAngle_liumk::ACOUSTIC_PATH()
{
  string result = "";
  result += "ACOUSTIC_PATH: elev_angle=";
  result += to_string(m_grazing_angle); // Grazing Angle equals elevation angle????
  result += ", transmission_loss=";
  result += to_string(m_transmission_loss);
  result += ", id=liumk@mit.edu";
  return(result);
}


//---------------------------------------------------------
// Purpose:  ACOUSTIC_PATH and CONNECTIVITY_LOCATION Output

string CommunicationAngle_liumk::CONNECTIVITY_LOCATION()
{
  string result = "";
  result += "CONNECTIVITY_LOCATION: x=";
  result += to_string(m_x);
  result += ", y=";
  result += to_string(m_y);
  result += ", depth=";
  result += to_string(m_depth);
  result += ", id=liumk@mit.edu";
  return(result);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void CommunicationAngle_liumk::RegisterVariables()
{
  Register("COLLABORATOR_NAME", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("NAV_DEPTH", 0);
  Register("NAV_HEADING", 0);
  Register("NAV_SPEED", 0);
  Register("COLLABORATOR_NAV_X", 0);
  Register("COLLABORATOR_NAV_Y", 0);
  Register("COLLABORATOR_NAV_DEPTH", 0);
  Register("COLLABORATOR_NAV_HEADING", 0);
  Register("COLLABORATOR_NAV_SPEED", 0);
  // Register("FOOBAR", 0);
}

