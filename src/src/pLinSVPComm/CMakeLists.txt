#--------------------------------------------------------
# The CMakeLists.txt for:                       pLinSVPComm
# Author(s):                              ruic
#--------------------------------------------------------

SET(SRC
  LinSVPComm.cpp
  LinSVPComm_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pLinSVPComm ${SRC})

TARGET_LINK_LIBRARIES(pLinSVPComm
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

