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
#include "MBUtils.h"
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
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 
   

    if (key=="VISIT_POINT"){
      if (sval=="firstpoint"){
	m_reached_first_point = true;
	std::cout<<"reached first point"<<std::endl;
      }
      else if (sval == "lastpoint"){
	m_reached_last_point = true;
	std::cout<<"reached last point"<<std::endl;
      }
      else if (m_reached_first_point==true && m_reached_last_point ==false){
	m_visit_points.push_back(sval);
	std::cout<<"added point to visit points"<<std::endl;
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
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);
	
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PointAssign::Iterate()
{
  // NVM: Only iterate if first and last points
  std::cout<<"notified all: "<<m_notified_all<<std::endl;
  //  if (m_reached_first_point == true && m_reached_last_point == true && m_notified_all == false){
  //   if (m_reached_first_point == true && m_reached_last_point == true && m_notified_all == false){
  //  if (m_reached_first_point == true && m_reached_last_point == true){

  //Loop through list of points and alternate assignment
  if (m_assign_by_region ==false){
    
    //  std::vector<std::string>::const_iterator i = m_vname_list.begin();
    int i = 0;
    for (std::vector<std::string>::const_iterator k = m_visit_points.begin(); k != m_visit_points.end(); ++k){
      
      std::string x_str = tokStringParse(*k, "x", ',', '=');
      std::string y_str = tokStringParse(*k, "y", ',', '=');
      std::string id_str = tokStringParse(*k,"id",',','=');
      std::cout<<"ID = "<<id_str<<std::endl;
       double x_double = 0.0;
       double y_double = 0.0;
       stringstream rr;
       stringstream ww;
       
       rr<<x_str;
       ww<<y_str;
       rr>>x_double;
       ww>>y_double;
       std::string color_label_str;
       
      if (i==0){
	m_vname_str="HENRY";
	i=1;
	color_label_str = "red";
      }
      else{
	m_vname_str="GILDA";
	i=0;
	color_label_str= "yellow";
      }
	stringstream ss;
	ss<<"VISIT_POINT_"<<m_vname_str;
	Notify(ss.str(),*k);
	postViewPoint(x_double, y_double,id_str, color_label_str);
    }
    std::cout<<"finished looping through all points"<<std::endl;
    m_notified_all = true;

  }
  else{ //Assign by region
    std::cout<<"Assigning by region"<<"size of visit points: "<<m_visit_points.size()<<std::endl;
    for (std::vector<std::string>::const_iterator k = m_visit_points.begin(); k != m_visit_points.end(); ++k){
      std::string x_str = tokStringParse(*k, "x", ',', '=');
      std::string y_str = tokStringParse(*k, "y", ',', '=');
      std::string id_str = tokStringParse(*k,"id",',','=');
      std::cout<<"ID = "<<id_str<<std::endl;
       double x_double = 0.0;
       double y_double = 0.0;
       stringstream rr;
       stringstream ww;
       rr<<x_str;
       ww<<y_str;
       rr>>x_double;
       ww>>y_double;
       
       bool is_east = PointRegionIsEast(x_double);
       std::cout<<"east: "<<is_east<<std::endl;
       if (is_east){
	 	stringstream vv;
		vv<<"VISIT_POINT_"<<m_vname_list[0];
		std::cout<<vv.str()<<std::endl;
		Notify(vv.str(),*k);
		std::cout<<"calling post view point"<<std::endl;
		postViewPoint(x_double, y_double, id_str, "yellow");

       }
       else{
	 	stringstream vv;
		vv<<"VISIT_POINT_"<<m_vname_list[1];
		std::cout<<vv.str()<<std::endl;
		Notify(vv.str(),*k);
		std::cout<<"calling post view point"<<std::endl;
		postViewPoint(x_double, y_double, id_str, "red");

	
       }
    }
    std::cout<<"finished looping through all points"<<std::endl;
    m_notified_all = true;
  }
  
  // }
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
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);
         std::cout<<"param: "<<param<<std::endl;
      if(param == "VNAME") {
        //add to vehicle list of names
		m_vname_list.push_back(value);
      }
      else if(param == "ASSIGN_BY_REGION") {
	if (value=="true"){
	  m_assign_by_region = true;
	}
	else{
	  m_assign_by_region = false;
	}
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
  Register("VISIT_POINT_HENRY",0);
  Register("VISIT_POINT_GILDA",0);
}


//--------------------------------------------------

bool PointAssign::PointRegionIsEast(double x_val){
  return (x_val<100.0);

}

//----------------------------------------------------

void PointAssign::postViewPoint(double x, double y, string label, string color){
 {
   XYPoint point(x, y);
   point.set_label(label);
   point.set_label_color(color);
   point.set_color("vertex", color);
   point.set_param("vertex_size", "5");

   string spec = point.get_spec();
   std::cout<<"notifying view point"<<std::endl;
   Notify("VIEW_POINT", spec);
 }


}
