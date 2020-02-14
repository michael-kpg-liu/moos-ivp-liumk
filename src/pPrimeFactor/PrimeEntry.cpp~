/************************************************************/
/*    NAME: Michael Liu                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.cpp                                       */
/*    DATE: 14 FEB 2020                                                */
/************************************************************/

#include "PrimeEntry.h"
#include "PrimeFactor.h"

PrimeEntry::PrimeEntry()
{
  m_orig = 0;
  m_received_index = 0;
  m_calculated_index = 0;
  m_start_index = 0;
  m_current = 0;
  m_done = false;
}

//---------------------------------------------------------
// Procedure:  Prime Factorization
// Reference:  The Modern C++ Challenge by Marius Bancilla [p.21]
bool PrimeEntry::Factor
{
  int iteration = 0;
  while(m_current %2 == 0) {
    m_factors.push_back(2);
    m_current = m_current/2;
    iteration = iteration + 1; // Counts number of iterations
    // Following "if" statement exits the function if max steps reached.
    if (iteration >= max_steps)
      return(false);
  }

  // Function must be odd, since even result would have exited above.
  for(int p=3; p <= sqrt(m_current); p += 2){
    m_stored = m_stored+2;
    iteration = iteration + 1;
    while(m_current%i == 0){
      m_factors.push_back(i);
      m_current = m_current/i;
    }
    // Following "if" statement exits the function if max steps reached.
    if (iteration >= max_steps)
      return(false);
  }

  if(m_current > 2)
    m_factors.push_back(m_current);
  return(true);
}

int main()
{
  cout << "Number: " << endl;
  cin >> number << endl;
  auto factors = prime_factors(number);
  copy(begin(factors), end(factors), ostream_iterator<Factor>(cout << " "));
}
