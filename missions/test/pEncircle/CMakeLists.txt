#--------------------------------------------------------
# The CMakeLists.txt for:                       pEncircle
# Author(s):                              Mike Benjamin
#--------------------------------------------------------

SET(SRC
  Encircle.cpp
  Encircle_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pEncircle ${SRC})

TARGET_LINK_LIBRARIES(pEncircle
   ${MOOS_LIBRARIES}
   apputil
   contacts
   ufield
   geometry
   mbutil
   m)

