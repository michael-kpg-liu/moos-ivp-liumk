#--------------------------------------------------------
# The CMakeLists.txt for:                       pVehicleDataExchange
# Author(s):                              appcasting
#--------------------------------------------------------

SET(SRC
  VehicleDataExchange.cpp
  VehicleDataExchange_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pVehicleDataExchange ${SRC})

TARGET_LINK_LIBRARIES(pVehicleDataExchange
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   ufield
   m
   pthread)

