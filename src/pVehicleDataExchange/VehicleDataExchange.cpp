/************************************************************/
/*    NAME: appcasting                                              */
/*    ORGN: MIT                                             */
/*    FILE: VehicleDataExchange.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "VehicleDataExchange.h"
#include "NodeMessage.h"

using namespace std;

//---------------------------------------------------------
// Constructor

VehicleDataExchange::VehicleDataExchange()
{
  m_num_notified_msgs = 0;
  m_num_self_msgs=0;

}

//---------------------------------------------------------
// Destructor

VehicleDataExchange::~VehicleDataExchange()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool VehicleDataExchange::OnNewMail(MOOSMSG_LIST &NewMail)
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
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    std::cout<<"key: "<<key<<std::endl;
    if (key=="UCTD_MSMNT_REPORT"){
      //Parse
      string temp_vname = tokStringParse(sval, "vname", ',', '=');

      if (temp_vname==m_vname){ //If it is the same vehicle, send to the other vehicle
	m_dest_name = temp_vname;
	//Build and  message
	NodeMessage node_message;
	node_message.setSourceNode(m_vname);
	//betty vs. archie
	if (m_vname == "archie"){
	  m_dest_name = "betty";
        }
        else{
	  m_dest_name = "archie";
	}
	node_message.setDestNode(m_dest_name);
	node_message.setVarName(key);
	node_message.setStringVal(sval);

	string msg = node_message.getSpec();
	m_num_notified_msgs++;

	Notify("NODE_MESSAGE_LOCAL", msg);
      }
      else{
	m_num_self_msgs++;
      }
    }

  
     
      
     

     if(key == "FOO") 
       cout << "great!";

     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);

  }
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool VehicleDataExchange::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool VehicleDataExchange::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool VehicleDataExchange::OnStartUp()
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
    if(param == "vname") {
      m_vname = value;
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

void VehicleDataExchange::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
  Register("UCTD_MSMNT_REPORT",0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool VehicleDataExchange::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;
  
  m_msgs <<"Own vehicle name: "<<m_vname<<endl;
  m_msgs <<"Dest vehicle: "<<m_dest_name<<endl;

  m_msgs<< "Number of notifications to dest vehicle: "<<m_num_notified_msgs <<endl;
  m_msgs << "Number of temperature measurements to own vehicle: "<< m_num_self_msgs<< endl;

  return(true);
}




