set(LIBRARY_SOURCES
        BezierSurface.cpp
        BezierSurface.h
        Camera.cpp
        Camera.h
        Color.cpp
        Color.h
        DrawAPI.cpp
        DrawAPI.h
        Fragment.h
        Framebuffer.cpp
        Framebuffer.h
        Line.cpp
        Line.h
        Matrix4x4.h
        ParabolicCurve.cpp
        ParabolicCurve.h
        Sampler.cpp
        Sampler.h
        Scene.cpp
        Scene.h
        Shader.h
        Shape.h
        Triangle.cpp
        Triangle.h
        Vector2.h
        Vector4.h
        Vertex.h
)

add_library(open_ll ${LIBRARY_SOURCES})

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(open_ll PUBLIC OpenMP::OpenMP_CXX)
endif()
