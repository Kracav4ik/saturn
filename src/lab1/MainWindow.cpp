#include "MainWindow.h"

#include "OpenLL/Sampler.h"
#include <QDebug>
#include <QWheelEvent>

MainWindow::MainWindow()
    : fbs{{320, 240}, {320, 240}, {320, 240}, {320, 240}}
{
    setupUi(this);

    timer.setInterval(100);
    timer.start();

    connect(cwRB, &QRadioButton::clicked, [this]() {
        drawAPi.setCullMode(ll::CullMode::DrawCW);
    });

    connect(ccwRB, &QRadioButton::clicked, [this]() {
        drawAPi.setCullMode(ll::CullMode::DrawCCW);
    });

    connect(bothRB, &QRadioButton::clicked, [this]() {
        drawAPi.setCullMode(ll::CullMode::DrawBoth);
    });

    connect(zoomSB, &QDoubleSpinBox::textChanged, [this]() {
        setWindowTitle(QString("Zoom %1").arg(zoomSB->value()));
    });
    
    connect(&timer, &QTimer::timeout, [this]() {
        static float a = 0;
        static QGraphicsScene scene1;
        static QGraphicsScene scene2;
        static QGraphicsScene scene3;
        static QGraphicsScene scene4;
//        auto dynamicRot = ll::Matrix4x4::rotX(M_PI / 4 * sinf(a)) * ll::Matrix4x4::rotY(M_PI / 4 * sinf(a));
        auto dynamicRot = ll::Matrix4x4::rotZ(0);
        drawScene(fbs[0], drawArea1, dynamicRot, ll::Matrix4x4::rotX(M_PI_4) * ll::Matrix4x4::rotY(-M_PI_4), true, scene1);
        drawScene(fbs[1], drawArea2, dynamicRot, ll::Matrix4x4::rotX(-M_PI_2), false, scene2);
        drawScene(fbs[2], drawArea3, dynamicRot, ll::Matrix4x4::rotY(M_PI_2), false, scene3);
        drawScene(fbs[3], drawArea4, dynamicRot, ll::Matrix4x4::rotZ(0), false, scene4);
        a += 0.1;
    });
}

void MainWindow::wheelEvent(QWheelEvent* event) {
    zoomSB->setValue(zoomSB->value() - event->angleDelta().y() / 1200.f);
    setWindowTitle(QString("Zoom %1").arg(zoomSB->value()));
}

void MainWindow::drawScene(ll::Framebuffer& fb, QGraphicsView* drawArea, ll::Matrix4x4 dynamicRot, ll::Matrix4x4 staticRot, bool isPerspective, QGraphicsScene& scene) {
    drawAPi.reset();

    drawAPi.clear(fb, ll::Color(1, 1, 0));
    drawAPi.pushMatrix(ll::Matrix4x4::toScreenSpace(320, 240));

    if (isPerspective) {
        drawAPi.pushMatrix(ll::Matrix4x4::perspective(M_PI / 2, 320.f / 240.f, 3, -4));
    } else {
        drawAPi.pushMatrix(ll::Matrix4x4::ortho(2.5 * 320.f / 240.f, 2.5, -0.5 * 320.f / 240.f, -0.5, 3, -3));
    }

    drawAPi.pushMatrix(ll::Matrix4x4::lookAt(
            ll::Vector4::position(0, 0, zoomSB->value()),
            ll::Vector4::position(0, 0, 0),
            ll::Vector4::direction(0, 1, 0))
    );

    drawAPi.setFragmentShader([&](const ll::Vertex& vert, const ll::Sampler* sampler) {
        return vert.color;
    });

    drawAPi.pushMatrix(staticRot);

    {
        auto wrapper = drawAPi.saveTransform();

        drawAPi.pushMatrix(ll::Matrix4x4::translation(0.5, 0.5, 0.5));
        drawAPi.pushMatrix(ll::Matrix4x4::translation(0.5, 0.5, 0.5));
        drawAPi.pushMatrix(dynamicRot);
        drawAPi.pushMatrix(ll::Matrix4x4::translation(-0.5, -0.5, -0.5));

        ll::Vertex v000{ll::Color(0, 0, 0), ll::Vector4::position(0, 0, 0)};
        ll::Vertex v001{ll::Color(0, 0, 1), ll::Vector4::position(0, 0, 1)};
        ll::Vertex v010{ll::Color(0, 1, 0), ll::Vector4::position(0, 1, 0)};
        ll::Vertex v011{ll::Color(0, 1, 1), ll::Vector4::position(0, 1, 1)};
        ll::Vertex v100{ll::Color(1, 0, 0), ll::Vector4::position(1, 0, 0)};
        ll::Vertex v101{ll::Color(1, 0, 1), ll::Vector4::position(1, 0, 1)};
        ll::Vertex v110{ll::Color(1, 1, 0), ll::Vector4::position(1, 1, 0)};
        ll::Vertex v111{ll::Color(1, 1, 1), ll::Vector4::position(1, 1, 1)};

        auto getTrianglesFromRect = [&](ll::Vertex lb, ll::Vertex lt, ll::Vertex rt, ll::Vertex rb) {
            return std::vector<ll::Triangle> {
                    ll::Triangle{{ rt, lt, lb }},
                    ll::Triangle{{ rt, rb, lt }},
            };
        };

        drawAPi.addTriangles(getTrianglesFromRect(v010, v110, v000, v100));
        drawAPi.addTriangles(getTrianglesFromRect(v000, v100, v001, v101));
        drawAPi.addTriangles(getTrianglesFromRect(v000, v001, v010, v011));
        drawAPi.addTriangles(getTrianglesFromRect(v011, v111, v010, v110));
        drawAPi.addTriangles(getTrianglesFromRect(v001, v101, v011, v111));
        drawAPi.addTriangles(getTrianglesFromRect(v110, v111, v100, v101));
    //*/
    }
    float width = 0.1;
    drawCenter(width, {0, 0, 0}, {0, 0, 0});

    float scaleValue = 20;
    float w2 = width / 2;

    {
        auto wrapper = drawAPi.saveTransform();

        drawAPi.pushMatrix(ll::Matrix4x4::translation(w2, 0, 0));
        drawAPi.pushMatrix(ll::Matrix4x4::scale(scaleValue, 1, 1));
        drawAPi.pushMatrix(ll::Matrix4x4::translation(w2, 0, 0));
        drawCenter(width, {0, 0, 0}, {1, 0, 0});
    }
    {
        auto wrapper = drawAPi.saveTransform();

        drawAPi.pushMatrix(ll::Matrix4x4::translation(0, w2, 0));
        drawAPi.pushMatrix(ll::Matrix4x4::scale(1, scaleValue, 1));
        drawAPi.pushMatrix(ll::Matrix4x4::translation(0, w2, 0));
        drawAPi.pushMatrix(ll::Matrix4x4::rotZ(M_PI_2));
        drawCenter(width, {0, 0, 0}, {0, 1, 0});
    }

    {
        auto wrapper = drawAPi.saveTransform();

        drawAPi.pushMatrix(ll::Matrix4x4::translation(0, 0, w2));
        drawAPi.pushMatrix(ll::Matrix4x4::scale(1, 1, scaleValue));
        drawAPi.pushMatrix(ll::Matrix4x4::translation(0, 0, w2));
        drawAPi.pushMatrix(ll::Matrix4x4::rotY(-M_PI_2));
        drawCenter(width, {0, 0, 0}, {0, 0, 1});
    }

    drawAPi.drawFrame(fb);

    auto pixels = fb.getColorsARGB32();
    auto img = QImage((const uchar*)pixels.data(), fb.getW(), fb.getH(), QImage::Format_ARGB32);
    auto pm = QPixmap::fromImage(img);

    scene.clear();
    scene.addPixmap(pm);
    drawArea->setScene(&scene);
    drawArea->show();
}

void MainWindow::drawCenter(float width, ll::Color colorLeft, ll::Color colorRight) {
    float w2 = width/2;
    ll::Vertex v000{colorLeft, ll::Vector4::position(-w2, -w2, -w2)};
    ll::Vertex v001{colorLeft, ll::Vector4::position(-w2, -w2, w2)};
    ll::Vertex v010{colorLeft, ll::Vector4::position(-w2, w2, -w2)};
    ll::Vertex v011{colorLeft, ll::Vector4::position(-w2, w2, w2)};
    ll::Vertex v100{colorRight, ll::Vector4::position(w2, -w2, -w2)};
    ll::Vertex v101{colorRight, ll::Vector4::position(w2, -w2, w2)};
    ll::Vertex v110{colorRight, ll::Vector4::position(w2, w2, -w2)};
    ll::Vertex v111{colorRight, ll::Vector4::position(w2, w2, w2)};

    auto getTrianglesFromRect = [&](ll::Vertex lb, ll::Vertex lt, ll::Vertex rt, ll::Vertex rb) {
        return std::vector<ll::Triangle> {
                ll::Triangle{{ rt, lt, lb }},
                ll::Triangle{{ rt, rb, lt }},
        };
    };

    drawAPi.addTriangles(getTrianglesFromRect(v010, v110, v000, v100));
    drawAPi.addTriangles(getTrianglesFromRect(v000, v100, v001, v101));
    drawAPi.addTriangles(getTrianglesFromRect(v000, v001, v010, v011));
    drawAPi.addTriangles(getTrianglesFromRect(v011, v111, v010, v110));
    drawAPi.addTriangles(getTrianglesFromRect(v001, v101, v011, v111));
    drawAPi.addTriangles(getTrianglesFromRect(v110, v111, v100, v101));
}
