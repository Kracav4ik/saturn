set(LIBRARY_SOURCES
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
