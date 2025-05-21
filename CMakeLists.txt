cmake_minimum_required(VERSION 3.30)
project(Eastwood)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/Qt5")
find_package(OpenSSL REQUIRED)
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(Eastwood test/test.cpp
        src/HTTPSClient.cpp
        include/BaseClient.h
        include/HTTPSClient.h
        src/BaseClient.cpp
)
target_link_libraries(Eastwood
        Qt::Core
        Qt::Gui
        Qt::Widgets
        OpenSSL::SSL
        OpenSSL::Crypto
)

