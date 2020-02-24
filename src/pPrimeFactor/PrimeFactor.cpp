/************************************************************/
/*    NAME: liumk                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.cpp                                        */
/*    DATE: 14 FEB 2020 to 23 FEB 2020                         */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PrimeFactor.h"
#include <cstdlib> // Library needed for 64-bit unsigned long
// intergers and converting from unsigned long int m_orig;
// Original prime number uint64_t used instead of double given
// the focus on positive integers (and 0). Prime factorization
// needs actual prime numbers not an approximate number.

using namespace std;

//---------------------------------------------------------
// Constructor

PrimeFactor::PrimeFactor()
{
  m_index = 0;
  m_factors = 0;
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
    string sval  = msg.GetString(); 
    
    if (key=="NUM_VALUE"){
      //Convert from string to long unsigned int
      m_index++;
      // Compose a MOOS string message with numerical components
      // temp(sval) - temporary variable for numerical components
      // to move to m_ival
      stringstream temp(sval);
      m_ival = 0;
      temp >> m_ival;
      
      //Make Prime Entry object and save to list
      PrimeEntry m_prime;
      m_prime.setOriginalVal(m_ival);
      m_prime.setReceivedIndex(m_index);
      m_mail_list.push_back(m_prime);

    }
  }

   
#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
   
	
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
//            happens AppTick times per second


bool PrimeFactor::Iterate()
{
  // Set max iterations. 100,000 per p. 11 of Lab 5.
  m_max_iter=100000;
  // Loop through mail list  
  for(std::list<PrimeEntry>::iterator k=m_mail_list.begin();k!=m_mail_list.end();k++){
    
    // Calculate prime number    
    PrimeEntry& m_prime_entry=*k;
    // Point to Prime Entry factor function
    m_prime_entry.factor(m_max_iter);
    
    // Publish result and erase object in list;
    // otherwise, hold for next iteration.
    if (m_prime_entry.done()){
      m_factors++; // Move factor array position.
      m_prime_entry.setCalculatedIndex(m_factors); //Update index calculated
      m_max_iter=m_max_iter-m_prime_entry.iter_calc(); // Update max_iter
      m_result_str=m_prime_entry.getReport(); // Update results
      Notify("PRIME_RESULT",m_result_str); // Display when prompted "PRIME_RESULT"
      k=m_mail_list.erase(k);       //Remove value from list
    }
    else{ //unfinished with factoring by max iterations
      break;
    }
    
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

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
  // Register("FOOBAR", 0);
}

