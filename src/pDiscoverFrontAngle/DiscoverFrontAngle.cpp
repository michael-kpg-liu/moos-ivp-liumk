/************************************************************/
/*    NAME: appcasting                                              */
/*    ORGN: MIT                                             */
/*    FILE: DiscoverFrontAngle.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "DiscoverFrontAngle.h"

using namespace std;

//---------------------------------------------------------
// Constructor

DiscoverFrontAngle::DiscoverFrontAngle()
{
  m_num_passes = 0;
}

//---------------------------------------------------------
// Destructor

DiscoverFrontAngle::~DiscoverFrontAngle()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool DiscoverFrontAngle::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval  = msg.GetString();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
    if (key=="LOITER_REPORT"){
      m_index = tokStringParse(sval, "index", ',', '=');
      if (m_index =="0"){
	m_num_passes++;
      }
    }
     if(key == "UCTD_MSMT_REPORT") 
       //parse
       //convert to double
       ;

     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool DiscoverFrontAngle::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool DiscoverFrontAngle::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool DiscoverFrontAngle::OnStartUp()
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
    else if(param == "bar") {
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void DiscoverFrontAngle::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
  Register("LOITER_REPORT",0);
  Register("UCTD_MSMT_REPORT",0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool DiscoverFrontAngle::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  m_msgs <<" Current index: "<<m_index<<endl;
  m_msgs <<"Number of times around circle: "<<m_num_passes<<endl;

  return(true);
}




