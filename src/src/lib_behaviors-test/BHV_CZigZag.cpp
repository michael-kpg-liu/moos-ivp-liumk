/************************************************************/
/*    NAME: Rui Chen                                        */
/*    ORGN: MIT                                             */
/*    FILE: BHV_CZigZag.cpp                                 */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_CZigZag.h"
#include <math.h>

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_CZigZag::BHV_CZigZag(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Default values for configuration parameters 
  m_desired_speed  = 0; 
  m_heading = 270;
  m_zlength = 100;
  m_decay_factor = 0.5;
  //m_stagger_width = 10;
  m_num_cycles = 0;
  thres = -999;


  // Default values for behavior state variables
  prev_x = 0;
  prev_y = 0;
  current_x = 0;
  current_y = 0;
  prev_temp = -999;
  current_temp = -999;
  estimate_report = "";
  fronty = -999;
  crossed_front = false;
  prev_tgrad = -999;
  tgrad = -999;
  survey_stage = true;
  min_temp = 0;
  max_temp = 0;

  // Add any variables this behavior needs to subscribe for
  addInfoVars("UCTD_MSMNT_REPORT");

}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_CZigZag::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if((param == "heading_init") && isNumber(val)) {
    m_heading = double_val;
  }
   else if((param == "zigzag_length") && isNumber(val)) {
    m_zlength = double_val;
  }
   else if((param == "decay_factor") && isNumber(val)) {
    m_decay_factor = double_val;
  }
  else if((param == "speed") && isNumber(val)) {
    m_desired_speed = double_val;
  }
  else if((param == "cycles_thres") && isNumber(val)) {
    m_cycles_thres = double_val;
  }
  else if((param == "zig_heading") && isNumber(val)) {
    m_zig_heading = double_val;
  }
  else if((param == "zag_heading") && isNumber(val)) {
    m_zag_heading = double_val;
  }
  else if((param == "up_bound") && isNumber(val)) {
    m_up_bound = double_val;
  }
  else if((param == "low_bound") && isNumber(val)) {
    m_low_bound = double_val;
  }

  // else if((param == "grad_thres") && isNumber(val)) {
  //   thres = double_val;
  // }

  else{
    return(false);
  }

  return(true);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_CZigZag::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_CZigZag::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_CZigZag::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_CZigZag::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_CZigZag::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_CZigZag::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_CZigZag::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_CZigZag::onRunState()
{
  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;

  bool ok1;

  estimate_report = getBufferStringVal("UCTD_MSMNT_REPORT",ok1);
  if(ok1){

    if(survey_stage){
      postBoolMessage("SURVEY_STAGE",true);
      postBoolMessage("ESTIMATION_STAGE",false);
      bool handled = handleSurveyReport(estimate_report);
      if(!handled){
        postWMessage("Unable to process UCTD Report during survey!");
      }
    }

    else{
      postBoolMessage("SURVEY_STAGE",false);
      postBoolMessage("ESTIMATION_STAGE",true);
      bool handled = handleSensingReport(estimate_report);
      if(!handled){
        postWMessage("Unable to process UCTD Report during estimation!");
      }

      if(current_y>m_up_bound){
        m_heading = -180;
      }
      if(current_y<m_low_bound){
        m_heading = 0;
      }
    }

  }

  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.

  ipf = buildFunctionWithZAIC();

  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}


//-----------------------------------------------------------
// Procedure: buildFunctionWithZAIC

IvPFunction *BHV_CZigZag::buildFunctionWithZAIC() 
{
  ZAIC_PEAK spd_zaic(m_domain, "speed");
  spd_zaic.setSummit(m_desired_speed);
  spd_zaic.setPeakWidth(0.5);
  spd_zaic.setBaseWidth(1.0);
  spd_zaic.setSummitDelta(0.8);  
  if(spd_zaic.stateOK() == false) {
    string warnings = "Speed ZAIC problems " + spd_zaic.getWarnings();
    postWMessage(warnings);
    return(0);
  }
  
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(m_heading);
  crs_zaic.setPeakWidth(0);
  crs_zaic.setBaseWidth(180.0);
  crs_zaic.setSummitDelta(0);  
  crs_zaic.setValueWrap(true);
  if(crs_zaic.stateOK() == false) {
    string warnings = "Course ZAIC problems " + crs_zaic.getWarnings();
    postWMessage(warnings);
    return(0);
  }

  IvPFunction *spd_ipf = spd_zaic.extractIvPFunction();
  IvPFunction *crs_ipf = crs_zaic.extractIvPFunction();

  OF_Coupler coupler;
  IvPFunction *ivp_function = coupler.couple(crs_ipf, spd_ipf, 50, 50);

  return(ivp_function);
}

//-----------------------------------------------------------
// Procedure: handleSensingReport

bool BHV_CZigZag::handleSensingReport(const string& request)
{
  //Parse the string report
  string vname;
  vector<string> svector = parseString(request, ',');
  unsigned int i, vsize = svector.size();
  std::string s;
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "vname"){
      vname = value;
    }
    else if (param == "x"){
      prev_x = current_x;
      current_x = atof(value.c_str());
    }
    else if (param == "y"){
      prev_y = current_y;
      current_y = atof(value.c_str());
    }
    else if (param == "temp"){
      prev_temp = current_temp;
      current_temp = atof(value.c_str());
    }
  }

  //Estimate temp gradient
  postMessage("CYCLE",m_num_cycles);

  if (prev_temp != -999){

    //prev_tgrad  = tgrad;
    double dist = pow((pow((current_y-prev_y),2) + pow((current_x-prev_x),2)),0.5);
    tgrad = (current_temp-prev_temp)/dist;
    //postWMessage("grad = "+to_string(tgrad));
    //postWMessage("grad_prev = "+to_string(prev_tgrad));
    //postWMessage("grad_div = "+to_string(tgrad/prev_tgrad));
    if (abs(tgrad)>thres && !crossed_front && abs(tgrad)<0.3){
      postMessage("TEMP_GRADIENT", tgrad);

      //add point to max gradient list
      //xgradlist.push_back(prev_x);
      //ygradlist.push_back(prev_y);

      //frontx = prev_x;
      fronty = prev_y;
      //postWMessage("fronty = " +to_string(fronty));
      //post to pMarineViewer
      XYPoint pt(prev_x,prev_y);
      pt.set_label("front");
      pt.set_vertex_color("white");
      pt.set_edge_color("white");
      pt.set_active(true);
      string spec = pt.get_spec();
      postMessage("VIEW_POINT",spec);

      crossed_front = true;
    }

    //postWMessage("currenty = " +to_string(current_y));
    // if(crossed_front){
    //   //postWMessage("crossed front!");
    //   double diff = abs(fronty-current_y);
    //   postWMessage("diff="+to_string(diff));
    //   //postWMessage("zlength="+to_string(m_zlength));
    //   postWMessage("fronty = " +to_string(fronty));
    //   postWMessage("currenty="+to_string(current_y));
    // }

    double diff = abs(fronty-current_y);
    //change vehicle heading if crossed front
    if (m_num_cycles < floor(m_cycles_thres/2)){
      if ((crossed_front) && (diff>=m_zlength)){
        if((m_heading>=90) && (m_heading<=270)){
          m_heading = m_zig_heading;
          m_num_cycles = m_num_cycles+1;
        }

        else{
          m_heading = m_zag_heading;
          m_num_cycles = m_num_cycles+1;
        }
        m_zlength = m_decay_factor*m_zlength;
        crossed_front=false;
        //postWMessage("new_heading = " +to_string(m_heading));
      }
    }

    if ((m_num_cycles)>=floor(m_cycles_thres/2) && (m_num_cycles)<=m_cycles_thres){
      if ((crossed_front) && (diff>=m_zlength)){
        if((m_heading>=90) && (m_heading<=270)){
          if(m_heading<180){
            m_heading = m_zag_heading+180;
          }
          else{
            m_heading = m_zag_heading-180;
          }
          m_num_cycles = m_num_cycles+1;
        }

        else{
          if(m_heading<90){
            m_heading = m_zig_heading+180;
          }
          else{
            m_heading = m_zig_heading-180;
          }
          m_num_cycles = m_num_cycles+1;
        }
        crossed_front=false;

      }
    }

    if ((m_num_cycles)>m_cycles_thres){
      setComplete();
    }
  }

  return(true);
}


//-----------------------------------------------------------
// Procedure: handleSurveyReport

bool BHV_CZigZag::handleSurveyReport(const string& request)
{
  //Parse the string report
  string vname;
  vector<string> svector = parseString(request, ',');
  unsigned int i, vsize = svector.size();
  std::string s;
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "vname"){
      vname = value;
    }
    else if (param == "x"){
      prev_x = current_x;
      current_x = atof(value.c_str());
    }
    else if (param == "y"){
      prev_y = current_y;
      current_y = atof(value.c_str());
    }
    else if (param == "temp"){
      prev_temp = current_temp;
      current_temp = atof(value.c_str());
    }
  }

  //postWMessage("curr_y = " + to_string(current_y));

  if(current_y>m_up_bound-10){
    min_temp = current_temp;
  }

  if(current_y<m_low_bound+10){
    max_temp = current_temp;
    double t_diff = abs(max_temp-min_temp);
    //postWMessage("max_temp = " + to_string(max_temp));
    //postWMessage("min_temp = " + to_string(min_temp));

    postMessage("TEMP_DIFF",t_diff);

    if(t_diff<=2){
      thres = 0.04;
    }

    else if((t_diff>2) && (t_diff <3)){
      thres = 0.045;
    }

    else if((t_diff>=3) && (t_diff <4)){
      thres = 0.06;
    }

    else if((t_diff>=4) && (t_diff <5)){
      thres = 0.07;
    }

    else if((t_diff>=5) && (t_diff <6)){
      thres = 0.08;
    }

    else if((t_diff>=6) && (t_diff <8)){
      thres = 0.09;
    }

    else if((t_diff>=8)){
      thres = 0.1;
    }

    survey_stage = false;
    m_heading = 0;
    postMessage("GRADIENT_THRESHOLD",thres);

  }
  return(true);
}





