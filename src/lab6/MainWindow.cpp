#include "MainWindow.h"

#include "OpenLL/Scene.h"
#include "OpenLL/Fragment.h"
#include "OpenLL/Camera.h"
#include <QDebug>
#include <QWheelEvent>
#include <chrono>

MainWindow::MainWindow() {
    setupUi(this);

    QFile f(":/low-poly-mill.obj");

    f.open(QIODevice::ReadOnly | QIODevice::Text);

    static ll::Scene scene = ll::Scene::parseObj(f.readAll().toStdString());
    scene.setTransform(ll::Matrix4x4::rotY(M_PI_2));

    timer.setInterval(50);
    timer.start();

    cam.move(0, 2, 5);

    static ll::Camera sunCam;

    static const float sunRotX = -M_PI / 4;
    static float sunRotY;

    drawArea2->setAllowDragging(false);
    connect(drawArea1, &Frame::dragMove, [this](int dx, int dy) {
        cam.rotateX(dy / 1000.f);
        cam.rotateY(dx / 1000.f);
    });

    connect(reset, &QPushButton::clicked, [this]() {
        cam.reset();
        cam.move(0, 2, 5);
    });

    connect(zoomSB, &QDoubleSpinBox::textChanged, [this]() {
        setWindowTitle(QString("Zoom %1").arg(zoomSB->value()));
    });

    drawArea1->setDrawer([this](ll::DrawAPI& drawAPi, float elapsed) {
        drawAPi.setFragmentShader([&](const ll::Fragment& frag, const ll::Sampler* sampler) {
            auto lightFrag = (drawArea2->getToScreen() * drawArea2->getViewProjection() * frag.world).toHomogenous();
            int xx = lightFrag.x;
            int yy = lightFrag.y;
            if (xx < 0 || xx >= drawArea2->getFb().getW() || yy < 0 || yy >= drawArea2->getFb().getH()) {
                return ll::Color(1, 0, 1);
            }

            float delta = 0.01;
            if (lightFrag.z < drawArea2->getFb().getZ(xx, yy) + delta) {
                return frag.color;
            }
            return ll::Color(0, 0, 0);
        });

        scene.draw(drawAPi);

        drawAPi.setFragmentShader([&](const ll::Fragment& frag, const ll::Sampler* sampler) {
            return frag.color;
        });

        {
            auto wrapper = drawAPi.saveTransform();
            drawAPi.pushModelMatrix(ll::Matrix4x4::translation(sunCam.getPos()));
            drawAPi.pushModelMatrix(ll::Matrix4x4::rotY(sunRotY));
            drawAPi.pushModelMatrix(ll::Matrix4x4::rotX(sunRotX));
            drawAPi.drawCube(ll::Vector4::position(0, 0, 0), 0.2, ll::Color(1, 1, 1));
            drawAPi.drawLinesCube(ll::Vector4::position(0, 0, 0), 0.2007, ll::Color(0, 0, 0));
        }

        {
            auto wrapper = drawAPi.saveTransform();
            drawAPi.pushModelMatrix(drawArea2->getViewProjection().inverse());
            drawAPi.drawLinesCube(ll::Vector4::position(0, 0, 0), 2, ll::Color(1, 0, 1));
        }

        ll::Vertex v000{ll::Color(0, 0, 0), ll::Vector4::position(0, 0, 0)};
        ll::Vertex v001{ll::Color(0, 0, 1), ll::Vector4::position(0, 0, 2)};
        ll::Vertex v010{ll::Color(0, 1, 0), ll::Vector4::position(0, 2, 0)};
        ll::Vertex v100{ll::Color(1, 0, 0), ll::Vector4::position(2, 0, 0)};
        drawAPi.addShapes<ll::Line>({
            { v000, v001 },
            { v000, v010 },
            { v000, v100 },
        });
    });

    drawArea2->setDrawer([](ll::DrawAPI& drawAPi, float elapsed){
        drawAPi.setFragmentShader([](const ll::Fragment& frag, const ll::Sampler* sampler) {
            float d = (frag.z + 1) / 2;
            return ll::Color(1 - d, 1 - d, 1 - d);
        });

        scene.draw(drawAPi);
    });

    connect(&timer, &QTimer::timeout, [this]() {
        float step = 0.1;

        if(keys[Qt::Key_W]) {
            cam.move(0, 0, -step);
        }

        if(keys[Qt::Key_A]) {
            cam.move(-step, 0, 0);
        }

        if(keys[Qt::Key_S]) {
            cam.move(0, 0, step);
        }

        if(keys[Qt::Key_D]) {
            cam.move(step, 0, 0);
        }

        if(keys[Qt::Key_X]) {
            cam.move(0, step, 0);
        }

        if(keys[Qt::Key_Z]) {
            cam.move(0, -step, 0);
        }


        static float elapsed = 0;
        using namespace std::chrono;
        auto start = high_resolution_clock::now();

        auto perspective =  ll::Matrix4x4::perspective(M_PI / 2 / std::pow(1.3, zoomSB->value()), 320.f / 240.f, 0.1, 20);

        sunRotY = -M_PI * elapsed * 0.1;
        auto sunPos = ll::Matrix4x4::translation(0, 1.45, 0) * ll::Matrix4x4::rotY(sunRotY) * ll::Vector4::position(0, 0, 1);
        sunCam.reset();
        sunCam.moveTo(sunPos);
        sunCam.rotateX(sunRotX);
        sunCam.rotateY(sunRotY);

        auto sunPerspective = ll::Matrix4x4::perspective(M_PI / 3, 320.f / 240.f, 0.5, 2.75);

        drawArea2->drawFrame(ll::Color::greyscale(0), sunPerspective, sunCam, elapsed);
        drawArea1->drawFrame(ll::Color::greyscale(0.7), perspective, cam, elapsed);

        auto stop = high_resolution_clock::now();
        qDebug() << "time spent" << duration_cast<microseconds>(stop - start).count();
        elapsed += 0.02;
    });
}

void MainWindow::wheelEvent(QWheelEvent* event) {
    zoomSB->setValue(zoomSB->value() + event->angleDelta().y() / 1200.f);
    setWindowTitle(QString("Zoom %1").arg(zoomSB->value()));
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    keys[event->nativeVirtualKey()] = true;
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
    keys[event->nativeVirtualKey()] = false;
}

