/************************************************************/
/*    NAME: Michael Liu                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.cpp                                       */
/*    DATE: 14 FEB 2020                                                */
/************************************************************/

#include "PrimeEntry.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// PrimeEntry
// Purpose: Constructor PrimeEntry class, set latest number to factor
//---------------------------------------------------------

PrimeEntry::PrimeEntry()
{
  m_N=0;
  m_ii=0;
  m_done = false;
  m_k = 2;
}

//---------------------------------------------------------
// setOriginalVal
// Purpose: Set original value to factor
//---------------------------------------------------------
void PrimeEntry::setOriginalVal(unsigned long int v){
  m_orig=v;
};

//---------------------------------------------------------
// showOriginal
// Purpose: Show original value
//---------------------------------------------------------
unsigned long int PrimeEntry::showOriginal(){
  return m_orig;
}

//---------------------------------------------------------
//setReceivedIndex
//Purpose: Index at which # was received
//---------------------------------------------------------

void PrimeEntry::setReceivedIndex(unsigned int v){
  m_received_index=v;
};

//---------------------------------------------------------
//setCalcualtedIndex
//Purpose: Set index at which number was solved
//---------------------------------------------------------
void PrimeEntry::setCalculatedIndex(unsigned int v){
  m_calculated_index=v;
};

//---------------------------------------------------------
// showCalculatedIndex
// Purpose: Show index of calculated numbers
//---------------------------------------------------------
unsigned long int PrimeEntry::showCalculatedIndex(){
  return m_calculated_index;
}

//---------------------------------------------------------
//returnPrimeString
//Purpose: Display string of prime numbers
//---------------------------------------------------------
std::string PrimeEntry::returnPrimeString(){
  return uintToString(m_calculated_index);
}

//---------------------------------------------------------
//iter_calc
//Purpose: Return number of iterations to calculate prime factors
//---------------------------------------------------------

long unsigned int PrimeEntry::iter_calc(){
  return m_finished_iter;
}

//---------------------------------------------------------
//Set Done
//Purpose: Set finished status m_done = true or false
//---------------------------------------------------------

void PrimeEntry::setDone(bool v){
  m_done=v;
};

//---------------------------------------------------------
//done()
//Purpose: Return finished status
//---------------------------------------------------------

bool PrimeEntry::done(){
  return(m_done);
};

//---------------------------------------------------------
//factor
//Purpose: Factoring function for a certain number of steps.
//Max steps depends on received/calculated index.
//---------------------------------------------------------

void PrimeEntry::factor(unsigned long int max_steps){
  //Reset iterations of factor function.
  m_ii=0;

  if(m_N==0){ //First entry
    m_N=m_orig;
    m_start_time=MOOSTime();
  }

     while(m_done==false){
    // If the condition (m_done==false) is true, do the while loop.
    // If not, jump out the while loop. 
        for(int m_k=2; m_k<=m_orig;){
        //In this for loop, I want to go through 2 to the input number to find the factor.
            if(m_orig%m_k==0){
            // if the input can be devided by i,
                cout << "found " << m_k << endl;
                factor.push_back(m_k);
                // I'll store i to the end of the list.
                m_orig=m_orig/m_k;
                //then let the input number devided by i.
 
            }
            else{
	        // If input number cannot be devided by m_k, check m_k+1.
                m_k++;
		// Iteration
		m_ii++;
            }
        }
        // Once the for loop is over,
	m_finish_time=MOOSTime();
        m_done=true;
        //we'll set the boolean variable m_done=true, so that it will jump out of the while loop. 
        cout << "done!" << endl; 
    }
// We've found all the prime factor, let's show all of them. 
    list<int>::iterator m_calculated_index;
    // We declare a function of list and set a pointer called p.  
    for(m_calculated_index=factor.begin(); m_calculated_index!=factor.end();m_calculated_index++){
    // the pointer p will go through the list (which stored the primefactor we found).
    // for here {2, 3,  3,  47, 14593}
    //           ^
    //              ^ 
    //                  ^
    //                      ^
    //                          ^
       cout << *m_calculated_index << " ";  
    }
    return;    
}

//---------------------------------------------------------
//getReport
//Purpose: Print out prime list
//---------------------------------------------------------

std::string PrimeEntry::getReport(){
  //Print results: orig=90090,received=34,calculated=33,solve_time=2.03, primes=2:3:3:5:7:11:13,username=jane"

  std::stringstream ss;
  ss << "orig=" << m_orig << ", received=" << m_received_index << ", calculated=" << m_calculated_index << "," << "solve_time=";
  m_solve_time=m_finish_time-m_start_time; // Calculate time.
  ss<<m_solve_time<<", ";

  std::vector<unsigned long int>::iterator p;
  ss << "primes=";
  std::sort(m_factors.begin(),m_factors.end()); //Sort factors.
  for(p=m_factors.begin();p!=m_factors.end();p++){
    ss<<*p<<":";
  }

  ss.seekp(-1, std::ios_base::end); //Delete last ":"

  ss<<", username=liumk"<<std::endl;
  //Return ss stream object
  return ss.str();
}
