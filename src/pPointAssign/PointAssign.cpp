/************************************************************/
/*    NAME: Michael Liu                                              */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.cpp                                        */
/*    DATE: 12 MAR 2020                                      */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PointAssign.h"
#include "MOOS/libMOOS/Utils/MOOSUtils.h"
#include "XYPoint.h"
#include "GeomUtils.h"
#include "XYObject.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PointAssign::PointAssign()
{
  m_reached_first_point = false;
  m_reached_last_point = false;
  m_notified_all = false;
}

//---------------------------------------------------------
// Destructor

PointAssign::~PointAssign()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PointAssign::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey();
    string sval = msg.GetString();

    if (key == "VISIT_POINT"){
      if (sval == "firstpoint"){
	m_reached_first_point = true;
	std::cout << "Reached First Point" << std::endl;
      }
      else if (sval == "lastpoint"){
	m_reached_last_point = true;
	std::cout << "Reached Last Point" << std::endl;
      }
      else if (m_reached_first_point == true && m_reached_last_point == false){
	m_visit_points.push_back(sval);
	std::cout << "Added Point to Visit Points" << std::endl;
      }
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

bool PointAssign::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PointAssign::Iterate()
{
  std::cout << "Notified All: " << m_notified_all << std::endl;
  // Loop through Points and Alternate Vessel Assignment
  if (m_assign_by_region == false){
    int i = 0;
    for (std::vector<std::string>::const_iterator k = m_visit_points.begin(); k != m_visit_points.end(); ++k){
      // tokStringParse:  Function works on a comma-separated list of parameter=value paris and pulls out the
      // value for a given parameter.  First character argument is the "global" separation, and the second
      // argument is the "local" separator.  First character - reason for pre-increment.
      std::string x_str = tokStringParse(*k, "x", ',', '='); // Stored in "x_str"
      std::string y_str = tokStringParse(*k, "y", ',', '='); // Stored in "y_str"
      std::string id_str = tokStringParse(*k,"id",',','=');  // Stored in "id_str"
      std::cout << "ID = " << id_str << std::endl;           // Check ID
      double x_double = 0.0;
      double y_double = 0.0;
      stringstream rr;
      stringstream ww;

      rr << x_str; // Stores string of x_str
      ww << y_str; // Stores string of y_str
      rr >> x_double; // Inputs into x_double
      ww >> y_double; // Inputs into y_double
      std::string color_label_str;

      // Alternate Vessel Alignment
      // Note that "HENRY" is "default", since int i = 0 is original definition.
      if (i == 0){
	m_vname_str = "HENRY";
	i=1;
	color_label_str = "red";
      }

      else{
	m_vname_str = "GILDA";
	i=0;
	color_label_str = "yellow";
      }

      stringstream ss;
      ss << "VISIT_POINT_" << m_vname_str; // Stores string of vehicle names
      Notify(ss.str(),*k);                 // Notifies database
      postViewPoint(x_double, y_double, id_str, color_label_str); // Calls prodecure at bottom of .cpp file.
    }
    std::cout << "Finished looping through all points." << std::endl;
    m_notified_all = true;
  }
    // Assign by Region
    else {
      std::cout << "Assigning by Region:  Visit Points: " << m_visit_points.size() << std::endl;
      for(std::vector<std::string>::const_iterator k = m_visit_points.begin(); k!= m_visit_points.end(); ++k){
	std::string x_str = tokStringParse(*k, "x", ',', '=');
	std::string y_str = tokStringParse(*k, "y", ',', '=');
	std::string id_str = tokStringParse(*k, "id", ',','=');
	std::cout << "ID = " << id_str << std::endl;
	double x_double = 0.0;
	double y_double = 0.0;
	stringstream rr;
	stringstream ww;
	
	rr << x_str;
	ww << y_str;
	rr >> x_double;
	ww >> y_double;
	
	bool is_east = PointRegionIsEast(x_double);
	std::cout << "East: " << is_east << std::endl;
	if (is_east){
	  stringstream vv;
	  vv << "VISIT_POINT_" << m_vname_list[0];
	  std::cout << vv.str() << std::endl;
	  Notify(vv.str(),*k);
	  std::cout << "Calling post view point" << std::endl;
	  postViewPoint(x_double, y_double, id_str, "yellow");
	}
	else{
	  stringstream vv;
	  vv << "VISIT_POINT_" << m_vname_list[1];
	  std::cout << vv.str() << std::endl;
	  Notify(vv.str(),*k);
	  std::cout << "Calling post view point" << std::endl;
	  postViewPoint(x_double, y_double, id_str, "red");
	}
      }
      std::cout << "Finished looping through all points." << std::endl;
      m_notified_all = true;
    }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PointAssign::OnStartUp()
{
  Notify("UTS_PAUSE","false");
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line  = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);
      std::cout << "param: " << param << std::endl;

      // Determine Vehicle Name and Whether to Assign Region
      if(param == "VNAME") {
	m_vname_list.push_back(value);
      }
      else if (param == "ASSIGN_BY_REGION") {
	if (value == "true"){
	  m_assign_by_region = true;
	}
	else{
	  m_assign_by_region = false;
	}
      //if(param == "foo") {
        //handled
      //}
      // else if(param == "bar") {
        //handled
      //}
      }
    }
  }
  
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void PointAssign::RegisterVariables()
{
  // Register("FOOBAR", 0);
  Register("VISIT_POINT",0);
}

//---------------------------------------------------------
// Procedure: PointRegionIsEast

bool PointAssign::PointRegionIsEast(double x_val)
{
  return (x_val<100.0);
}

//---------------------------------------------------------
// Procedure: postViewPoint

void PointAssign::postViewPoint(double x, double y, string label, string color)
{
  XYPoint point(x,y);
  point.set_label(label);
  point.set_label_color(color);
  point.set_color("vertex",color);
  point.set_param("vertex_size","2");

  string spec = point.get_spec();
  std::cout << "Notifying View Point" << std::endl;
  Notify("VIEW_POINT",spec);
}


