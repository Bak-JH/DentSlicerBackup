cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME DentSlicer)
project(${PROJECT_NAME}) # Your project name

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

LINK_LIBRARIES( -lpthread)

# if (${Qt5_FOUND})
#     message("Found Qt " ${Qt5_VERSION})
# else()
#     message("Couldn't find Qt")
# endif()

find_package(Qt5Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5SerialPort REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Gui  REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(Qt5WebSockets REQUIRED)


set(SOURCES
bedcontrol.cpp
bedserialport.cpp
common/jsonreadsetting.cpp
common/jsonsetting.cpp
common/jsonutil.cpp
common/jsonwritesetting.cpp
filevalidator.cpp
infosetting.cpp
kinetimecalc.cpp
logger.cpp
main.cpp
modelno.cpp
networkcontrol.cpp
printersetting.cpp
printscheduler.cpp
resinsetting.cpp
resinupdater.cpp
updater.cpp
version.cpp
websocketclient.cpp
wifiinfo.cpp
wpa.cpp
wpa_ctrl/common.c
wpa_ctrl/os_unix.c
wpa_ctrl/wpa_ctrl.c
wpa_ctrl/wpa_debug.c
ymodem.cpp
zip/zip.cpp
qmlconnecter.cpp
qmlconnecter.h
Singleton.h
bedcontrol.h
bedserialport.h
schedulerthread.h
schedulerthread.cpp
common.h
common/jsonreadsetting.h
common/jsonsetting.h
common/jsonutil.h
common/jsonwritesetting.h
filevalidator.h
infosetting.h
kinetimecalc.h
logger.h
modelno.h
networkcontrol.h
printersetting.h
printscheduler.h
resinsetting.h
resinupdater.h
updater.h
version.h
websocketclient.h
wifiinfo.h
wpa.h
wpa_ctrl/build_config.h
wpa_ctrl/common.h
wpa_ctrl/ieee802_11_defs.h
wpa_ctrl/includes.h
wpa_ctrl/os.h
wpa_ctrl/os.h
wpa_ctrl/wpa_ctrl.h
wpa_ctrl/wpa_debug.h
wpa_ctrl/wpabuf.h
ymodem.h
zip/zip.h
schedulerthread.h
schedulerthread.cpp
FilesystemModel.h
FilesystemModel.cpp
updateconnector.h
updateconnector.cpp
qml.qrc
font.qrc
image.qrc
#    zip/zip_file.hpp
)

add_executable( ${PROJECT_NAME} WIN32  ${SOURCES})

target_link_libraries(${PROJECT_NAME}  
Qt5::Widgets
Qt5::Network
Qt5::Gui
Qt5::Qml
Qt5::Svg
Qt5::WebSockets
Qt5::Quick
Qt5::SerialPort
)
