/************************************************************/
/*    NAME: ruic                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.b                                          */
/*    DATE:                                                 */
/************************************************************/

#include <string>
#include <vector>
#include <cstdint>

#ifndef PRIME_ENTRY_HEADER 
#define PRIME_ENTRY_HEADER

class PrimeEntry
{
	public:
		PrimeEntry(); 
		~PrimeEntry();
		
		void setOriginalVal(unsigned long int v) {m_orig=v; m_current=v;};
		void setReceivedIndex(unsigned int v) {m_received_index=v;};
		void setCalculatedIndex(unsigned int v) {m_calculated_index=v;};
		void addFactor(unsigned long int v) {m_factors.push_back(v);};
		void setDone(bool v) {m_done=v;};

		bool done() {return(m_done);};
		
		
		bool factor(unsigned long	int max_steps);

		std::string  getReport();


	protected: 
		uint64_t m_orig;
		uint64_t m_current;
		uint64_t m_current_ii;
		bool m_done;
		unsigned int m_received_index;
		unsigned int m_calculated_index;
		unsigned int m_cal_time;
		std::vector<uint64_t> m_factors; 
};

#endif