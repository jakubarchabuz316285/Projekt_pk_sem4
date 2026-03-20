cmake_minimum_required(VERSION 3.16)

project(PK-Symulator_UAR VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)

set(PROJECT_SOURCES
    main.cpp
    MainWindow.h MainWindow.cpp
    Testy_UAR.cpp
    ARX.h ARX.cpp
    RegulatorPID.h RegulatorPID.cpp
    UAR.h UAR.cpp
    Generator.h Generator.cpp
    GeneratorSinusoida.h GeneratorSinusoida.cpp
    GeneratorProstokatny.h GeneratorProstokatny.cpp
    State.h State.cpp
    Test.h Test.cpp
    DialogARX.h DialogARX.cpp
    DialogARX.ui MainWindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS Core)

    qt_add_executable(PK-Symulator_UAR
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        BUILD_DEFINE.h

        DialogARX.h DialogARX.cpp DialogARX.ui
        ListWithWxtremes.hpp ListWithWxtremes.cpp
        Doxyfile
        .gitignore
        .clang-uml
        QSaveState.hpp QSaveState.cpp
        DialogDebug.hpp DialogDebug.cpp DialogDebug.ui
        ARXCoefficientItem.hpp ARXCoefficientItem.cpp ARXCoefficientItem.ui
        GeneratorSkokJednostkowy.hpp GeneratorSkokJednostkowy.cpp
        QTimerState.hpp QTimerState.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET nienazwane APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(PK-Symulator_UAR SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(PK-Symulator_UAR
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(PK-Symulator_UAR PRIVATE Qt${QT_VERSION_MAJOR}::Widgets  Qt${QT_VERSION_MAJOR}::Charts)
target_link_libraries(PK-Symulator_UAR PRIVATE Qt6::Core)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.PKSymulatorUAR)
endif()
set_target_properties(PK-Symulator_UAR PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS PK-Symulator_UAR
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(PK-Symulator_UAR)
endif()
