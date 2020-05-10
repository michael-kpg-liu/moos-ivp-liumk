/************************************************************/
/*    NAME: ruic                                              */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.cpp                                        */
/*    DATE: 03/17/2020                                                */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "XYPoint.h"
#include "PointAssign.h"
#include <string>

using namespace std;

//---------------------------------------------------------
// Constructor

PointAssign::PointAssign()
{
  num_vehs=0;
  xpoint=0;
  ypoint=0;
  id=0;
  msg_vec={};
  pt_rec = 0;
  pt_bc = 0;
}

//---------------------------------------------------------
// Destructor

PointAssign::~PointAssign()
{
}

//---------------------------------------------------------
// Procedure: buildReport

bool PointAssign::buildReport()
{
  m_msgs << "Number of points received " << pt_rec << endl;
  m_msgs << "Number of points broadcasted: " << pt_bc << endl;
 
  return(true);
}

//---------------------------------------------------------
// Procedure: postViewPoint

void PointAssign::postViewPoint(double x, double y, string label, string color)
{
  XYPoint point(x, y); 
  point.set_label(label); 
  point.set_color("vertex", color); 
  point.set_param("vertex_size", "5");
  string spec = point.get_spec();

  Notify("VIEW_POINT", spec); 
}

//---------------------------------------------------------
// Procedure: OnNewMail

// get incoming mail and put all msg in a vector

bool PointAssign::OnNewMail(MOOSMSG_LIST &NewMail)
{

  AppCastingMOOSApp::OnNewMail(NewMail);

  num_vehs = veh_names.size();

  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey();
    
    if (key == "VISIT_POINT"){

      ptmsg = msg.GetString();
      msg_vec.push_back(ptmsg);

      pt_rec = pt_rec+1;
    }
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

//check if received "lastpoint"
//if so, check assign_by_region
//notify vehicles of their way points

bool PointAssign::Iterate()
{ 
  AppCastingMOOSApp::Iterate();

  if(ptmsg=="lastpoint" && !msg_vec.empty()){
    cout << msg_vec.size() <<endl;

    if(assign_by_reg){

      while(!msg_vec.empty()){

        if(msg_vec.front()=="firstpoint"){
          out = "firstpoint";
          v1out = "VISIT_POINT_"+veh_names[0];
          v2out = "VISIT_POINT_"+veh_names[1];
          Notify(v1out,out);
          Notify(v2out,out);
          pt_bc = pt_bc + 1;
          msg_vec.erase(msg_vec.begin());
        }

        else if(msg_vec.front()=="lastpoint"){
          out = "lastpoint";
          v1out = "VISIT_POINT_"+veh_names[0];
          v2out = "VISIT_POINT_"+veh_names[1];
          Notify(v1out,out);
          Notify(v2out,out);
          pt_bc = pt_bc + 1;
          msg_vec.erase(msg_vec.begin());
        }

        else{
          pointmsg = msg_vec.front();
          xpointmsg = biteStringX(pointmsg, ',');
          xpointprefix = biteStringX(xpointmsg, '=');
          xpoint = stod(xpointmsg);

          ypointmsg = biteStringX(pointmsg, ',');
          ypointprefix = biteStringX(ypointmsg, '=');
          ypoint = stod(ypointmsg);

          idmsg = biteStringX(pointmsg, ',');
          idprefix = biteStringX(idmsg, '=');
          id = stod(idmsg);

          out = "x="+to_string(xpoint)+", y="+to_string(ypoint)+", id="+to_string(id);

          if(xpoint>=87.5){
            v1out = "VISIT_POINT_"+veh_names[0];
            postViewPoint(xpoint, ypoint, to_string(id), colors[0]);
            Notify(v1out,out);
            pt_bc = pt_bc + 1;
          }
          
          else{
            v2out = "VISIT_POINT_"+veh_names[1];
            postViewPoint(xpoint, ypoint, to_string(id), colors[1]);
            Notify(v2out,out);
            pt_bc = pt_bc + 1;
          }

          msg_vec.erase(msg_vec.begin());
        }
      }

    }

    else{

      while(!msg_vec.empty()){

        if(msg_vec.front()=="firstpoint"){
          out = "firstpoint";
          v1out = "VISIT_POINT_"+veh_names[0];
          v2out = "VISIT_POINT_"+veh_names[1];
          Notify(v1out,out);
          Notify(v2out,out);
          pt_bc = pt_bc + 1;
          msg_vec.erase(msg_vec.begin());
        }

        else if(msg_vec.front()=="lastpoint"){
          out = "lastpoint";
          v1out = "VISIT_POINT_"+veh_names[0];
          v2out = "VISIT_POINT_"+veh_names[1];
          Notify(v1out,out);
          Notify(v2out,out);
          pt_bc = pt_bc + 1;
          msg_vec.erase(msg_vec.begin());
        }

        else{
          pointmsg = msg_vec.front();
          xpointmsg = biteStringX(pointmsg, ',');
          xpointprefix = biteStringX(xpointmsg, '=');
          xpoint = stod(xpointmsg);

          ypointmsg = biteStringX(pointmsg, ',');
          ypointprefix = biteStringX(ypointmsg, '=');
          ypoint = stod(ypointmsg);

          idmsg = biteStringX(pointmsg, ',');
          idprefix = biteStringX(idmsg, '=');
          id = stod(idmsg);

          out = "x="+to_string(xpoint)+", y="+to_string(ypoint)+", id="+to_string(id);

          if((id%2)==0){
            v1out = "VISIT_POINT_"+veh_names[0];
            postViewPoint(xpoint, ypoint, to_string(id), colors[0]);
            Notify(v1out,out);
            pt_bc = pt_bc + 1;
          }
          
          else{
            v2out = "VISIT_POINT_"+veh_names[1];
            postViewPoint(xpoint, ypoint, to_string(id), colors[1]);
            Notify(v2out,out);
            pt_bc = pt_bc + 1;
          }

          msg_vec.erase(msg_vec.begin());

        }
      }

    }
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PointAssign::OnStartUp()
{ 
  AppCastingMOOSApp::OnStartUp();
  //Notify("UTS_PAUSE","true");
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      if(tolower(param) == tolower("VNAME")) {
        veh_names.push_back(value);
      }

      else if(tolower(param) == tolower("ASSIGN_BY_REGION")) {

        try{
          double num = stod(value);

          if(num == 1){
            assign_by_reg = true;
          }

          else if(num == 0){
            assign_by_reg = false;
          }

          else{
            assign_by_reg = false;
            cout << "INVALIDE ASSIGN_BY_REGION INPUT; SET TO FALSE." << endl;
          }
        }

        catch(...){
          if(tolower(value) == "true"){
            assign_by_reg = true;
          }

          else if(tolower(value) == "false"){
            assign_by_reg = false;
          }

          else{
            assign_by_reg = false;
            cout << "INVALIDE ASSIGN_BY_REGION INPUT; SET TO FALSE." << endl;
          } 

        }

      }
    }
  }
  
  RegisterVariables();
  //Notify("UTS_PAUSE","false");
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void PointAssign::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("VISIT_POINT",0);
}

