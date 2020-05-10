#--------------------------------------------------------
# The CMakeLists.txt for:                       pDiscoverFrontAngle
# Author(s):                              appcasting
#--------------------------------------------------------

SET(SRC
  DiscoverFrontAngle.cpp
  DiscoverFrontAngle_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pDiscoverFrontAngle ${SRC})

TARGET_LINK_LIBRARIES(pDiscoverFrontAngle
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

