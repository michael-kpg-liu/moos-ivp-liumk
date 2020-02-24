/************************************************************/
/*    NAME: Michael Liu                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.cpp                                       */
/*    DATE: 14 FEB 2020 to 23 FEB 2020                                */
/************************************************************/

#include "PrimeEntry.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// PrimeEntry
// Purpose: Construct PrimeEntry class, set latest number to factor
//---------------------------------------------------------

PrimeEntry::PrimeEntry()
{
  m_start_index=0;
  m_ii=0;
  m_done = false;
  m_k = 2;
}

//---------------------------------------------------------
// factor()
// Purpose: Find next prime within a given maximum number
// of steps to search. Max steps depends on received/calculated
// index. Build idea from notion of going up from 1, 2, 3, etc.
// Prime numbers are only divisible by 1 and themselves.
// Therefore, check to see if a number is divisible by the first
// numbers.  Keep dividing by the low number until it cannot
// divide then increment (addresses 4, 9, etc.).
//---------------------------------------------------------

void PrimeEntry::factor(unsigned long int max_steps){
  //Reset iterations of factor function. 
  m_ii=0; 
  
  if (m_start_index==0){ //First entry
    m_start_index=m_orig; // Storage of term from m_orig to m_start_index.
    m_k=2; // First check for number "2".  Skip "1".
    m_start_time=MOOSTime(); // Start counting calculation time.
    }

  while (m_k<=ceil(sqrtl(m_start_index))){
    // Use of square root based on a mathematical proof found
    // on mathandmultimedia.com (also told p.12 of Lab 5).
    // Proof deals with composite numbers (ie positive integers
    // formed my multiplying two smaller positive integers.  We
    // are allowed to assume inputs are only positive numbers
    // (and 0), so this will be a good condition. 
       m_ii++; // Add iteration
       if (m_ii>max_steps){ // Reached max steps, stop factoring
	 break;
       }
       if (m_start_index%m_k==0){ // Divisible 
	 m_factors.push_back(m_k); // .push_back adds terms to
	                           // end of array.
	 m_start_index=m_start_index/m_k; // Update m_start_index
	 m_k=2; //Reset k. 
       }
       m_k++; // Increase m_k to next positive integer.
              // This method will check redundant numbers (ex. 4),
              // but this is "OK" as cited in p.12 of Lab 5.
  }
 
   //Check if done
  if (m_k>=ceil(sqrtl(m_start_index))){ // Check based on proof discussed
                                        // above.
    m_finished_iter=m_ii; // Retain iteration number.
    m_finish_time=MOOSTime(); // Stop counting calculation time.

    if (m_start_index!=1){ //Add last (prime) number
      m_factors.push_back(m_start_index);
      }
      setDone(true);
  }
  else{
    setDone(false);
  }
}

//---------------------------------------------------------
// getReport()
// Purpose: Print out prime list
//---------------------------------------------------------

std::string PrimeEntry::getReport(){
  // Print results: orig=90090,received=34,calculated=33,
  // solve_time=2.03,primes=2:3:3:5:7:11:13,username=jane"

  std::stringstream ss;
  ss << "orig=" << m_orig << ",received=" << m_received_index;
  ss << ",calculated=" << m_calculated_index << ",solve_time=";
  m_solve_time=m_finish_time-m_start_time; // Calculate time.
  ss << m_solve_time << ",";

  std::vector<unsigned long int>::iterator p;
  ss << "primes=";
  std::sort(m_factors.begin(),m_factors.end()); //Sort factors.
  for(p=m_factors.begin();p!=m_factors.end();p++){
    ss<<*p<<":";
  }

  ss.seekp(-1, std::ios_base::end); //Delete last ":"

  ss << ",username=liumk" << std::endl;
  //Return ss stream object
  return ss.str(); // Converts numerical values into string.
}
