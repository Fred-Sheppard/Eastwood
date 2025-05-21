cmake_minimum_required(VERSION 3.30)
project(Eastwood)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
  Core
  Gui
  Widgets
  UiTools
  REQUIRED)
  
set(FRONTEND_SOURCES
    frontend/windows/login/login.cpp
        frontend/windows/login/login.h
        frontend/windows/login/login.ui
    frontend/windows/register/register.cpp
    frontend/windows/register/register.h
    frontend/windows/register/register.ui
    frontend/utils/messagebox.h
)

add_executable(Eastwood
        main.cpp
        ${FRONTEND_SOURCES}
)

target_link_libraries(Eastwood
  Qt::Core
  Qt::Gui
  Qt::Widgets
  Qt::UiTools
)

