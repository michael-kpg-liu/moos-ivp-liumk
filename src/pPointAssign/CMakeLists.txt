#--------------------------------------------------------
# The CMakeLists.txt for:                       pPointAssign
# Author(s):                              Michael Liu
#--------------------------------------------------------

SET(SRC
  PointAssign.cpp
  PointAssign_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pPointAssign ${SRC})

TARGET_LINK_LIBRARIES(pPointAssign
   ${MOOS_LIBRARIES}
   geometry
   mbutil
   apputil
   m
   pthread)

