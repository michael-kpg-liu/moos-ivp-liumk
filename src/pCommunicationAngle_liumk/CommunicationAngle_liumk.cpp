/************************************************************/
/*    NAME: liumk                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle_liumk.cpp                           */
/*    DATE: 25 FEB 2020                                                */
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
  m_surface_sound_speed = 1480; // Units: m/s
  m_sound_speed_gradient = 0.016; // Units: (m/s)/m
  m_water_depth = 6000; // Units: m
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

  if (sval == "VEHICLE_NAME"){
    m_VEHICLE_NAME = msg.GetString();
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

  else if (sval == "COLLABORATOR_NAME"){
    m_COLLABORATOR_NAME = msg.GetString();
    neptune_NAV_X = m_COLLABORATOR_NAME + "_NAV_X";
    neptune_NAV_Y = m_COLLABORATOR_NAME + "_NAV_Y";
    neptune_NAV_DEPTH = m_COLLABORATOR_NAME + "_NAV_DEPTH";
    neptune_NAV_HEADING = m_COLLABORATOR_NAME + "_NAV_HEADING";
    neptune_NAV_SPEED = m_COLLABORATOR_NAME + "_NAV_SPEED";
  }
  
  else if (key == "neptune_NAV_X"){
    m_neptune_NAV_X = msg.GetDouble();
  }
  else if (key == "neptune_NAV_Y"){
    m_neptune_NAV_Y = msg.GetDouble();
  }
  else if (key == "neptune_NAV_DEPTH"){
    m_neptune_NAV_DEPTH = msg.GetDouble();
  }
  else if (key == "neptune_NAV_HEADING"){
    m_neptune_NAV_HEADING = msg.GetDouble();
  }
  else if (key == "neptune_NAV_SPEED"){
    m_neptune_NAV_SPEED = msg.GetDouble();
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
  //ACOUSTIC_PROFILE
  
  // Chord Distance Analysis Between Two Points [Circle - Acoustic Path Approximation]
  m_chord = sqrt((m_NAV_X-m_neptune_NAV_X)*(m_NAV_X-m_neptune_NAV_X)+(m_NAV_Y-m_neptune_NAV_Y)*(m_NAV_Y-m_neptune_NAV_Y));
  m_depth_difference = (m_NAV_DEPTH-m_neptune_NAV_DEPTH);

  // Grazing angle assumed to be same as sind(theta) of depth difference/chord distance
  // Small angle approximation uses triangle approximation
  m_grazing_angle = asin(m_depth_difference/m_chord);

  // Radius
  m_sound_speed = m_surface_sound_speed + m_sound_speed_gradient*(m_NAV_DEPTH);
  m_radius = m_sound_speed/(m_sound_speed_gradient*cos(m_grazing_angle));

  // Arc Length
  // Intersecting Chords Theorem https://en.wikipedia.org/wiki/Circular_segment
  // m_chord_angle translates to triangle angle across chord
  // Arc Length = m_theta*m_radius
  m_center_chord_depth_difference = m_radius-m_depth_difference;
  m_chord_angle = asin(m_chord/(m_depth_difference+m_chord*m_chord/(4*m_depth_difference)));
  m_arc_length = m_chord_angle*(m_depth_difference+m_chord*m_chord/(4*m_depth_difference));

  // Projection of Circular Arc
  m_r_projection = m_radius*(sin(m_grazing_angle) + sin(m_arc_length/m_radius - m_grazing_angle));
  m_depth_projection = m_radius*cos(m_arc_length/m_radius)-m_surface_sound_speed/m_sound_speed_gradient;

  // Elevation Angle
  m_elevation_angle = atan(m_depth_projection/m_r_projection);

  // Transmission Loss
  
  // Incrementally Adjusted - m_depth_difference broken into XXX steps
  double m_number_of_steps = 100000;
  for (int i = 0; i <= m_depth_difference; i = i+abs(m_depth_difference/m_number_of_steps)) {
    double m_sound_speed_i = m_surface_sound_speed + m_sound_speed_gradient*(m_NAV_DEPTH+i);
    // m_Jacobian estimated as radius times portion of arc length "traveled"
    double m_Jacobian = (m_NAV_X-m_neptune_NAV_X)*m_arc_length*(1-i/m_depth_difference);
    // 1/(4*pi) constant ignored, given denominator, p(1) = 1/(4*pi)
    double m_pressure = sqrt(((m_sound_speed_i*cos(m_grazing_angle))/(m_sound_speed*m_Jacobian)));
    double transmission_loss_i = -20*log10(m_pressure);
    m_transmission_loss = m_transmission_loss + m_transmission_loss_i;
  }

  string elevation_angle = "elev_angle=" + doubleToString(m_elevation_angle,4);
  string transmission_loss = ", transmission_loss=" + doubleToString(m_transmission_loss,4);
  string acoustic_path_id = ",id=liumk@mit.edu";
  string acoustic_path = elevation_angle+transmission_loss+id;
  
  // CONNECTIVITY_LOCATION

  // Maximum Circular Arc Depth
  m_maximum_circular_arc_depth = m_sound_speed/(m_sound_speed_gradient*cos(m_grazing_angle))-m_surface_sound_speed/m_sound_speed_gradient;
  
  if (m_water_depth >= m_maximum_circular_arc_depth){
    Notify("ACOUSTIC_PATH",acoustic_path);
    double m_x = m_NAV_X;
    double m_y = m_NAV_Y;
    double m_depth = m_NAV_DEPTH;
    string m_x_position = "x=" + doubleToString(m_NAV_X,6);
    string m_y_position = ",y=" + doubleToString(m_NAV_Y,6);
    string m_depth_position = ",depth=" + doubleToString(m_NAV_DEPTH,4);
    string m_connectivity_location_id = ",id=liumk@mit.edu";
    string m_connectivity_location = m_x_position+m_y_position+m_depth_position+id;
    Notify("CONNECTIVITY_LOCATION",m_connectivity_location);
  }
  else if (m_water_depth < m_maximum_circular_arc_depth){
    Notify("ACOUSTIC_PATH","NaN");
    
    // Maximum Sound Speed Dependent on Maximum Depth of Circular Arc
    double m_max_sound_speed = m_surface_sound_speed+m_sound_speed_gradient*m_maximum_circular_arc_depth;
    
    // Variable Calculations Dependent on Collaborator Vehicle Postion (not own vehicle)
    double m_neptune_sound_speed = m_surface_sound_speed+m_sound_speed_gradient*m_neptune_NAV_DEPTH;
    double m_max_grazing_angle = acos(m_neptune_sound_speed/m_max_sound_speed);
    double m_max_radius = m_neptune_sound_speed/(m_sound_speed_gradient*cos(m_max_grazing_angle));
    
    // Determine New_X and New_Y Based on Revised Radius
    // Assume circle radius "in-line" with "x" of collaborator vehicle
    double m_circle_center_depth = m_surface_sound_speed/m_sound_speed_gradient;
    double m_revised_x = sqrt((m_max_radius*m_max_radius)-(m_neptune_NAV_DEPTH-m_circle_center_depth)*(m_neptune_NAV_DEPTH-m_circle_center_depth));

    // Determine Path to New Circle Center
    double m_rho = acos(m_revised_x/m_max_radius);
    double m_revised_y = m_max_radius*sin(m_rho);
    if (m_revised_y > m_water_depth){ // Bottom Boundary Concern
      m_revised_y=m_NAV_DEPTH; // Maintain Depth
    }
    else if (m_revised_y < 0){ // Air Boundary Concern
      m_revised_y = m_NAV_DEPTH; // Maintain Depth
    }

    // Determine Adjusted Projections
    double m_adjusted_r_projection = m_r_projection/(m_chord-m_revised_x); // Similarity - Euclidean Geometry (Triangles)

    // Determine New Own Vehicle Navigation Direction
    double m_phi = atan((m_neptune_NAV_Y-m_NAV_Y)/(m_neptune_NAV_X-m_NAV_X)); // Angle between Own Vehicle and Collaborator
    double m_new_NAV_X = m_adjusted_r_projection*cos(m_phi);
    double m_new_NAV_Y = m_adjusted_r_projection*sin(m_phi);
    
    string m_new_x_position = "x=" + doubleToString(m_new_NAV_X,6);
    string m_new_y_position = ",y=" + doubleToString(m_new_NAV_Y,6);
    string m_new_depth_position = ",depth=" + doubleToString(m_revised_y,4);
    string m_new_connectivity_location_id = ",id=liumk@mit.edu";
    string m_new_connectivity_location = m_x_position+m_y_position+m_depth_position+id;
    Notify("CONNECTIVITY_LOCATION",m_new_connectivity_location);
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
// Purpose:  ACOUSTIC_PATH Output

//string CommunicationAngle_liumk::ACOUSTIC_PATH()
//{
//  string result = "";
//  result += "ACOUSTIC_PATH: elev_angle=";
//  result += to_string(m_elevation_angle);
//  result += ", transmission_loss=";
//  result += to_string(m_transmission_loss);
//  result += ", id=liumk@mit.edu";
//  return(result);
//}


//---------------------------------------------------------
// Purpose:  CONNECTIVITY_LOCATION Output

//string CommunicationAngle_liumk::CONNECTIVITY_LOCATION()
//{
//  string result = "";
//  result += "CONNECTIVITY_LOCATION: x=";
//  result += to_string(m_x);
//  result += ", y=";
//  result += to_string(m_y);
//  result += ", depth=";
//  result += to_string(m_depth);
//  result += ", id=liumk@mit.edu";
//  return(result);
//}

//---------------------------------------------------------
// Procedure: RegisterVariables

void CommunicationAngle_liumk::RegisterVariables()
{
  Register("VEHICLE_NAME", 0);
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
  Register("neptune_NAV_X", 0);
  Register("neptune_NAV_Y", 0);
  Register("neptune_NAV_DEPTH", 0);
  Register("neptune_NAV_HEADING", 0);
  Register("neptune_NAV_SPEED", 0);
  
  // Register("FOOBAR", 0);
}

