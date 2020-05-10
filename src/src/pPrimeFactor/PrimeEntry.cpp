/************************************************************/
/*    NAME: ruic                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.cpp                                          */
/*    DATE: Feb 20 2020                                          */
/************************************************************/

#include "PrimeEntry.h"
#include <cmath>
#include <string>
#include <iostream>
#include <chrono>

using namespace std;

//---------------------------------------------------------
// Constructor

PrimeEntry::PrimeEntry()
{
  m_orig = 0;
  m_done = false;
  m_received_index = 0;
  m_calculated_index = 0;
  m_current = 0;
  m_current_ii = 2;
  m_cal_time = 0;

}

//---------------------------------------------------------
// Destructor

PrimeEntry::~PrimeEntry()
{
}

//---------------------------------------------------------
// factor

bool PrimeEntry::factor(unsigned long	int max_steps){

	unsigned long int iter = 0;
	auto start = chrono::steady_clock::now();

	while(iter <= max_steps){ // check iter count

		// DEBUG COMMENTS
		// cout<<"m_current = " << m_current <<endl;
		// cout<<"m_current_ii = "<< m_current_ii << endl;
		// cout<<"iter  = "<< iter <<endl;

		if (m_current % m_current_ii == 0){ // if factor found, add to factor list 
			addFactor(m_current_ii);
			m_current = m_current/m_current_ii;
			m_current_ii = 1;

			if (m_current == 1){ // if all factors found, end
				setDone(true);
				auto end = chrono::steady_clock::now();
				m_cal_time = m_cal_time + chrono::duration_cast<chrono::milliseconds>(end - start).count();
				return(true);
			}
		}

		if (m_current_ii > pow(m_current,0.5)+1){ // if tested all numbers less than sqrt of N, end
			addFactor(m_current);
			setDone(true);
			auto end = chrono::steady_clock::now();
			m_cal_time = m_cal_time + chrono::duration_cast<chrono::milliseconds>(end - start).count();
			return(true);
		}

		m_current_ii = m_current_ii+1; // update current test number and iter count
		iter = iter + 1;
	}
	
	auto end = chrono::steady_clock::now();
	m_cal_time = m_cal_time + chrono::duration_cast<chrono::milliseconds>(end - start).count();

	return(false);
}

//---------------------------------------------------------
// getReport

string PrimeEntry::getReport(){

	string report;
	string output;

	for (int ii=0; ii<m_factors.size();ii++){
		output=output+to_string(m_factors[ii])+":";
	}

	output.pop_back();

	report = "orig="+ to_string(m_orig) + ", received="+ to_string(m_received_index)+", calculated="+ to_string(m_calculated_index)+", solve_time=" + to_string(m_cal_time)+"msec, primes= "+output+", username=ruic";

	return(report);
}


