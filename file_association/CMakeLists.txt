cmake_minimum_required(VERSION 3.14)

project(file_association LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core DBus REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core DBus REQUIRED)

add_executable(file_association
  main.cpp
  fileassociation.cpp
  fileassociation.h
  FA_Adaptor.cpp
  FA_Adaptor.h
  FA_Interface.cpp
  FA_Interface.h
  essentials.cpp
  essentials.h
)

target_link_libraries(file_association Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::DBus)
