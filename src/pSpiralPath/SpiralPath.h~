/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: SpiralPath.h                                    */
/*    DATE: 2019 May                                        */
/************************************************************/

#ifndef SpiralPath_HEADER
#define SpiralPath_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class SpiralPath : public AppCastingMOOSApp
{
  public:
    SpiralPath();
    ~SpiralPath(){};

  protected: // Standard MOOSApp functions to overload  
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    bool setLoiterDelta(std::string);
    bool setCaptain(std::string);
    bool handleNodeReport(std::string);
    bool handleNodeReportLocal(std::string);
    bool handleSpiralRequest(std::string);
    bool handleParameterEstimate(std::string);
    bool handleLoiterReport(std::string);
    bool handleSpiralMode(std::string);
    bool handleNavX(std::string);
    bool handleNavY(std::string);
    bool handleNewSensorReport(std::string);


  protected: // Standard AppCastingMOOSApp function to overload 
    bool buildReport();

  protected:
    void registerVariables();

  private:
    // general information
    std::string m_captain;
    std::string m_os_name;
    std::string m_collab_name;
    std::string m_grp_filter;
    uint        m_total_node_reports;
    double      m_os_tstamp;

    // loiter parameters
    double m_osx;
    double m_osy;
    double m_loiter_x;
    double m_loiter_y;
    double m_loiter_radius;
    double m_loiter_delta;
    double m_loiter_offset;

    // variables parsed from the Front Estimator
    std::string m_latest_estimate;
    std::string m_est_vname;
    uint   m_num_estimate_rcd;
    uint   m_num_cycles;
    uint   m_num_msgs_sent;
    uint   m_num_msgs_received;
    bool   m_req_new_estimate;
    bool   m_spr_active;
    double m_est_offset;
    double m_est_angle;
    double m_est_amplitude;
    double m_est_period;
    double m_est_wavelength;
    double m_est_alpha;
    double m_est_beta;
    double m_est_tempnorth;
    double m_est_tempsouth;
};

#endif 
