add_executable(vibroTest
  vibroTest.cpp
)
target_link_libraries(vibroTest
  VibroControl
)

add_executable(vibroStudy
  vibroStudy.cpp
)
target_link_libraries(vibroStudy
  VibroControl
)