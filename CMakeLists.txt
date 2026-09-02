cmake_minimum_required(VERSION 3.16)

project(Desktop_App_Qt2 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
    QT NAMES Qt6 Qt5
    REQUIRED COMPONENTS Widgets Sql Network
)

find_package(
    Qt${QT_VERSION_MAJOR}
    REQUIRED COMPONENTS Widgets Sql Network
)

set(PROJECT_SOURCES
    main.cpp

    mainwindow.h
    mainwindow.cpp
    mainwindow.ui

    src/models/User.h
    src/models/User.cpp
    src/models/Task.h
    src/models/Task.cpp

    src/database/DatabaseManager.h
    src/database/DatabaseManager.cpp
    src/database/UserRepository.h
    src/database/UserRepository.cpp
    src/database/TaskRepository.h
    src/database/TaskRepository.cpp

    src/security/PasswordHasher.h
    src/security/PasswordHasher.cpp

    src/services/AuthService.h
    src/services/AuthService.cpp
    src/services/ReminderService.h
    src/services/ReminderService.cpp

    src/ui/LoginWidget.h
    src/ui/LoginWidget.cpp
    src/ui/LoginWidget.ui
    src/ui/DashboardWidget.h
    src/ui/DashboardWidget.cpp
    src/ui/DashboardWidget.ui
    src/ui/TaskDialog.h
    src/ui/TaskDialog.cpp
    src/ui/TaskDialog.ui

    resources/resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(
        Desktop_App_Qt2
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(
        Desktop_App_Qt2
        ${PROJECT_SOURCES}
    )
endif()

target_include_directories(
    Desktop_App_Qt2
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
    Desktop_App_Qt2
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Network
)

set_target_properties(
    Desktop_App_Qt2
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Desktop_App_Qt2)
endif()

include(CTest)

if(BUILD_TESTING)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

    add_executable(
        TaskManagementTests
        tests/TaskManagementTests.cpp

        src/models/User.h
        src/models/User.cpp
        src/models/Task.h
        src/models/Task.cpp

        src/database/DatabaseManager.h
        src/database/DatabaseManager.cpp
        src/database/UserRepository.h
        src/database/UserRepository.cpp
        src/database/TaskRepository.h
        src/database/TaskRepository.cpp

        src/security/PasswordHasher.h
        src/security/PasswordHasher.cpp

        src/services/AuthService.h
        src/services/AuthService.cpp
    )

    target_include_directories(
        TaskManagementTests
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    target_link_libraries(
        TaskManagementTests
        PRIVATE
            Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::Sql
            Qt${QT_VERSION_MAJOR}::Test
            Qt${QT_VERSION_MAJOR}::Network
    )

    add_test(NAME TaskManagementTests COMMAND TaskManagementTests)
endif()
