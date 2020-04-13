/************************************************************/
/*    NAME: Kristen Railey                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_Pulse.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_Pulse.h"
#include "XYRangePulse.h"
#include <thread>
#include <chrono>
#include <cmath>
#include <iostream>

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_Pulse::BHV_Pulse(IvPDomain domain) :
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
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y, WPT_INDEX");
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_Pulse::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if((param == "pulse_range") && isNumber(val)) {
    // Set local member variables here
    m_range = double_val;
    //    return(true);
  }
  else if ((param == "pulse_duration") && isNumber(val)) {
    // return(setBooleanOnString(m_my_bool, val));
    m_pulse_duration = double_val;

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

void BHV_Pulse::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_Pulse::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_Pulse::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_Pulse::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_Pulse::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_Pulse::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_Pulse::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_Pulse::onRunState()
{
  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;
  
  //  m_curr_time = getBufferCurrTime();
 

  bool ok_x, ok_y, ok_wpt;
 
  //Check to see if waypoint changes
  m_wpt_index = getBufferDoubleVal("WPT_INDEX",ok_wpt);
  m_curr_time = getBufferCurrTime();

  if (m_wpt_index != m_wpt_index_curr){
    //New waypt, set pulse time
    m_pulse_time =m_curr_time+5.0;
    m_wpt_index_curr = m_wpt_index;
  }
  
  double time_diff = abs(m_pulse_time-m_curr_time);
  postMessage("TIME_DIFFERENCE",time_diff);
  if (abs(m_pulse_time -m_curr_time)<3.0){
    m_osx = getBufferDoubleVal("NAV_X",ok_x);
    m_osy = getBufferDoubleVal("NAV_Y",ok_y);
    sendPulse(m_pulse_time);
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

void BHV_Pulse::sendPulse(double time){
  //Get current x, y


  XYRangePulse pulse;
  pulse.set_x(m_osx);
  pulse.set_y(m_osy);
  pulse.set_label("bhv_pulse");
  pulse.set_rad(m_range);
  pulse.set_duration(m_pulse_duration);
  pulse.set_time(time);
  pulse.set_color("edge", "yellow");
  pulse.set_color("fill", "yellow");

  string spec = pulse.get_spec();
  postMessage("VIEW_RANGE_PULSE", spec);

}



