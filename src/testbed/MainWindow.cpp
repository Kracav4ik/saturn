#include "MainWindow.h"

#include "OpenLL/Fragment.h"

#include <QDebug>
#include <chrono>

MainWindow::MainWindow()
    : fb(640, 480)
{
    setupUi(this);

    img.load(":/UV_Grid.jpg");

    drawAPi.loadTexture(reinterpret_cast<const uint32_t*>(img.constBits()), img.width(), img.height());
    timer.setInterval(100);
    timer.start();
    connect(&timer, &QTimer::timeout, [this]() {
        using namespace std::chrono;

        static size_t i = 0;
        qDebug() << "frame" << i;

        struct profile_info {
            profile_info(std::string what, microseconds amount) : what(std::move(what)), amount(amount) {}

            std::string what;
            microseconds amount;
        };
        std::vector<profile_info> prof;

        auto cur = high_resolution_clock::now();
        auto push_prof = [&](std::string what) {
            auto next = high_resolution_clock::now();
            prof.emplace_back(std::move(what), duration_cast<microseconds>(next - cur));
            cur = next;
        };

        static const std::vector<ll::Color> cls = {
                ll::Color(1, 0, 0),
                ll::Color(1, 0.5, 0),
                ll::Color(1, 1, 0),
                ll::Color(0, 1, 0),
                ll::Color(0, 0, 0),
        };

        drawAPi.reset();

        drawAPi.clear(fb, cls[i++ / 10 % cls.size()]);
        push_prof("fill done");

        static float a = 0;

        drawAPi.pushMatrix(ll::Matrix4x4::toScreenSpace(640, 480));

        drawAPi.pushMatrix(ll::Matrix4x4::perspective(M_PI/2, 640.0/480.0, 1, -2));

        drawAPi.pushMatrix(ll::Matrix4x4::lookAt(
                ll::Vector4::position(-0.5, 1, 4),
                ll::Vector4::position(-0.5, 0, 0),
                ll::Vector4::direction(0, 1, 0))
        );

        drawAPi.setFragmentShader([&](const ll::Fragment& vert, const ll::Sampler* sampler) {
            return sampler->getColor(vert.uv);
        });

//        drawAPi.pushMatrix(ll::Matrix4x4::translation(0, 0, 1));
        drawAPi.pushMatrix(ll::Matrix4x4::rotY(a/3));
//        drawAPi.pushMatrix(ll::Matrix4x4::translation(0, 0, -1));
        ll::Vertex vb0{ll::Color(0, 0, 0), ll::Vector4::position(-2, 0, -3), ll::Vector2{0, 0}};
        ll::Vertex vb1{ll::Color(1, 0, 0), ll::Vector4::position(2, 0, -3), ll::Vector2{1, 0}};
        ll::Vertex vb2{ll::Color(0, 1, 0), ll::Vector4::position(-2, 0, 3), ll::Vector2{0, 1}};
        ll::Vertex vb3{ll::Color(0, 0, 1), ll::Vector4::position(2, 0, 3), ll::Vector2{1, 1}};
        drawAPi.addTriangles({
                ll::Triangle{ vb0, vb1, vb2 },
                ll::Triangle{ vb2, vb1, vb3 },
        });
//        drawAPi.popMatrix();
        drawAPi.popMatrix();
//        drawAPi.popMatrix();

        drawAPi.setFragmentShader([&](const ll::Fragment& vert, const ll::Sampler* sampler) {
            return vert.color;
        });

        drawAPi.pushMatrix(ll::Matrix4x4::rotY(M_PI/3*sinf(a)));
        a += 0.1;

        ll::Vertex v0{ll::Color(0, 0, 0), ll::Vector4::position(0, 0, 0)};
        ll::Vertex v1{ll::Color(1, 0, 0), ll::Vector4::position(1, 0, 0)};
        ll::Vertex v2{ll::Color(0, 1, 0), ll::Vector4::position(0, 1, 0)};
        ll::Vertex v4{ll::Color(0, 0, 1), ll::Vector4::position(0, 0, 1)};

        drawAPi.addTriangles({
            ll::Triangle{ v0, v1, v2 },
            ll::Triangle{ v0, v2, v4 },
            ll::Triangle{ v0, v4, v1 },
            ll::Triangle{ v2, v1, v4 },
        });

        v0.pos.z = v1.pos.z = v2.pos.z = 1;
        v4.pos.z = 2;
        drawAPi.addTriangles({
            ll::Triangle{ v0, v1, v2 },
            ll::Triangle{ v0, v2, v4 },
            ll::Triangle{ v0, v4, v1 },
            ll::Triangle{ v2, v1, v4 },
        });

        v0.pos.z = v1.pos.z = v2.pos.z = 2;
        v4.pos.z = 3;
        drawAPi.addTriangles({
            ll::Triangle{ v0, v1, v2 },
            ll::Triangle{ v0, v2, v4 },
            ll::Triangle{ v0, v4, v1 },
            ll::Triangle{ v2, v1, v4 },
        });
        drawAPi.drawFrame(fb);
        push_prof("frame draw");

        static std::vector<uint32_t> pixels;
        pixels = fb.getColorsARGB32();
        push_prof("ARGB32 convert");

        static QImage img;
        img = QImage((const uchar*)pixels.data(), fb.getW(), fb.getH(), QImage::Format_ARGB32);
        push_prof("image done");

        static QPixmap pm;
        pm = QPixmap::fromImage(img);
        push_prof("pixmap done");

        static QGraphicsScene scene;
        scene.clear();
        scene.addPixmap(pm);
        drawArea->setScene(&scene);
        drawArea->show();
        push_prof("draw done");

        for (const auto& line : prof) {
            qDebug() << "  *" << line.what.c_str() << "in" << line.amount.count() << "us";
        }
    });
}
