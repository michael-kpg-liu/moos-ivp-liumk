/************************************************************/
/*    NAME: Kristen Railey                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_ZigLeg.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_ZigLeg.h"
#include "XYRangePulse.h"
#include <thread>
#include <chrono>
#include <cmath>
#include <iostream>
#include "ZAIC_PEAK.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_ZigLeg::BHV_ZigLeg(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Behavior variables - default values
  m_osx = 15.0;
  m_osy = -45.0;
  m_range = 40.0;
  m_pulse_duration = 4;
  m_wpt_index = 0.0;
  m_wpt_index_curr = 0.0;
  m_pulse_time=0.0;
  m_curr_time = 0.0;
  m_heading = 0.0;
  m_zig_angle = 45.0;
  m_zig_duration = 10.0;
  m_temp_int=0;
  m_past_waypt = false;

  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y, WPT_INDEX, NAV_HEADING");
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_ZigLeg::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if((param == "zig_duration") && isNumber(val)) {
    // Set local member variables herez
    m_zig_duration = double_val;
    //    return(true);
  }
  else if ((param == "zig_angle") && isNumber(val)) {
    // return(setBooleanOnString(m_my_bool, val));
    m_zig_angle = double_val;

  }
  else
    return (false);
  // If not handled above, then just return false;
  return(true);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_ZigLeg::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_ZigLeg::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_ZigLeg::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_ZigLeg::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_ZigLeg::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_ZigLeg::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_ZigLeg::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_ZigLeg::onRunState()
{
  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;
  
  bool ok_x, ok_y, ok_wpt, ok_hdg;
  
  postMessage("PAST_WPT",m_past_waypt);
  //Check to see if waypoint changes
  m_wpt_index = getBufferDoubleVal("WPT_INDEX",ok_wpt);
  m_curr_time = getBufferCurrTime();
  
  if (m_wpt_index != m_wpt_index_curr){
    //New waypt, set pulse time
    m_pulse_time =m_curr_time+5.0;
    m_wpt_index_curr = m_wpt_index;
    m_past_waypt=false; //Just arrived at new waypt
    m_temp_int = 0;
 }
  
  double time_diff = abs(m_pulse_time-m_curr_time);
  postMessage("TIME_DIFFERENCE",time_diff);
  
  if ((abs(m_pulse_time -m_curr_time)<1.0)&&(!m_past_waypt)){
    m_past_waypt = true;
    m_osx = getBufferDoubleVal("NAV_X",ok_x);
    m_osy = getBufferDoubleVal("NAV_Y",ok_y);
    m_heading = getBufferDoubleVal("NAV_HEADING",ok_hdg);
    postMessage("VEH_HEADING",m_heading);
    postMessage("NUM_POST",m_temp_int);
    m_temp_int++;
    // sendPulse(m_pulse_time);
    //Change ivp function
    /* ZAIC_PEAK zaic_peak(m_domain, "course");
    zaic_peak.setSummit(45);
    zaic_peak.setMinMaxUtil(20,120);
    zaic_peak.setBaseWidth(20);
    ipf = zaic_peak.extractIvPFunction();
    m_priority_wt = 90;*/
  }



  if ((m_past_waypt)&&(abs(m_pulse_time - m_curr_time)<m_zig_duration)){
    //Change ivp function              
    postMessage("SENT_NEW_ZAIC","true");
    ZAIC_PEAK zaic_peak(m_domain, "course");
    zaic_peak.setSummit(m_zig_angle+m_heading);
    zaic_peak.setMinMaxUtil(20,120);
    zaic_peak.setBaseWidth(20);
    ipf = zaic_peak.extractIvPFunction();
    m_priority_wt = 90;


  }
  else{
    postMessage("SENT_NEW_ZAIC","false");
    m_priority_wt = 10;
  }
  //std::this_thread::sleep_for(std::chrono::milliseconds(200));

  //sendPulse(m_curr_time);
  //std::this_thread::sleep_for(std::chrono::milliseconds(200));
  
  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

//--------------------------------------------------------------

void BHV_ZigLeg::sendPulse(double time){
  //Get current x, y


  XYRangePulse pulse;
  pulse.set_x(m_osx);
  pulse.set_y(m_osy);
  pulse.set_label("BHV_ZigLeg");
  pulse.set_rad(m_range);
  pulse.set_duration(m_pulse_duration);
  pulse.set_time(time);
  pulse.set_color("edge", "yellow");
  pulse.set_color("fill", "yellow");

  string spec = pulse.get_spec();
  postMessage("VIEW_RANGE_PULSE", spec);

}



