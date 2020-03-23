/************************************************************/
/*    NAME: Michael Liu                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.cpp                                        */
/*    DATE: 12 MAR 2020 - 16 MAR 2020                         */
/************************************************************/


#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "GenPath.h"
#include "XYObject.h"
#include "ColorPack.h"
#include <algorithm>
#include <map>


using namespace std;

//---------------------------------------------------------
// Constructor

GenPath::GenPath()
{
  m_all_points_mail = false;
  m_all_points_posted = false;
  m_first_time = false;
  m_first_time_regen = false;
  m_finished_search = false;
  m_current_size = 0;
  m_previous_size = 0;
  m_regenerate = false;
  m_first_dist_to_point = 0;
  m_old_x = 0.0;
  m_old_y=0.0;
  m_first_nav_x=true;
  m_first_nav_y=true;
  m_first_node_report = true;
  m_collect_nav=false;
  m_visit_radius = 0.0;
}

//---------------------------------------------------------
// Destructor

GenPath::~GenPath()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GenPath::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 

    if (key == "VISIT_POINT"){
      handleMailVisitPoint(sval);
    }

    if (key == "WPT_STAT"){
      handleMailWayPointStat(sval);
    }
   
    if (key == "NODE_REPORT_LOCAL"){
      handleMailNodeReportLocal(sval);
    }
    if (key=="GENPATH_REGENERATE"){

      handleMailRegenerate(sval);
    }
    
    if (key=="FINISHED_SEARCH"){
      handleMailFinishedSearch(sval);

      }
    if (key=="FIRST_TIME_REGEN"){
      handleMailRegenerateFirstTime(sval);
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

bool GenPath::OnConnectToServer()
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

bool GenPath::Iterate()
{
	
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  if ((m_all_points_mail==true && m_all_points_posted == false)){
    m_first_time = true;
  }

  if (m_first_time==false){ //Change !m_first_time m_regenerate 
    if (m_regenerate==true){
      
      
      m_current_size =m_dist_to_point.size();
      if (m_finished_search ==true){ //added this
	findRevisitPoints();
      }

      if ((m_revisit_points.size()==0)&&(m_finished_search==true)){ //DONE                   
        Notify("RETURN","true");
        Notify("SEARCH","false");
        Notify("MISSED_POINTS","false");
	Notify("FINISH_STATUS", "No missed points, notified to return, finished search is true");
	m_finished_status = "No missed points, notified to return, finished search is true";
        m_regenerate = false;
        return (true);
      }
      else if ((m_revisit_points.size()>0)&&(m_finished_search==true)){
        Notify("MISSED_POINTS","true");
        m_visit_points.insert(m_visit_points.end(), m_revisit_points.begin(), m_revisit_points.end());
	resetAllLists();    
	m_first_dist_to_point = 0; 
        m_finished_search = false;
        Notify("FINISHED_SEARCH","false");
	Notify("FINISH_STATUS","finished a search, visit missed points");
	m_finished_status = "finished a search, visit missed points";
      }
      else if((m_revisit_points.size()>0)&&(m_first_time_regen==true)){
        Notify("MISSED_POINTS","true");
        m_visit_points.insert(m_visit_points.end(), m_revisit_points.begin(), m_revisit_points.end());
	resetAllLists();
        m_first_time_regen=false;
	Notify("FINISHED_SEARCH","false"); 
	Notify("FINISH_STATUS","revisit missed points, first time");
	m_finished_status = "revisit missed points, first time";
      }
      else if((m_revisit_points.size()==0)&&(m_first_time_regen==true)&&(m_finished_search==true)){ //DONE              
        Notify("RETURN","true");
        Notify("SEARCH","false");
        Notify("MISSED_POINTS","false");
	Notify("FINISH_STATUS","Notified to return, no missed points");
	m_finished_status = "Notified to return, no missed points";
        m_regenerate = false;
        return (true);
      }
      else{ //Not finished                                                                   
        //      continue;                                                                    
      }
    }
    else{
      
    }
  }

  if (m_visit_points.size()>0){
	std::string updates_str = updateTransitPointsTSM(); //Update with travelling salesman, greedy nearest

    //If first time                                                                          
    if (m_first_time ==true){
      Notify("FIRST_TIME","true");
      Notify("TRANSIT_UPDATES_FIRST_TIME",updates_str);
      Notify("SEARCH","true");
      m_all_points_posted = true;
      UnRegister("VISIT_POINT");
      m_first_dist_to_point = 0;
      m_first_time = false; //Next time will be revisit points                               
    }
    else if (m_regenerate==true){
      Notify("TRANSIT_UPDATES_MISSED_POINTS",updates_str);
      Notify("MISSED_POINTS","true");
      m_first_dist_to_point = 0; // added this
    }

  }

  else{
    
    
  }


  //NEW STUFF - find minimum distance to nearest waypoint                                                                                

  int index_int = 0;
  
  for (std::vector<std::string>::iterator k = m_points_ordered.begin(); k != m_points_ordered.end(); ++k){
    double current_min = 1000.0;
    
    if (m_first_dist_to_point == 0){
      m_dist_to_point.push_back(current_min);
    }
  
    
    std::string x_str = tokStringParse(*k, "x", ',', '=');
    std::string y_str = tokStringParse(*k, "y", ',', '=');
    double x_double = 0.0;
    double y_double = 0.0;
    stringstream rr;
    stringstream ww;
    rr<<x_str;
    ww<<y_str;
    rr>>x_double;
    ww>>y_double;

  
    for (int node_index_temp = 0;node_index_temp<m_nav_x.size();node_index_temp++){  
      double temp_dist=pow((pow((x_double-m_nav_x[node_index_temp]),2)+pow((y_double-m_nav_y[node_index_temp]),2)),0.5);
      if (temp_dist<current_min){  
	current_min=temp_dist;
	}
    }	
   
    m_dist_to_point[index_int] = current_min; //Update to current min                        
    
    index_int++;
  }	
      
                                                                                
 
  m_first_dist_to_point = 1;

    
  AppCastingMOOSApp::PostReport();
  return(true);
}


//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GenPath::OnStartUp()
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
    if(param == "visit_radius") { //Added this                                             
      //handled                                                                            
      stringstream vr;   
      vr<<value;
      vr>>m_visit_radius;
     handled = true; //REmember to do this for params
    }

    else if(param == "bar") {
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  Notify("UTS_PAUSE_GENPATH","false");
  
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void GenPath::RegisterVariables()
{
  // Register("FOOBAR", 0);
  Register("VISIT_POINT",0);
  Register("NAV_X",0);
  Register("NAV_Y",0);
  Register("NODE_REPORT_LOCAL",0);
  Register("WPT_STAT",0);
  Register("WPT_INDEX",0);
  Register("GENPATH_REGENERATE",0);
  Register("FINISHED_SEARCH",0);
  Register("FIRST_TIME_REGEN",0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool GenPath::buildReport() 
{
	cout << "About to start build report "<<endl;
  m_msgs << "============================================"<< endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "Size of ordered points: "<<m_points_ordered.size()<<", distance to  points size: "<<m_dist_to_point.size()<<std::endl;
  m_msgs << "size of nav x : "<<m_nav_x.size()<<", nav y: "<<m_nav_y.size()<<std::endl;
  m_msgs << "Vist radius minimum: "<<m_visit_radius<<std::endl;
  m_msgs << "Finished search: "<<m_finished_search<<std::endl;
  m_msgs << "Regenerate: "<<m_regenerate<<std::endl;
  m_msgs << "Status: "<<m_finished_status<<std::endl;
  m_msgs << "First time through distance points: "<< m_first_dist_to_point << endl;


/*  ACTable actab(2);
  actab << "Visit Point# | Distances";
  actab.addHeaderLines();

if (!m_dist_to_point.empty()) {
  for (int dd = 0; dd<m_dist_to_point.size(); dd++){ 
cout << "in for loop about to look at m_dist_to_point with size:" << m_dist_to_point.size() << endl;
    actab << dd<<m_dist_to_point[dd];
  }
  m_msgs << actab.getFormattedString();
}*/

cout << "about to return from buildReport()"<<endl;

  return(true);
}




//---------------------------------------------------------
void GenPath::handleMailVisitPoint(std::string sval){
  std::string id_str = tokStringParse(sval, "id", ',', '=');

  if(std::find(m_id_points.begin(), m_id_points.end(), id_str) != m_id_points.end()) {
    m_all_points_mail=true;
  }
  else {
    m_id_points.push_back(id_str);
    m_visit_points.push_back(sval);
  }
  
}

void GenPath::handleMailNodeReportLocal(std::string sval){
 

  std::string x_str = tokStringParse(sval,"X",',','=');
  std::string y_str = tokStringParse(sval,"Y",',','=');
  double x_double = 0.0;
  double y_double = 0.0;
  stringstream rr;
  stringstream ww;
  rr<<x_str;
  ww<<y_str;
  rr>>x_double;
  ww>>y_double;
  
  if (m_first_node_report){
    m_old_x = x_double;
    m_old_y = y_double;
    m_first_node_report = false;
  }
 
  else if ((m_collect_nav==true)&&(m_first_node_report==false)&&( ((abs(x_double-m_old_x)>1.0)||(abs(y_double-m_old_y)>1.0)) )){
      m_nav_x.push_back(x_double);
      m_old_x=x_double;
      m_nav_y.push_back(y_double);
      m_old_y=y_double;
  }
}
   



void GenPath::handleMailNavX(double dval){
  if (m_first_nav_x){
    m_old_x=dval;
    m_first_nav_x=false;
  }

  else if ((m_collect_nav==true)&&(m_first_nav_x==false)&& (abs(dval-m_old_x)>1.0)){
    m_nav_x.push_back(dval);
    m_old_x=dval;
  }

  
}

void GenPath::handleMailNavY(double dval){
  if (m_first_nav_y){
    m_old_y=dval;
    m_first_nav_y=false;
  }
  else if ((m_collect_nav==true)&&(m_first_nav_y==false)&&(abs(dval-m_old_y)>1.0)){
    m_nav_y.push_back(dval);
    m_old_y=dval;
  }
  
}


void GenPath::handleMailWayPointStat(std::string sval){
  std::string dist_str = tokStringParse(sval,"dist",',','=');
  std::string index_str = tokStringParse(sval,"index",',','=');
  double dist_double = 0.0;
  int index_int = 0;
  stringstream dd;			     
  stringstream ii;			     
  dd<<dist_str;			     
  ii<<index_str;
  dd>>dist_double;			     
  ii>>index_int;			     
  if (dist_double<6){ 
    m_collect_nav=true;	     
  }			     
  else{			       
    m_collect_nav =false;			     
  }
}


void GenPath::handleMailRegenerate(std::string sval){
  if (sval =="true"){
    m_regenerate = true;
 }
 else{
   m_regenerate = false;
 }
}

void GenPath::handleMailFinishedSearch(std::string sval){
  if (sval =="true"){				 
    m_finished_search = true;
  }			       
  else{				 
    m_finished_search = false;				 
  }
}

void GenPath::handleMailRegenerateFirstTime(std::string sval){
  if (sval =="true"){
    m_first_time_regen = true;
 
  }
  else{
    m_first_time_regen = false;

  }
}


void GenPath::findRevisitPoints(){
   int temp_index=0;
   for (std::vector<double>::iterator k = m_dist_to_point.begin(); k != m_dist_to_point.end(); ++k){                                                                                                                      
 
          if ((*k>m_visit_radius)){ //Miss                                                                                                                                                                                        
 
                                                                                                                                                                                                                                  
            std::string id_str = tokStringParse(m_points_ordered[temp_index], "id", ',', '=');                                                                                                                                    
            if(std::find(m_id_revisit_points.begin(), m_id_revisit_points.end(), id_str) !=m_id_revisit_points.end()) {                                                                                                           
              temp_index++;                                                                                                                                                                                                       
              std::cout<<"already have it"<<std::endl;                                                                                                                                                                            
            } //already have it                                                                                                                                                                                                   
            else{                                                                                                                                                                                                                 
            //if unique id., push back                                                                                                                                                                                            
              m_id_revisit_points.push_back(id_str);                                                                                                                                                                              
                                                                                                                                                                                                                                  
                                                                                                                                                                                                                                  
              m_revisit_points.push_back(m_points_ordered[temp_index]);                                                                                                                                                           
              temp_index++;                                                                                                                                                                                                       
            }                                                                                                                                                                                                                     
          }                                                                                                                                                                                                                       
          else{ //Hit                                                                                                                                                                                                             
            temp_index++;                                                                                                                                                                                                         
          }                                                                                                                                                                                                                       
          }
}




void GenPath::resetAllLists(){
	m_revisit_points.clear(); //Clear all lists                                          
	m_dist_to_point.clear();
	m_index_points.clear();
	m_points_ordered.clear(); // added this
        m_id_revisit_points.clear();
        m_dist_final_val.clear();
        m_nav_x.clear();
        m_nav_y.clear();
        m_current_size = 0;
        m_previous_size =0;
}



std::string GenPath::updateTransitPointsTSM(){
   //Imaginary starting point                                                               
    double start_x = 0.0;
    double start_y = 0.0;
    double current_x, current_y;
    std::string current_visit_point_str;


    //List of points                                                                         
    XYSegList my_seglist;

    while (m_visit_points.size()>0){
      current_x=start_x;
      current_y=start_y;
      std::vector<std::string>::iterator current_k;
      double current_min=10000.0;
      double temp_dist = 0.0;

      //Iterate through points                                                               
      for (std::vector<std::string>::iterator k = m_visit_points.begin(); k != m_visit_points.end(); ++k){
        //Get x, y positions from string                                                     
	std::string x_str = tokStringParse(*k, "x", ',', '=');
	std::string y_str = tokStringParse(*k, "y", ',', '=');
        double x_double = 0.0;
        double y_double = 0.0;
        stringstream rr;
        stringstream ww;
        rr<<x_str;
        ww<<y_str;
        rr>>x_double;
        ww>>y_double;
        //Calculate distance                                                                 
        temp_dist=sqrt(pow((x_double-start_x),2)+pow((y_double-start_y),2));
        if (temp_dist<current_min){  // Check if less than current min, update               
          current_min=temp_dist;
          current_x=x_double;
          current_y=y_double;
          current_k=k;
          current_visit_point_str = *k;
        }
      }
      //Add to seglist. Pop off best value.Update start point.                               


      my_seglist.add_vertex(current_x,current_y);
      m_points_ordered.push_back(current_visit_point_str); //Added this                      
      m_visit_points.erase(current_k); //Updated for generating path to traverse             
      start_x = current_x;
      start_y = current_y;
    }
    std::string updates_str = "points = ";
    updates_str +=my_seglist.get_spec();
  return updates_str;
}
