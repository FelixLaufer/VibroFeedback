add_library(VibroControl
  VibroStudy.cpp
  VibroControl.cpp
  TcpClient.cpp
)

target_link_libraries(VibroControl
  Boost::system
 )
