#--------------------------------------------------------
# The CMakeLists.txt for:                       pSpiralPath
# Author(s):                              Zach Duguid
#--------------------------------------------------------

SET(SRC
  SpiralPath.cpp
  SpiralPath_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pSpiralPath ${SRC})

TARGET_LINK_LIBRARIES(pSpiralPath
   ${MOOS_LIBRARIES}
   apputil
   contacts
   ufield
   geometry
   mbutil
   m
   pthread)

