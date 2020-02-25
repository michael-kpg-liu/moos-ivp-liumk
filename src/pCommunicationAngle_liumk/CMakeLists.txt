#--------------------------------------------------------
# The CMakeLists.txt for:                       pCommunicationAngle_liumk
# Author(s):                              liumk
#--------------------------------------------------------

SET(SRC
  CommunicationAngle_liumk.cpp
  CommunicationAngle_liumk_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pCommunicationAngle_liumk ${SRC})

TARGET_LINK_LIBRARIES(pCommunicationAngle_liumk
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

