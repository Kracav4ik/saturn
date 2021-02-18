set(LIBRARY_SOURCES
        Color.cpp
        Color.h
        DrawAPI.cpp
        DrawAPI.h
        Framebuffer.cpp
        Framebuffer.h
        Matrix4x4.h
        Sampler.cpp
        Sampler.h
        Triangle.h
        Vector2.h
        Vector4.h
        Vertex.h
)

add_library(open_ll ${LIBRARY_SOURCES})
