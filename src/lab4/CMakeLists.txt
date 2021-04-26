set(EXE_NAME saturn_lab4)

set(EXE_SOURCES
        Frame.cpp
        Frame.h
        main.cpp
        MainWindow.h
        MainWindow.cpp
        Mesh.cpp
        Mesh.h
        Model.cpp
        Model.h
        Polyline.cpp
        Polyline.h
        SelectedAndDraggable.cpp
        SelectedAndDraggable.h
        LinesWithClipRect.cpp LinesWithClipRect.h)

add_executable(${EXE_NAME} ${EXE_SOURCES})

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
