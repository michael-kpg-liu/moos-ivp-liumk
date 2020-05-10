/************************************************************/
/*    NAME: ruic                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.cpp                                        */
/*    DATE: Feb 20 2020                                         */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PrimeFactor.h"
#include <string>
#include <cstdlib>
#include <cstdint>

using namespace std;

//---------------------------------------------------------
// Constructor

PrimeFactor::PrimeFactor()
{
  m_index=0;
  m_cal_index=0;
}

//---------------------------------------------------------
// Destructor

PrimeFactor::~PrimeFactor()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PrimeFactor::OnNewMail(MOOSMSG_LIST &NewMail)
{

  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey();

    if (key == "NUM_VALUE"){
      
      PrimeEntry Ent;
      string mystr = msg.GetString();
      uint64_t val = strtoul(mystr.c_str(),NULL,0); //convert string input into unsigned long int

      Ent.setOriginalVal(val);
      Ent.setReceivedIndex(m_index);

      m_input_list.push_back(Ent);

      m_index = m_index + 1;

    }

   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PrimeFactor::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
// happens AppTick times per second

bool PrimeFactor::Iterate()
{

  list<PrimeEntry>::iterator p;
  for(p=m_input_list.begin(); p!=m_input_list.end(); p++) {

    unsigned long int max_steps = 100000; // set max iter count to prevent blocking
    
    p->factor(max_steps); //find factors

    if (p->done()){ //publish results if done 
      p->setCalculatedIndex(m_cal_index);
      Notify("PRIME_RESULT",p->getReport());
      m_cal_index = m_cal_index+1;
      p = m_input_list.erase(p);
    }

	}
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
// happens before connection is open

bool PrimeFactor::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
      if(param == "foo") {
        //handled
      }
      else if(param == "bar") {
        //handled
      }
    }
  }
  
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void PrimeFactor::RegisterVariables()
{
  Register("NUM_VALUE", 0);
}

