set(EXE_NAME saturn_lab6)

set(EXE_SOURCES
        Frame.cpp
        Frame.h
        main.cpp
        MainWindow.h
        MainWindow.cpp
        Model.cpp
        Model.h
        Polyline.cpp
        Polyline.h
)

qt5_add_resources(EXE_RESOURCES main.qrc)

add_executable(${EXE_NAME} ${EXE_SOURCES} ${EXE_RESOURCES})

target_link_libraries(${EXE_NAME} ${QT_LIBS} open_ll)

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
