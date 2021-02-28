#include "MainWindow.h"

#include "OpenLL/Sampler.h"
#include <QDebug>
#include <QWheelEvent>

MainWindow::MainWindow()
    : cullMode(ll::CullMode::DrawCW)
{
    setupUi(this);

    timer.setInterval(100);
    timer.start();

    connect(reset, &QPushButton::clicked, [this]() {
        drawArea1->reset();
        drawArea2->reset();
        drawArea3->reset();
        drawArea4->reset();
    });

    connect(cwRB, &QRadioButton::clicked, [this]() {
        cullMode = ll::CullMode::DrawCW;
    });

    connect(ccwRB, &QRadioButton::clicked, [this]() {
        cullMode = ll::CullMode::DrawCCW;
    });

    connect(bothRB, &QRadioButton::clicked, [this]() {
        cullMode = ll::CullMode::DrawBoth;
    });

    connect(zoomSB, &QDoubleSpinBox::textChanged, [this]() {
        setWindowTitle(QString("Zoom %1").arg(zoomSB->value()));
    });

    auto drawFunc = [this](ll::DrawAPI& drawAPi, float angle){
        drawAPi.setCullMode(cullMode);
        drawAPi.setFragmentShader([&](const ll::Vertex& vert, const ll::Sampler* sampler) {
            return vert.color;
        });

        {
            auto wrapper = drawAPi.saveTransform();

            drawAPi.pushMatrix(ll::Matrix4x4::translation(0.5, 0.5, 0.5));
            drawAPi.pushMatrix(ll::Matrix4x4::translation(0.5, 0.5, 0.5));
//            drawAPi.pushMatrix(ll::Matrix4x4::rotX(M_PI / 4 * sinf(angle)) * ll::Matrix4x4::rotY(M_PI / 4 * sinf(angle)));
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
        }
        float width = 0.1;
        drawCenter(drawAPi, width, {0, 0, 0}, {0, 0, 0});

        float scaleValue = 20;
        float w2 = width / 2;

        {
            auto wrapper = drawAPi.saveTransform();

            drawAPi.pushMatrix(ll::Matrix4x4::translation(w2, 0, 0));
            drawAPi.pushMatrix(ll::Matrix4x4::scale(scaleValue, 1, 1));
            drawAPi.pushMatrix(ll::Matrix4x4::translation(w2, 0, 0));
            drawCenter(drawAPi, width, {0, 0, 0}, {1, 0, 0});
        }
        {
            auto wrapper = drawAPi.saveTransform();

            drawAPi.pushMatrix(ll::Matrix4x4::translation(0, w2, 0));
            drawAPi.pushMatrix(ll::Matrix4x4::scale(1, scaleValue, 1));
            drawAPi.pushMatrix(ll::Matrix4x4::translation(0, w2, 0));
            drawAPi.pushMatrix(ll::Matrix4x4::rotZ(M_PI_2));
            drawCenter(drawAPi, width, {0, 0, 0}, {0, 1, 0});
        }

        {
            auto wrapper = drawAPi.saveTransform();

            drawAPi.pushMatrix(ll::Matrix4x4::translation(0, 0, w2));
            drawAPi.pushMatrix(ll::Matrix4x4::scale(1, 1, scaleValue));
            drawAPi.pushMatrix(ll::Matrix4x4::translation(0, 0, w2));
            drawAPi.pushMatrix(ll::Matrix4x4::rotY(-M_PI_2));
            drawCenter(drawAPi, width, {0, 0, 0}, {0, 0, 1});
        }
    };

    drawArea1->setDrawer(drawFunc);
    drawArea2->setDrawer(drawFunc);
    drawArea3->setDrawer(drawFunc);
    drawArea4->setDrawer(drawFunc);

    connect(&timer, &QTimer::timeout, [this]() {
        static float a = 0;

        auto lookAt = ll::Matrix4x4::lookAt(
                ll::Vector4::position(0, 0, zoomSB->value()),
                ll::Vector4::position(0, 0, 0),
                ll::Vector4::direction(0, 1, 0)
        );

        auto perspective =  ll::Matrix4x4::perspective(M_PI / 2, 320.f / 240.f, 3, -4);
        auto ortho =  ll::Matrix4x4::ortho(2.5 * 320.f / 240.f, 2.5, -0.5 * 320.f / 240.f, -0.5, 3, -3);

        drawArea1->drawFrame(perspective * lookAt, ll::Matrix4x4::rotX(M_PI_4) * ll::Matrix4x4::rotY(-M_PI_4), a);
        drawArea2->drawFrame(ortho * lookAt, ll::Matrix4x4::rotX(-M_PI_2), a);
        drawArea3->drawFrame(ortho * lookAt, ll::Matrix4x4::rotY(M_PI_2), a);
        drawArea4->drawFrame(ortho * lookAt, ll::Matrix4x4::rotZ(0), a);

        a += 0.1;
    });
}

void MainWindow::wheelEvent(QWheelEvent* event) {
    zoomSB->setValue(zoomSB->value() - event->angleDelta().y() / 1200.f);
    setWindowTitle(QString("Zoom %1").arg(zoomSB->value()));
}

void MainWindow::drawCenter(ll::DrawAPI& drawAPi, float width, ll::Color colorLeft, ll::Color colorRight) {
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
