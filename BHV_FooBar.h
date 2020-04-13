/************************************************************/
/*    NAME: Michael Liu                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_FooBar.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef FooBar_HEADER
#define FooBar_HEADER

#include <string>
#include "IvPBehavior.h"

class BHV_FooBar : public IvPBehavior {
public:
  BHV_FooBar(IvPDomain);
  ~BHV_FooBar() {};
  
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onCompleteState();
  void         onIdleState();
  void         onHelmStart();
  void         postConfigStatus();
  void         onRunToIdleState();
  void         onIdleToRunState();
  IvPFunction* onRunState();

protected: // Local Utility functions

protected: // Configuration parameters

protected: // State variables
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_FooBar(domain);}
}
#endif
