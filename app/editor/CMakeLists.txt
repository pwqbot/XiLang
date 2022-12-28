file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp *.h)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(
  Qt5
  COMPONENTS Widgets DBus
  REQUIRED)

add_executable(editor ${srcs} lpf.qrc lpf.ui)
target_link_libraries(editor parser ast generator Qt5::Widgets Qt5::DBus)
