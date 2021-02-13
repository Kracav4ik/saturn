# QTDIR must point to c:/Qt/5.7/mingw53_32/ or similar
message("qtdir is set to '$ENV{QTDIR}' (must be 'c:/Qt/5.7/mingw53_32/' or similar)")
set(CMAKE_PREFIX_PATH "$ENV{QTDIR}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(EXE_NAME saturn)

set(QT_LIBS
        Qt5::Core
        Qt5::Widgets
)

include_directories(.)

set(EXE_SOURCES
        main.cpp
        MainWindow.h
        MainWindow.cpp
)

add_executable(${EXE_NAME} ${EXE_SOURCES})
target_link_libraries(${EXE_NAME} ${QT_LIBS})

# Copy required dlls
add_custom_command(
        TARGET ${EXE_NAME} POST_BUILD
        COMMAND $ENV{QTDIR}/bin/windeployqt.exe
                --no-svg
                --no-translations
                --no-opengl-sw
                --no-system-d3d-compiler
                $<TARGET_FILE:${EXE_NAME}>
)
