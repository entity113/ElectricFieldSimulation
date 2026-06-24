cmake_minimum_required(VERSION 3.19)
project(ElectricFieldSimulation LANGUAGES CXX)

find_package(Qt6 6.12 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(ElectricFieldSimulation
    WIN32 MACOSX_BUNDLE
    main.cpp
    window.h window.cpp window.ui
    simulation.h simulation.cpp
    coordinatesdialog.h coordinatesdialog.cpp coordinatesdialog.ui
)

target_link_libraries(ElectricFieldSimulation
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS ElectricFieldSimulation
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ElectricFieldSimulation
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
