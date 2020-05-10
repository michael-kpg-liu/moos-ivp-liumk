/************************************************************/
/*    NAME: ruic                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.cpp                                        */
/*    DATE: 03/17/2020                                                  */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "XYSegList.h"
#include "GenPath.h"
#include <algorithm>
#include <cmath>

using namespace std;

//---------------------------------------------------------
// Constructor

GenPath::GenPath()
{
  xpoint=0;
  ypoint=0;
  v1_vpx = {};
  v1_vpy = {};
  xloclist = {};
  yloclist = {};
  not_visited_x = {};
  not_visited_y = {};
  pt_rec = 0;
  pt_planned = 0;
  xloc=0;
  yloc=0;
  num_vp=0;
  regenpath=false;
}

//---------------------------------------------------------
// Destructor

GenPath::~GenPath()
{
}

//---------------------------------------------------------
// Procedure: Greedy minDistance

int GenPath::MinDist(double x, double y, vector<double> xvec, vector<double> yvec)
{
  int vecsize = xvec.size();
  vector<double> dist = {};

  for(int ii=0; ii<vecsize; ii++){

    dist.push_back(pow(pow(x-xvec[ii],2)+pow(y-yvec[ii],2),0.5));
  }

  vector<double>::iterator min = min_element(dist.begin(), dist.end());
  int argmin = distance(dist.begin(), min);

  return(argmin);

}

//---------------------------------------------------------
// Procedure: buildReport

bool GenPath::buildReport()
{
  m_msgs << "Number of points received " << pt_rec << endl;
  m_msgs << "Number of points added to path: " << pt_planned << endl;
  m_msgs << "Number of points visited: " << num_vp << endl;
  m_msgs << outstr << endl;
  //m_msgs << "Regen path: " << regenpath << endl;
 
  return(true);
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GenPath::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey();
    
    if (key == "VISIT_POINT"){
      pointmsg1 = msg.GetString();

      if(pointmsg1=="firstpoint" || pointmsg1=="lastpoint"){
        continue;
      }

      else{
        xpointmsg = biteStringX(pointmsg1, ',');
        xpointprefix = biteStringX(xpointmsg, '=');
        xpoint = stod(xpointmsg);
        

        ypointmsg = biteStringX(pointmsg1, ',');
        ypointprefix = biteStringX(ypointmsg, '=');
        ypoint = stod(ypointmsg);

        if(v1_vpx.empty() && v1_vpy.empty()){
          v1_vpx.push_back(xpoint);
          v1_vpy.push_back(ypoint);
          not_visited_x.push_back(xpoint);
          not_visited_y.push_back(ypoint);
          pt_rec = pt_rec + 1;
        }

        else if(xpoint==v1_vpx.back() && ypoint==v1_vpy.back()){
          continue;
        }

        else{
          v1_vpx.push_back(xpoint);
          v1_vpy.push_back(ypoint);
          not_visited_x.push_back(xpoint);
          not_visited_y.push_back(ypoint);
          pt_rec = pt_rec + 1;
        }
      }
    }

   

   if (key == "NAV_X"){
    xloc = msg.GetDouble();

    // if(xloc!= xloclist.back() || xloclist.empty()){
    //   xloclist.push_back(xloc);
    // }

   }

   if (key == "NAV_Y"){
    yloc = msg.GetDouble();

    // if((yloc!=yloclist.back())||(yloclist.empty())){
    //   yloclist.push_back(yloc);
    // }

   }

    if (key == "GENPATH_REGENERATE"){
      regenpathmsg = msg.GetString();
      if(regenpathmsg=="true"){
        regenpath = true;
      }
      else{
        regenpath = false;
      }
   }

  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool GenPath::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool GenPath::Iterate()
{
  AppCastingMOOSApp::Iterate();
  //cout<< regenpath <<endl;

  if(pointmsg1=="lastpoint" && !v1_vpx.empty()){

    double currentx = startx;
    double currenty = starty;
    XYSegList v1_seglist;
    v1_seglist.add_vertex(currentx,currenty);

    while(!v1_vpx.empty()){
      int arg = MinDist(currentx,currenty,v1_vpx,v1_vpy);
      currentx = v1_vpx[arg];
      currenty = v1_vpy[arg];
      v1_seglist.add_vertex(currentx,currenty);
      v1_vpx.erase (v1_vpx.begin()+arg);
      v1_vpy.erase (v1_vpy.begin()+arg);
      pt_planned = pt_planned + 1;
    }

    string updates_str = "points = ";
    updates_str += v1_seglist.get_spec();
    Notify("WPT_UPDATES", updates_str);

    if(regenpath && ~not_visited_x.empty()){
      Notify("RETURN","false");
      Notify("GENPATH_REGENERATE","false");
    }

  }


  if(!not_visited_x.empty()){
    vector<double> arglist = {};

    for(int kk=0;kk<not_visited_x.size();kk++){

      double dist = pow(pow(xloc-not_visited_x[kk],2)+pow(yloc-not_visited_y[kk],2),0.5);
      
      if(dist<=visit_radius){
        arglist.push_back(kk);
        num_vp = num_vp+1;
      }
    }

    if(!arglist.empty()){
      for(int aa=0;aa<arglist.size();aa++){
        not_visited_x.erase(not_visited_x.begin()+arglist[aa]);
        not_visited_y.erase(not_visited_y.begin()+arglist[aa]);
      }
    }

  }

  if(regenpath && ~not_visited_x.empty()){

    for(int kk=0;kk<not_visited_x.size();kk++){
      v1_vpx.push_back(not_visited_x[kk]);
      v1_vpy.push_back(not_visited_y[kk]);
    }
  }

  if(regenpath && not_visited_x.empty()){
    outstr = "All points visited!";
    //Notify("WPT_UPDATES", "points=0,0");
  }


  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GenPath::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  Notify("UTS_PAUSE","true");

  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
      if(tolower(param) == tolower("START_X")) {
        startx = stod(line);
      }

      if(tolower(param) == tolower("START_Y")) {
        starty = stod(line);
      }

      if(tolower(param) == tolower("VISIT_RADIUS")) {
        visit_radius = stod(line);
      }
    }
  }
  
  RegisterVariables();
  Notify("UTS_PAUSE","false");	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void GenPath::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  Register("VISIT_POINT", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("GENPATH_REGENERATE",0);

}

