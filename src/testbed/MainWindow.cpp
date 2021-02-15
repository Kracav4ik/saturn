#include "MainWindow.h"

#include "OpenLL/DrawAPI.h"

#include <QDebug>
#include <chrono>

MainWindow::MainWindow()
    : fb(640, 480)
{
    setupUi(this);

    timer.setInterval(100);
    timer.start();
    connect(&timer, &QTimer::timeout, [this]() {
        using namespace std::chrono;

        static ll::DrawAPI drawAPi;

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

        drawAPi.setFragmentShader([&](const ll::Vertex& vert) {
            return vert.color;
        });
        drawAPi.reset();

        drawAPi.clear(fb, cls[i++ / 10 % cls.size()]);
        push_prof("fill done");

        static float a = 0;
        drawAPi.pushMatrix(ll::Matrix4x4::translation(320, 240, 0));
        drawAPi.pushMatrix(ll::Matrix4x4::rotZ(a));
        drawAPi.pushMatrix(ll::Matrix4x4::translation(-320, -240, 0));
        a += 0.3;

        drawAPi.addTriangles({
            ll::Triangle{{
                {ll::Color(1, 0, 0), ll::Vector4::position(20, 200, 0)},
                {ll::Color(0, 1, 0), ll::Vector4::position(400, 400, 0)},
                {ll::Color(0, 0, 1), ll::Vector4::position(600, 20, 0)},
            }},
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
