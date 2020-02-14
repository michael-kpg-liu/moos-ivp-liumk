/************************************************************/
/*    NAME: Michael Liu                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.h                                        */
/*    DATE: 14 FEB 2020                                                */
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
  ~PrimeEntry() {};

  void setOriginalVal(unsigned long int v) {m_orig=v};
  void setReceivedIndex(unsigned int v)    {m_received_index=v;};
  void setCalculatedIndex(unsigned int v)  {m_calculated_index=v;};
  void setStartTime(unsigned int v)        {m_start_index=v;};
  void setCurrentValue(unsigned int v)     {m_current=v;};
  void setDone(bool v)                     {m_done=v};

  bool done() {return(m_done);};

  bool factor(unsigned long int max_steps);

  std::string getReport();

 protected:
  uint64_t     m_orig;
  unsigned int m_received_index;
  unsigned int m_calculated_index;
  uint64_t     m_start_index;
  uint64_t     m_current;
  bool         m_done;
  
  std::vector<uint64_t> m_factors;
};
#endif
