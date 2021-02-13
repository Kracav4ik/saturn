#include "MainWindow.h"

#include "OpenLL/DrawAPI.h"

#include <QDebug>
#include <chrono>

MainWindow::MainWindow()
    : fb(640, 480)
{
    setupUi(this);

    timer.setInterval(1000);
    timer.start();
    connect(&timer, &QTimer::timeout, [this]() {
        using namespace std::chrono;

        static int i = 0;
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
                {1, 1, 0, 0},
                {1, 1, 0.5, 0},
                {1, 1, 1, 0},
                {1, 0, 1, 0},
                {1, 0, 0, 0},
        };

        ll::DrawAPI::clear(fb, cls[i++ % cls.size()]);
        push_prof("fill done");

        static std::vector<uint32_t> pixels;
        pixels = fb.getColorsARGB32();
        push_prof("i->f convert");

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
