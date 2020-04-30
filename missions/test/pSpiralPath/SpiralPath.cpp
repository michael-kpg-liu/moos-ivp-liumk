/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: SpiralPath.cpp                                  */
/*    DATE: 2019 May                                        */
/************************************************************/

#include <algorithm> 
#include <iterator>
#include <string>
#include "MBUtils.h"
#include "ACTable.h"
#include "NodeRecordUtils.h"
#include "NodeMessage.h"
#include "NodeMessageUtils.h"
#include "SpiralPath.h"
#include "XYObject.h"
#include "XYVector.h"
#include "XYPoint.h"

using namespace std;


//---------------------------------------------------------
// Constructor

SpiralPath::SpiralPath()
{
  // loiter parameters
  m_osx               = 0;
  m_osy               = 0;
  m_loiter_x          = 0;
  m_loiter_y          = 0;
  m_loiter_radius     = 0;
  m_loiter_delta      = 0;
  m_loiter_offset     = 50;

  // variables parsed from the Front Estimator
  m_num_estimate_rcd  = 0;
  m_num_cycles        = 0;
  m_num_msgs_sent     = 0;
  m_num_msgs_received = 0;
  m_req_new_estimate  = false;
  m_spr_active        = false;
  m_est_offset        = 0;
  m_est_angle         = 0;
  m_est_amplitude     = 0;
  m_est_period        = 0;
  m_est_wavelength    = 0;
  m_est_alpha         = 0;
  m_est_beta          = 0;
  m_est_tempnorth     = 0;
  m_est_tempsouth     = 0;
}


//---------------------------------------------------------
// Procedure: OnNewMail

bool SpiralPath::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval   = msg.GetString(); 
    bool handled  = false;

    if(key == "NODE_REPORT")
      handled = handleNodeReport(sval);
    else if(key == "NODE_REPORT_LOCAL")
      handled = handleNodeReportLocal(sval);
    else if(key == "UCTD_MSMNT_REPORT")
      handled = handleNewSensorReport(sval);
    else if(key == "SPIRAL_REPORT_PARAMS")
      handled = handleParameterEstimate(sval);
    else if(key == "LOITER_REPORT")
      handled = handleLoiterReport(sval);
    else if(key == "NAV_X")
      handled = handleNavX(sval);
    else if(key == "NAV_Y")
      handled = handleNavY(sval);
    else if(key == "MOVE_CENTER")
      handled = handleSpiralMode(sval);
    else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}


//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SpiralPath::OnConnectToServer()
{
   registerVariables();
   return(true);
}


//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool SpiralPath::Iterate()
{
  AppCastingMOOSApp::Iterate();

  AppCastingMOOSApp::PostReport();
  return(true);
}


//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool SpiralPath::OnStartUp()
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

    if(param == "delta") 
      handled = setLoiterDelta(value);
    if(param == "captain") 
      handled = setCaptain(value);
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  registerVariables();	
  return(true);
}


//------------------------------------------------------------
// Procedure: handleParameterEstimate()

bool SpiralPath::handleParameterEstimate(string request_string)
{
  m_num_estimate_rcd++;
  m_latest_estimate = request_string;

  // parse all of the values in the parameter estimate
  vector<string> svector = parseString(request_string, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    if (param == "vname")
      m_est_vname = value;
    else if (param == "offset")
      m_est_offset = atof(value.c_str());
    else if (param == "angle")
      m_est_angle = atof(value.c_str());
    else if (param == "amplitude")
      m_est_amplitude = atof(value.c_str());
    else if (param == "period")
      m_est_period = atof(value.c_str());
    else if (param == "wavelength")
      m_est_wavelength = atof(value.c_str());
    else if (param == "alpha")
      m_est_alpha = atof(value.c_str());
    else if (param == "beta")
      m_est_beta = atof(value.c_str());
    else if (param == "tempnorth")
      m_est_tempnorth = atof(value.c_str());
    else if (param == "tempsouth")
      m_est_tempsouth = atof(value.c_str());
    else
      reportConfigWarning("Invalid spiral request for " + GetAppName());
  }

  // generate the vector aligned with the front
  XYVector front_vector;
  front_vector.setPosition(0,     m_est_offset);
  front_vector.setVectorMA(70, 90-m_est_angle);
  front_vector.setHeadSize(15);
  front_vector.set_vertex_size(5);
  front_vector.set_edge_size(4);
  front_vector.set_label("front_estimate");
  front_vector.set_edge_color("magenta");
  front_vector.set_vertex_color("magenta");

  // generate the vector aligned with the amplitude of the front
  XYVector amplitude_vector;
  amplitude_vector.setPosition(0,  m_est_offset);
  amplitude_vector.setVectorMA(m_est_amplitude, -m_est_angle);
  amplitude_vector.setHeadSize(5);
  amplitude_vector.set_vertex_size(4);
  amplitude_vector.set_edge_size(2);
  amplitude_vector.set_label("amplitude_estimate");
  amplitude_vector.set_edge_color("yellowgreen");
  amplitude_vector.set_vertex_color("yellowgreen");

  // adjust the loiter position depending upon the current parameters
  if (m_spr_active)
    m_loiter_offset += m_loiter_delta;
  m_loiter_radius  = max(min(m_est_amplitude, 50.0), 20.0);
  m_loiter_x = front_vector.xpos() + m_loiter_offset*cos(m_est_angle*PI/180.0);
  m_loiter_y = front_vector.ypos() + m_loiter_offset*sin(m_est_angle*PI/180.0);

  // adjust loiter delta parameter to keep the vehicle within the op region
  if (m_loiter_x > 130)
    m_loiter_delta = - abs(m_loiter_delta);
  if (m_loiter_x < -20) 
    m_loiter_delta = abs(m_loiter_delta);

  // plot the point
  XYPoint loiter_center;
  loiter_center.set_vertex(m_loiter_x, m_loiter_y);
  loiter_center.set_label("loiter_center");
  loiter_center.set_vertex_size(10);
  loiter_center.set_vertex_color("darkorange");

  // display the vector information 
  Notify("VIEW_VECTOR", front_vector.get_spec()); 
  Notify("VIEW_VECTOR", amplitude_vector.get_spec()); 
  Notify("VIEW_POINT",  loiter_center.get_spec());
  return(true);
}


//------------------------------------------------------------
// Procedure: handleLoiterReport()

bool SpiralPath::handleLoiterReport(string request_string)
{
  string index   = tokStringParse(request_string, "index", ',', '=');
  if (index == "0") {
    m_num_cycles++;
    // update the Loiter behavior after every loiter cycle 
    //  -> MOOS Variable: "SPIRAL_UPDATES" (publishes to BHV_Loiter)
    //  -> MOOS Value:    polygon = radial:: x=60, y=-105, radius=30, pts=10
    ostringstream loiter_os;
    loiter_os << "polygon = radial:: "
              << "x="       << m_loiter_x 
              << ",y="      << m_loiter_y 
              << ",radius=" << m_loiter_radius
              << ",pts=10"; 
    string loiter_str = loiter_os.str();

    // update the pEncircle app of the new loiter position
    //  -> MOOS Variable: "ENCIRCLE_POSITION"
    //  -> MOOS Value:    x=60,y=-105,radius=30
    ostringstream encircle_os;
    encircle_os << "x="       << m_loiter_x 
                << ",y="      << m_loiter_y 
                << ",radius=" << m_loiter_radius;
    string encircle_str = encircle_os.str();

    // only update after the radius has been perceived 
    if (m_loiter_radius > 0) {

      // hard code AUV that will serve as the decision maker
      //  -> this allows the two vehicles to follow the same loiter patterns
      if (m_captain == m_os_name){
        // send the Loiter and pEncircle updates to the captain
        Notify("SPIRAL_UPDATES", loiter_str);
        Notify("ENCIRCLE_POSITION", encircle_str);

        // send the Loiter update to the collaborator
        NodeMessage node_message_loiter;
        node_message_loiter.setSourceNode(m_os_name);
        node_message_loiter.setDestNode(m_collab_name);
        node_message_loiter.setVarName("SPIRAL_UPDATES");
        node_message_loiter.setStringVal(loiter_str);
        string msg_loiter = node_message_loiter.getSpec();
        Notify("NODE_MESSAGE_LOCAL", msg_loiter);

        // send the pEncircle update to the collaborator
        NodeMessage node_message_encircle;
        node_message_encircle.setSourceNode(m_os_name);
        node_message_encircle.setDestNode(m_collab_name);
        node_message_encircle.setVarName("ENCIRCLE_POSITION");
        node_message_encircle.setStringVal(encircle_str);
        string msg_encircle = node_message_encircle.getSpec();
        Notify("NODE_MESSAGE_LOCAL", msg_encircle);
      }
    }
  }
  return(true);
}


//---------------------------------------------------------
// Procedure: handleNodeReport()

bool SpiralPath::handleNodeReport(string report)
{
  m_total_node_reports++;
  
  // Part 1: Parse the node record and post warning if bad
  NodeRecord new_node_record = string2NodeRecord(report, true);
  string err_msg;
  bool ok = new_node_record.valid("name,x,y,speed,heading", err_msg);
  if(!ok) {
    reportRunWarning("Bad CN NodeReport: " + err_msg);
    return(false);
  }

  if(m_grp_filter != "") {
    if(m_grp_filter != tolower(new_node_record.getGroup()))
      return(true);
  }    

  string vname = new_node_record.getName();    
  m_collab_name= vname;    
  return(true);
}


//---------------------------------------------------------
// Procedure: handleNodeReportLocal()

bool SpiralPath::handleNodeReportLocal(string report)
{
  NodeRecord record = string2NodeRecord(report, true);

  string err_msg;
  bool ok = record.valid("name,x,y,speed,heading", err_msg);
  if(!ok) {
    reportRunWarning("Bad OS NodeReport: " + err_msg);
    return(false);
  }  
  m_os_name   = record.getName();
  m_os_tstamp = m_curr_time;
  return(true);
}


//------------------------------------------------------------
// Procedure: handleNavX()

bool SpiralPath::handleNavX(string sval)
{
  if(!isNumber(sval))
    return(false);
  m_osx = atof(sval.c_str());
  return(true);
}


//------------------------------------------------------------
// Procedure: handleNavY()

bool SpiralPath::handleNavY(string sval)
{
  if(!isNumber(sval))
    return(false);
  m_osy = atof(sval.c_str());
  return(true);
}


//------------------------------------------------------------
// Procedure: handleSpiralMode()

bool SpiralPath::handleSpiralMode(string sval)
{
  m_spr_active = true;
  return(true);
}


//------------------------------------------------------------
// Procedure: handleNewSensorReport()

bool SpiralPath::handleNewSensorReport(string sval)
{
  //Parse the message
  string report_vname = tokStringParse(sval, "vname", ',', '=');

  // Increase the num messages received if from collaborator
  if (report_vname == m_collab_name) {
    m_num_msgs_received++;
    return(true);
  }

  // Otherwise send the report message to the opposite vehicle
  else if (report_vname == m_os_name){
    //Build and  message
    NodeMessage node_message;
    node_message.setSourceNode(m_os_name); 
    node_message.setDestNode(m_collab_name);
    node_message.setVarName("UCTD_MSMNT_REPORT");
    node_message.setStringVal(sval);
    string msg = node_message.getSpec();
    m_num_msgs_sent++;
    Notify("NODE_MESSAGE_LOCAL", msg);
    return(true);
  }
  else
    return(false);
}


//---------------------------------------------------------
// Procedure: setLoiterDelta()

bool SpiralPath::setLoiterDelta(string str)
{
  if(!isNumber(str))
    return(false);
  
  double dval = atof(str.c_str());
  if(dval > 10)
    dval = 10;
  else if (dval < 0)
    dval = 0;

  m_loiter_delta = -abs(dval);
  return(true);
}


//---------------------------------------------------------
// Procedure: setCaptain()

bool SpiralPath::setCaptain(string str)
{
  if(isNumber(str))
    return(false);

  m_captain = str;
  return(true);
}


//---------------------------------------------------------
// Procedure: registerVariables

void SpiralPath::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("SPIRAL_REPORT_PARAMS",0);
  Register("UCTD_MSMNT_REPORT", 0);
  Register("LOITER_REPORT", 0);
  Register("NODE_REPORT", 0);
  Register("NODE_REPORT_LOCAL", 0);
  Register("MOVE_CENTER", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool SpiralPath::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "File: SpiralPath                            " << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "Message Info"                                 << endl;
  m_msgs << "  Captain:     " << m_captain                 << endl;
  m_msgs << "  Own Ship:    " << m_os_name                 << endl;
  m_msgs << "  Collab Ship: " << m_collab_name             << endl;
  m_msgs << "  # Estimates: " << m_num_estimate_rcd        << endl;
  m_msgs << "  # Msgs Sent: " << m_num_msgs_sent           << endl;
  m_msgs << "  # Msgs Recd: " << m_num_msgs_received       << endl;
  m_msgs << "--------------------------------------------" << endl;
  m_msgs << "Loiter Info"                                  << endl;
  m_msgs << "  Spiral Mode: " << m_spr_active               << endl;
  m_msgs << "  Num Cycles:  " << m_num_cycles              << endl;
  m_msgs << "  Position:    "
         << "<" << m_osx << "," << m_osy << ">"            << endl;
  m_msgs << "--------------------------------------------" << endl;
  m_msgs << "Parameter Estimation Info"                    << endl;
  m_msgs << "  vname:       " << m_est_vname               << endl; 
  m_msgs << "  offset:      " << m_est_offset              << endl; 
  m_msgs << "  angle:       " << m_est_angle               << endl; 
  m_msgs << "  amplitude:   " << m_est_amplitude           << endl; 
  m_msgs << "  period:      " << m_est_vname               << endl; 
  m_msgs << "  wavelength:  " << m_est_wavelength          << endl; 
  m_msgs << "  alpha:       " << m_est_alpha               << endl; 
  m_msgs << "  beta:        " << m_est_beta                << endl; 
  m_msgs << "  tempnorth:   " << m_est_tempnorth           << endl; 
  m_msgs << "  tempsouth:   " << m_est_tempsouth           << endl; 
  return(true);
}






