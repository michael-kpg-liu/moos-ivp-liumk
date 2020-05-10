/************************************************************/
/*    NAME: Mike Benjamin, Kyle Woerner                     */
/*    ORGN: MIT                                             */
/*    FILE: Encircle.cpp                                    */
/*    DATE: March 27th 2017                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "Encircle.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "CPAEngine.h"
#include "NodeRecordUtils.h"
#include "NodeMessage.h"
#include "NodeMessageUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Encircle::Encircle()
{
  // Configuration params
  m_center_x = 0;
  m_center_y = 0;
  m_radius   = 0;
  m_on_circle_thresh = 10;
  m_consider_thresh  = 100;
  
  m_stale_thresh = 60; // one minute
  m_aggression = 1; 

  m_speed_refresh_rate = 10;
  m_desired_group_speed = 2;

  m_msg_color = "dodger_blue";
  m_update_parameter = "LOITER_UPDATE";
  
  // State variables
  m_os_clockwise  = false;
  m_os_ctr_clock  = false;

  m_closest_fore_dist    = 0;
  m_closest_fore_gap_spd = 0;

  m_closest_aft_dist    = 0;
  m_closest_aft_gap_spd = 0;
  
  m_os_range_to_circle = -1;
  m_os_max_speed       = 4;
  m_os_name            = "unknown";
  m_os_tstamp          = 0;
  m_os_stale_thresh    = 2;
  
  m_total_node_reports = 0;

  m_encircle_active = false;

  m_max_time = MOOSTime();

  m_closest_fore_dist = 0;
  m_closest_aft_dist  = 0;

}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Encircle::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval   = msg.GetString(); 

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    bool handled = false;
    if(key == "NODE_REPORT")
      handled = handleMailNodeReport(sval);
    else if(key == "NODE_REPORT_LOCAL")
      handled = handleMailNodeReportLocal(sval);
    else if(key == "ENCIRCLE_POSITION") 
      handled = setCirclePosition(sval);
    else if(key == "ENCIRCLE_AGGRESSION") 
      handled = setAggression(sval);
    else if(key == "ENCIRCLE_MAXSPEED")
      handled = setMaxSpeedOS(sval);
    else if(key == "ENCIRCLE_ACTIVE")
      handled = setActiveState(sval);

    // handled by AppCastingMOOSApp
    else if(!handled || (key != "APPCAST_REQ")) 
      reportRunWarning("Unhandled Mail: " + key);
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Encircle::OnConnectToServer()
{
  registerVariables();
  return(true);
}



//---------------------------------------------------------
// Procedure: Iterate()

bool Encircle::Iterate()
{
  AppCastingMOOSApp::Iterate();

  checkForStaleContacts();
  
  bool ok = m_encircle_active;
  //============================================================
  // Part 1: Update the trajectories and determine if we should
  //         proceed with adjusting ownship speed.
  //         o Ownship position must be recently updated and on 
  //           circle and on a loitering heading.
  //         o At least one contact must be on the circle and
  //           on the same loitering heading as ownship.
  //============================================================
  ok = ok && updateTrajectoryStates();
  
  //============================================================
  // Part 2: Update state info regarding gaps to nearest fore/aft
  //         contacts
  //============================================================
  ok = ok && updateGapState();

  //============================================================
  // Part 3: Determine and post the appropriate adjustment speed
  //============================================================
  ok = ok && updateSpeed();

  //============================================================
  // Part 4: Perhaps publish equilibrium messages
  //============================================================
  ok = ok && postNodeMessages();
  
  //============================================================
  // Part 5: Publish info for graphing
  //============================================================
  ok = ok && postToGraph();
  
  AppCastingMOOSApp::PostReport();
  
  return(true);
}



//---------------------------------------------------------
// Procedure: OnStartUp()

bool Encircle::OnStartUp()
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
    if (param == "update_parameter")
      handled = setUpdateParameter(value);
    else if(param == "max_speed")
      handled = setMaxSpeedOS(value);
    else if(param == "aggression")
      handled = setAggression(value);
    else if(param == "circle_position")
      handled = setCirclePosition(value);
    else if(param == "on_circle_thresh")
      handled = setNonNegDoubleOnString(m_on_circle_thresh, value);
    else if(param == "consider_thresh")
      handled = setConsiderThresh(value);
    else if(param == "message_color")
      handled = setMessageColor(value);

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void Encircle::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("ENCIRCLE_POSITION", 0);
  Register("ENCIRCLE_AGGRESSION", 0);
  Register("ENCIRCLE_MAXSPEED", 0);
  Register("NODE_REPORT", 0);
  Register("NODE_REPORT_LOCAL", 0);
  Register("ENCIRCLE_ACTIVE", 0);
}


//---------------------------------------------------------
// Procedure: handleMailNodeReport()

bool Encircle::handleMailNodeReport(string report)
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

  // Part 2: If the contact is not known to us beforehand,
  //         determine if contact should be considered. 
  if(!m_map_node_records.count(vname)) {
    double range_to_circ = getRangeToCircle(new_node_record);
    // Use as thrash buffer, subtract N meters from consider_thresh
    // before accepting as new. Same is done when dropping contacts,
    // except 5 meters is added to the consider_thresh instead.
    double thrash_buffer = 5;
    if(range_to_circ > (m_consider_thresh - thrash_buffer))
      return(true);
  }
  
  m_map_node_records[vname] = new_node_record;
  m_map_node_count[vname]++;
  m_map_node_rng_to_os[vname] = -1;
  m_map_node_rng_to_circ[vname] = -1;
  m_map_node_on_circ[vname] = false;
  m_map_node_tstamp[vname]  = m_curr_time;
  m_map_node_arc_front[vname] = -1;
  m_map_node_arc_back[vname]  = -1;
  m_map_node_clockwise[vname] = false;
  m_map_node_ctr_clock[vname] = false;
  
  return(true);
}


//---------------------------------------------------------
// Procedure: handleMailNodeReportLocal()

bool Encircle::handleMailNodeReportLocal(string report)
{
  NodeRecord record = string2NodeRecord(report, true);

  string err_msg;
  bool ok = record.valid("name,x,y,speed,heading", err_msg);
  if(!ok) {
    reportRunWarning("Bad OS NodeReport: " + err_msg);
    return(false);
  }
  
  m_ownship = record;
  m_os_name = record.getName();
  //m_ownship.setName("ownship");

  m_os_tstamp = m_curr_time;
  return(true);
}


//---------------------------------------------------------
// Procedure: setAggression()

bool Encircle::setAggression(string str)
{
  if(!isNumber(str))
    return(false);
  double dval = atof(str.c_str());
  if(dval < 1)
    dval = 1;
  if(dval > 10)
    dval = 10;

  m_aggression = dval;
  return(true);
}


//---------------------------------------------------------
// Procedure: setConsiderThresh()

bool Encircle::setConsiderThresh(string str)
{
  if(!isNumber(str))
    return(false);
  
  double dval = atof(str.c_str());

  // This threshold must be at least 1.5x the on_circle_thresh
  if(dval < (m_on_circle_thresh * 1.5))
    dval = m_on_circle_thresh * 1.5;

  m_consider_thresh = dval;
  return(true);
}


//---------------------------------------------------------
// Procedure: setMessageColor()

bool Encircle::setMessageColor(string str)
{
  if(tolower(str) == "off") {
    m_msg_color = "off";
    return(true);
  }
  
  if(!isColor(str))
    return(false);

  m_msg_color = str;
  return(true);
}


//---------------------------------------------------------
// Procedure: setMaxSpeedOS()

bool Encircle::setMaxSpeedOS(string str)
{
  if(!isNumber(str))
    return(false);
  
  double dval = atof(str.c_str());
  if(dval < 0)
    dval = 0;

  m_os_max_speed = dval;
  return(true);
}


//---------------------------------------------------------
// Procedure: setActiveState()

bool Encircle::setActiveState(string str)
{
  if(!isBoolean(str))
    return(false);

  if(tolower(str) == "true") {
    m_encircle_active = true;
    return(true);
  }
  
  m_encircle_active = false;
  Notify(m_update_parameter, "use_alt_speed=false");
  
  return(true);
}


//---------------------------------------------------------
// Procedure: setCirclePosition()

bool Encircle::setCirclePosition(string str)
{
  bool x_set = false;
  bool y_set = false;
  bool rad_set = false;

  double center_x = 0;
  double center_y = 0;
  double radius = 0;
  
  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    double dval  = atof(value.c_str());
    
    if((param == "x") && isNumber(value)) {
      center_x = dval;
      x_set = true;
    }
    else if((param == "y") && isNumber(value)) {
      center_y = dval;
      y_set = true;
    }
    else if((param == "radius") && isNumber(value) && (dval > 0)) {
      radius = dval;
      rad_set = true;
    }
    else
      return(false);
  }

  if(!x_set || !y_set || !rad_set)
    return(false);

  m_center_x = center_x;
  m_center_y = center_y;
  m_radius = radius;
  
  return(true);
}


//---------------------------------------------------------
// Procedure: setUpdateParameter()

bool Encircle::setUpdateParameter(string str)
{
  if(isBoolean(str))
    return(false);
  if(isNumber(str))
    return(false);
  m_update_parameter = toupper(str);
  return(true);
}


//---------------------------------------------------------
// Procedure: checkForStaleContacts()
//   Purpose: Remove any contacts that haven't been updated
//            recently to guard against unbounded memory growth
//      Note: Will also remove contacts that have gotten out of
//            range, even if they are still updating recently.

void Encircle::checkForStaleContacts()
{  
  vector<string> contacts_to_remove;
  map<string, double>::iterator p;
  for(p=m_map_node_tstamp.begin(); p != m_map_node_tstamp.end(); p++) {
    string vname  = p->first;
    double tstamp = p->second;
    double age = m_curr_time - tstamp;
    if(age > m_stale_thresh) {
      contacts_to_remove.push_back(vname);
    }
    else { // check range to circle
      if(m_map_node_rng_to_circ[vname] > (m_consider_thresh+5))
	contacts_to_remove.push_back(vname);
    }
  }
  for(unsigned int i=0; i<contacts_to_remove.size(); i++) {
    string vname = contacts_to_remove[i];
    m_map_node_records.erase(vname);
    m_map_node_count.erase(vname);
    m_map_node_tstamp.erase(vname);
    m_map_node_rng_to_os.erase(vname);
    m_map_node_rng_to_circ.erase(vname);
    m_map_node_on_circ.erase(vname);
    m_map_node_arc_front.erase(vname);
    m_map_node_arc_back.erase(vname);
    m_map_node_clockwise.erase(vname);
    m_map_node_ctr_clock.erase(vname);
  }
}


//---------------------------------------------------------
// Procedure: updateTrajectoryStates()
//   Purpose: Update each vehicle w.r.t. on_circle range and bearing
//    Return: true if (a) ownship on circle (rng,bng) and at least
//            one other fresh contact

bool Encircle::updateTrajectoryStates()
{  
  // Part 1: Sanity check: If no ownship info found yet,
  //         return false now
  double os_age = m_curr_time - m_os_tstamp;
  if((m_os_tstamp == 0) || (os_age > 2)) {
    reportRunWarning("No or old ownship info");
    return(false);
  }
  
  // Part 2: Update the ownship trajectory state. If ownship is
  //         not on the circle, on a loiter trajectory, return now.
  bool os_on_circle = updateTrajectoryState(m_ownship);
  if(!os_on_circle)
    return(false);

  // Part 3: Update all the contact trajectory states. If there is
  //         not at least one contact on_circle (bng,rng) in the
  //         same direction as ownship, return now.
  unsigned int cn_on_circle_count = 0;
  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p != m_map_node_records.end(); p++) {
    bool cn_on_circle = updateTrajectoryState(p->second);
    if(cn_on_circle)
      cn_on_circle_count++;
  }
  if(cn_on_circle_count == 0)
    return(false);
  
  // Part 4: For all on_circle contacts, update distances from ownship
  for(p=m_map_node_records.begin(); p != m_map_node_records.end(); p++) {
    string vname = p->first;
    if(m_map_node_on_circ[vname]) {
      updateDirectRange(p->second);
      updateArcDistance(p->second);
    }
  }

  return(true);
}


//---------------------------------------------------------
// Procedure: updateTrajectoryState()
//   Purpose: Examine the trajectory of given vehicle record
//            Determine if can be considered to be currently on
//            a clockwise or counter_clockwise trajectory.
//            Range to the circle is also a consideration.

bool Encircle::updateTrajectoryState(NodeRecord record)
{
  cout << "============================================" << endl;
  cout << "In updateTrajState(), vname: " << record.getName() << endl;

  // Part 1: Check if dist-to-circle criteria is met
  double range_to_circ = getRangeToCircle(record);
  bool   on_circle = true;
  if(range_to_circ > m_on_circle_thresh) 
    on_circle = false;

  cout << "  on_circle: " << on_circle << endl;
  
  // Part 2: Check this can be considered clockwise or ctr_clockwise
  bool clockwise = false;
  bool ctr_clock = false;

  if(on_circle) {
    double posx = record.getX();
    double posy = record.getY();
    double posh = record.getHeading();
    
    double ship_to_center_rel_bng = relBearing(posx, posy, posh,
					       m_center_x, m_center_y);

    cout << "  rel_bng: " << ship_to_center_rel_bng << endl;
    if((ship_to_center_rel_bng >= 70) &&
       (ship_to_center_rel_bng <= 110))
      clockwise = true;
    
    if((ship_to_center_rel_bng >= 250) &&
       (ship_to_center_rel_bng <= 290))
      ctr_clock = true;
  }

  string vname = record.getName();

  // Part 3: Store results for ownship
  if(vname == m_os_name) {
    m_os_clockwise = clockwise;
    cout << "  ownship clockwise: " << clockwise << endl;
    m_os_ctr_clock = ctr_clock;
    cout << "  ownship ctr_clock: " << ctr_clock << endl;
    m_os_range_to_circle = range_to_circ;
    if(clockwise || ctr_clock)
      return(true);
    return(false);
  }

  // Part 4: Store resulst for contact
  m_map_node_rng_to_circ[vname] = range_to_circ;
  m_map_node_clockwise[vname] = clockwise;
  m_map_node_ctr_clock[vname] = ctr_clock;
  m_map_node_on_circ[vname]   = true; // confirm below

  if(clockwise && m_os_clockwise)
    return(true);
  if(ctr_clock && m_os_ctr_clock) 
    return(true);
    
  m_map_node_on_circ[vname] = false; // not in same direction
  return(false);
}


//---------------------------------------------------------
// Procedure: getRangeToCircle()

double Encircle::getRangeToCircle(NodeRecord record)
{
  double posx = record.getX();
  double posy = record.getY();

  double range_to_center = distPointToPoint(posx, posy,
					    m_center_x, m_center_y);
  double range_to_circle = m_radius - range_to_center;

  if(range_to_circle < 0)
    range_to_circle *= -1;

  return(range_to_circle);
}


//---------------------------------------------------------
// Procedure: updateDirectRange()

void Encircle::updateDirectRange(NodeRecord record)
{
  // Sanity check - if ownship position not set yet, return 
  if(m_ownship.getName() != m_os_name)
    return;

  string vname = record.getName();
  double osx = m_ownship.getX();
  double osy = m_ownship.getY();
  double cnx = record.getX();
  double cny = record.getY();

  double range = hypot(osx-cnx, osy-cny);
  m_map_node_rng_to_os[vname] = range;
}

  
//---------------------------------------------------------
// Procedure: updateArcDistance()

void Encircle::updateArcDistance(NodeRecord record)
{
  string vname = record.getName();
  m_map_node_arc_front[vname] = -1;
  m_map_node_arc_back[vname]  = -1;
  
  // Sanity check - if ownship position not set yet, return 
  if(m_ownship.getName() != m_os_name)
    return;

  // Sanity check - ownship should either be on a clockwise or
  // counter clockwise trajectory
  if(!m_os_clockwise && !m_os_ctr_clock)
    return;

  // Sanity check - ownship and contact should have matching
  // trajectory directions
  if(m_os_clockwise && !m_map_node_clockwise[vname])
    return;
  if(m_os_ctr_clock && !m_map_node_ctr_clock[vname])
    return;

  // Part 1: Determine point on the circle closest to ownship
  double osx = m_ownship.getX();
  double osy = m_ownship.getY();
  double os_to_ctr_rel_ang = relAng(osx, osy, m_center_x, m_center_y);
  double os_cx, os_cy;
  projectPoint(os_to_ctr_rel_ang, m_radius, m_center_x, m_center_y,
	       os_cx, os_cy);

  // Part 2: Determine point on the circle closest to contact
  double cnx = record.getX();
  double cny = record.getY();

  double cn_to_ctr_rel_ang = relAng(cnx, cny, m_center_x, m_center_y);
  double cn_cx, cn_cy;
  projectPoint(cn_to_ctr_rel_ang, m_radius, m_center_x, m_center_y,
	       cn_cx, cn_cy);

  // Part 3: Determine the angle between ownship, contact, center
  //double angle = angleFromThreePoints(os_cx, os_cy, m_center_x,
  //				      m_center_y, cn_cx, cn_cy);
  double angle = angleFromThreePoints(m_center_x, m_center_y, os_cx,
				      os_cy, cn_cx, cn_cy);
  angle = angle360(angle);

  // Part 4: Determine the long and short arcs of the circle
  double circum = 3.1415926 * 2 * m_radius;
  
  double dist1 = (angle/360) * circum;
  double dist2 = circum - dist1;
  double short_dist = dist1;
  double long_dist = dist2;
  if(short_dist > long_dist) {
    short_dist = dist2;
    long_dist = dist1;
  }
  
  // Part 5: Determine if contact is tailing ownship or vice versa

  // Dont use actual contact heading, but rather the heading
  // tangential to the circle depending on direction. This way
  // determinations of contact fore/aft won't be subject to
  // thrashing in edge-case situations.

  double cnv = record.getSpeed();
  double cnh = angle360(cn_to_ctr_rel_ang - 90);
  if(m_map_node_ctr_clock[vname]) 
    cnh = angle360(cn_to_ctr_rel_ang + 90);

  CPAEngine cpa_engine(cny, cnx, cnh, cnv, osy, osx);
  if(cpa_engine.aftOfContact()) {
    m_map_node_arc_front[vname] = short_dist;
    m_map_node_arc_back[vname]  = long_dist;
  }
  else {
    m_map_node_arc_front[vname] = long_dist;
    m_map_node_arc_back[vname]  = short_dist;
  }
}


//---------------------------------------------------------
// Procedure: updateGapState()
//   Returns: true if a fore and aft vehicle are found, satisfying
//            the on_circle (rng,bng) criteria.

bool Encircle::updateGapState()
{
  double osv = m_ownship.getSpeed();

  //=============================================================
  // Part 1: Handle the closest fore vehicle
  //=============================================================

  // Part 1A: Determine which vehicle is closest fore
  m_closest_fore_name = "";
  m_closest_fore_dist = 0;
  map<string, double>::iterator p;
  for(p=m_map_node_arc_front.begin(); p!=m_map_node_arc_front.end(); p++) {
    string vname = p->first;
    double dist  = p->second;
    if(m_map_node_on_circ[vname]) {
      if((m_closest_fore_name == "") || (dist < m_closest_fore_dist)) {
	m_closest_fore_name = vname;
	m_closest_fore_dist = dist;
      }
    }
  }
  if(m_closest_fore_name == "")
    return(false);

  // Part 1B: Determine relative speed to closest fore vehicle.
  NodeRecord fore_record = m_map_node_records[m_closest_fore_name];
  double closest_fore_spd  = fore_record.getSpeed();
  m_closest_fore_gap_spd = closest_fore_spd - osv ;


  //=============================================================
  // Part 2: Handle the closest fore vehicle
  //=============================================================

  // Part 2A: Determine which vehicle is closest aft
  m_closest_aft_name = "";
  m_closest_aft_dist = 0;
  
  map<string, double>::iterator q;
  for(q=m_map_node_arc_back.begin(); q!=m_map_node_arc_back.end(); q++) {
    string vname = q->first;
    double dist  = q->second;
    if(m_map_node_on_circ[vname]) {
      if((m_closest_aft_name == "") || (dist < m_closest_aft_dist)) {
	m_closest_aft_name = vname;
	m_closest_aft_dist = dist;
      }
    }
  }
  if(m_closest_aft_name == "")
    return(false);

  // Part 2B: Determine relative speed to closest aft vehicle.
  NodeRecord aft_record = m_map_node_records[m_closest_aft_name];
  double closest_aft_spd  = aft_record.getSpeed();
  m_closest_aft_gap_spd = osv - closest_aft_spd;  

  return(true);
}


//------------------------------------------------------------
// Procedure: updateSpeed()

bool Encircle::updateSpeed()
{
  // Sanity check
  if((m_closest_fore_dist <= 0) || (m_closest_aft_dist <= 0))
    return(false);

  // Part 1: Calculate the speed_factor always in range [-1, 1]
  double numerator  = m_closest_fore_dist - m_closest_aft_dist;
  double denominator = m_closest_fore_dist + m_closest_aft_dist;
  double speed_factor = numerator / denominator;
  double osv = m_ownship.getSpeed();

  double m_os_new_speed = osv;

#if 0
  
  // Part 2A: Handle the speed UP case
  if(speed_factor > 0) {
    // sanity check - if already maxed out, do nothing
    if(osv >= m_os_max_speed)
      return(false);
    double delta = m_os_max_speed - m_desired_group_speed;
    m_os_new_speed += (delta * speed_factor);
  }
  // Part 2B: Handle the speed UP case
  else {
    // sanity check - if ownship already stopped, do nothing
    if(osv <= 0)
      return(false);
    double delta = m_desired_group_speed;
    m_os_new_speed += (delta * speed_factor);
  }
#endif

    
#if 1
  // adjust new_speed to use aggression factor
  //speed_factor *= m_aggression;

  m_os_new_speed = m_desired_group_speed * (1 + speed_factor);
#endif

  if((speed_factor < 0.05) && (speed_factor > -0.05))
    m_os_new_speed = m_desired_group_speed;

  
  string str = "speed_alt=" + doubleToString(m_os_new_speed,1);
  str += "# use_alt_speed=true";

  if(m_encircle_active){
    Notify(m_update_parameter,str);
    Notify("ENC_RNG_TO_CIRC",doubleToString(m_os_range_to_circle,2));
    Notify("ENC_SPD_FACTOR",doubleToString(speed_factor,2));
    Notify("ENC_DIST_FORE",doubleToString(m_closest_fore_dist,2));
    Notify("ENC_DIST_AFT",doubleToString(m_closest_aft_dist,2));
  }

  return(true);
}


//------------------------------------------------------------
// Procedure: postNodeMessages()

bool Encircle::postNodeMessages()
{
  if(m_msg_color == "off")
    return(true);
  
  double avg = (m_closest_fore_dist + m_closest_aft_dist) / 2;
  double delta = m_closest_fore_dist - m_closest_aft_dist;
  if(delta < 0)
    delta *= -1;

  if((delta / avg) > 0.10)
    return(false);

  NodeMessage node_message_fore;
  node_message_fore.setSourceNode(m_os_name);
  node_message_fore.setDestNode(m_closest_fore_name);
  node_message_fore.setVarName("HELLO");
  node_message_fore.setStringVal("World");
  if(m_msg_color != "off")
    node_message_fore.setColor(m_msg_color);
  
  string msg_fore = node_message_fore.getSpec();
  Notify("NODE_MESSAGE_LOCAL", msg_fore); 

  NodeMessage node_message_aft;
  node_message_aft.setSourceNode(m_os_name);
  node_message_aft.setDestNode(m_closest_aft_name);
  node_message_aft.setVarName("HELLO");
  node_message_aft.setStringVal("World");
  if(m_msg_color != "off")
    node_message_aft.setColor(m_msg_color);

  string msg_aft = node_message_aft.getSpec();
  Notify("NODE_MESSAGE_LOCAL", msg_aft); 
  
  return(true);
}


//------------------------------------------------------------
// Procedure: postToGraph()

bool Encircle::postToGraph()
{
  if(m_closest_fore_name == "" || m_closest_aft_name == "")
    return false;

  double err = m_closest_fore_dist - m_closest_aft_dist;
  if(err < 0)
    err *= -1;

  Notify("ENCIRCLE_DATA_DIST_ERROR", err);
  return true;
}


//------------------------------------------------------------
// Procedure: buildReport()

bool Encircle::buildReport() 
{
  string grp_spd_str = doubleToStringX(m_desired_group_speed,1);
  string max_spd_str = doubleToStringX(m_os_max_speed,1);
  m_msgs << "Config:         " << endl;
  m_msgs << "-----------------" << endl;
  m_msgs << "  update:    " << m_update_parameter << endl;
  m_msgs << "  center_x:  " << m_center_x << endl;
  m_msgs << "  center_y:  " << m_center_y << endl;
  m_msgs << "  radius:    " << m_radius << endl;
  m_msgs << "  group spd: " << grp_spd_str << endl;
  m_msgs << "  os_max_speed:     " << max_spd_str << endl;
  m_msgs << "  on_circle_thresh: " << m_on_circle_thresh << endl;
  m_msgs << "  consider_thresh:  " << m_consider_thresh << endl;
  m_msgs << endl;  

  string active_str  = boolToString(m_encircle_active);
  string os_rng_str  = doubleToStringX(m_os_range_to_circle,1);
  string clockwise_str  = boolToString(m_os_clockwise);
  string ctr_clock_str  = boolToString(m_os_ctr_clock);
  if(!m_encircle_active) {
    clockwise_str = "-";
    ctr_clock_str = "-";
  }
  m_msgs << "General State:   " << endl;
  m_msgs << "-----------------" << endl;
  m_msgs << "  activated:      " << active_str << endl;
  m_msgs << "  reports:        " << m_total_node_reports << endl;
  m_msgs << "  os_clockwise:   " << clockwise_str << endl;  
  m_msgs << "  os_ctr_clock:   " << ctr_clock_str << endl;  
  m_msgs << "  os_rng_to_circ: " << os_rng_str << endl;
  m_msgs << endl;

  string fore_gap_spd = doubleToStringX(m_closest_fore_gap_spd,1);
  string aft_gap_spd  = doubleToStringX(m_closest_aft_gap_spd,1);
  m_msgs << "Closest:         " << endl;
  m_msgs << "-----------------" << endl;
  m_msgs << "closest_fore: " << m_closest_fore_name << endl;
  m_msgs << "closest_aft:  " << m_closest_aft_name  << endl;
  m_msgs << "closest_fore_gap_spd: " << fore_gap_spd << endl;
  m_msgs << "closest_aft_gap_spd:  " << aft_gap_spd  << endl;
  m_msgs << endl;  

  ACTable actab(7,2);
  actab.setColumnJustify(1, "right");
  actab << "Contact |  On  | RangeTo | Reps  | Rng | Rng  | Clock";
  actab << "        | Circ | Ownship | Total | Aft | Fore | -wise";
  actab.addHeaderLines();

  map<string, NodeRecord>::iterator q;
  for(q=m_map_node_records.begin(); q!=m_map_node_records.end(); q++) {
    string vname = q->first;
    string on_circ = boolToString(m_map_node_on_circ[vname]);
    string range = doubleToStringX(m_map_node_rng_to_os[vname],1);
    string reports  = uintToString(m_map_node_count[vname]);
    string rng_fore = doubleToStringX(m_map_node_arc_front[vname],1);
    string rng_back = doubleToStringX(m_map_node_arc_back[vname],1);

    string traj = "-";
    if(m_map_node_clockwise[vname])
      traj = "yes";
    if(m_map_node_ctr_clock[vname])
      traj = "ctr";

    string nspeed = "N/A";
    //    m_os_new_speed = m_ownship.getSpeed();
    if(m_encircle_active && updateSpeed()){
      nspeed = doubleToString(m_os_new_speed,1);
    } 
    actab << vname << on_circ << range << reports;
    actab << rng_back << rng_fore << traj;
  }
  m_msgs << actab.getFormattedString() << endl << endl;
  

  m_msgs << "closest fore: " << m_closest_fore_name <<
    " --> " << m_closest_fore_dist << endl;

  m_msgs << "closest aft: " << m_closest_aft_name <<
    " --> " << m_closest_aft_dist << endl;

  m_msgs << "speed (curr) " << doubleToString(m_ownship.getSpeed(),1) <<
    " --> (to encircle) " << m_os_new_speed  << endl;
  m_msgs << endl;
  m_msgs << "time remaining before next calculation: " << doubleToString(m_max_time - MOOSTime(),1) << endl;
  m_msgs << endl;

  return(true);
}

