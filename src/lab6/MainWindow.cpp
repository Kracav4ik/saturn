#include "MainWindow.h"

#include "OpenLL/Scene.h"
#include "OpenLL/Fragment.h"
#include <QDebug>
#include <QWheelEvent>

MainWindow::MainWindow()
    : model(std::make_shared<Model>())
{
    setupUi(this);

    QFile f(":/low-poly-mill.obj");

    f.open(QIODevice::ReadOnly | QIODevice::Text);

    static ll::Scene scene = ll::Scene::parseObj(f.readAll().toStdString());
    scene.setTransform(ll::Matrix4x4::rotY(M_PI_2));

    timer.setInterval(100);
    timer.start();

    static auto lightMat = ll::Matrix4x4::identity();

    drawArea1->setModel(model);
    drawArea2->setModel(model);
    drawArea2->setIsLight(true);
    connect(drawArea1, &Frame::press, model.get(), &Model::press);
    connect(drawArea1, &Frame::pressRight, model.get(), &Model::pressRight);
    connect(drawArea1, &Frame::move, model.get(), &Model::move);
    connect(drawArea1, &Frame::release, model.get(), &Model::release);

    connect(reset, &QPushButton::clicked, [this]() {
        drawArea1->reset();
    });

    connect(zoomSB, &QDoubleSpinBox::textChanged, [this]() {
        setWindowTitle(QString("Zoom %1").arg(zoomSB->value()));
    });

    drawArea1->setDrawer([this](ll::DrawAPI& drawAPi, float angle) {
        drawAPi.setFragmentShader([&](const ll::Fragment& frag, const ll::Sampler* sampler) {
            auto lightFrag = (lightMat * drawAPi.getMatrix().inverse() * ll::Vector4::position(frag.x, frag.y, frag.z)).toHomogenous();
//                auto lightFrag = (lightMat * vert.world).toHomogenous();
            int xx = lightFrag.x;
            int yy = lightFrag.y;
            if (0 > xx || xx >= drawArea2->getFb().getW() || 0 > yy || yy >= drawArea2->getFb().getH()) {
                return ll::Color(1, 0, 0);
            }

            if (std::abs(drawArea2->getFb().getZ(xx, yy) - lightFrag.z) < 0.01) {
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
            drawAPi.pushMatrix(ll::Matrix4x4::translation(0, 1.5, 0));
            drawAPi.pushMatrix(ll::Matrix4x4::rotY(M_PI * angle * 0.1));
            drawAPi.pushMatrix(ll::Matrix4x4::translation(0, 0, 1));
            drawAPi.drawCube(ll::Vector4::position(0, 0, 0), 0.1, ll::Color(1, 1, 1));
        }

        ll::Vertex v000{ll::Color(0, 0, 0), ll::Vector4::position(0, 0, 0)};
        ll::Vertex v001{ll::Color(0, 0, 1), ll::Vector4::position(0, 0, 1.5)};
        ll::Vertex v010{ll::Color(0, 1, 0), ll::Vector4::position(0, 1.5, 0)};
        ll::Vertex v100{ll::Color(1, 0, 0), ll::Vector4::position(1.5, 0, 0)};
        drawAPi.addShapes<ll::Line>({
            { v000, v001 },
            { v000, v010 },
            { v000, v100 },
        });
    });

    drawArea2->setDrawer([](ll::DrawAPI& drawAPi, float angle){
        lightMat = drawAPi.getMatrix();
        drawAPi.setFragmentShader([](const ll::Fragment& frag, const ll::Sampler* sampler) {
            float d = (frag.z + 1) / 2;
            return ll::Color(1 - d, 1 - d, 1 - d);
        });

        scene.draw(drawAPi);
    });

    connect(&timer, &QTimer::timeout, [this]() {
        static float a = 0;

        auto lookAt = ll::Matrix4x4::lookAt(
                ll::Vector4::position(0, 0, 3),
                ll::Vector4::position(0, 0, 0),
                ll::Vector4::direction(0, 1, 0)
        );

        auto perspective =  ll::Matrix4x4::perspective(M_PI / 2 / std::pow(1.2, zoomSB->value()), 320.f / 240.f, 1, 10);
        auto ortho =  ll::Matrix4x4::ortho(2.5 * 320.f / 240.f, 2.5, -0.5 * 320.f / 240.f, -0.5, 3, -3);

        auto sunLookAt = ll::Matrix4x4::lookAt(
            ll::Vector4::position(0, 1.5, 1),
            ll::Vector4::position(0, 0, 0),
            ll::Vector4::direction(0, 1, 0)
        );

        auto sunPerspective = ll::Matrix4x4::perspective(M_PI / 2, 320.f / 240.f, 0.5, 10);

        drawArea2->drawFrame(sunPerspective, sunLookAt,
                             ll::Matrix4x4::translation(0, 1.5, 0) * ll::Matrix4x4::rotY(-M_PI * a * 0.1) * ll::Matrix4x4::translation(0, -1.5, 0)
                             , a);
        drawArea1->drawFrame(perspective, lookAt,
//                             ll::Matrix4x4::identity()
                             ll::Matrix4x4::rotX(M_PI_4) * ll::Matrix4x4::rotY(-M_PI_4)
                             , a);

        a += 0.1;
    });
}

void MainWindow::wheelEvent(QWheelEvent* event) {
    zoomSB->setValue(zoomSB->value() + event->angleDelta().y() / 1200.f);
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
                ll::Triangle{ rt, lt, lb },
                ll::Triangle{ rt, rb, lt },
        };
    };

    for (const auto& triangles : {getTrianglesFromRect(v010, v110, v000, v100), getTrianglesFromRect(v000, v100, v001, v101),
                                  getTrianglesFromRect(v000, v001, v010, v011), getTrianglesFromRect(v011, v111, v010, v110),
                                  getTrianglesFromRect(v001, v101, v011, v111), getTrianglesFromRect(v110, v111, v100, v101)}) {
        drawAPi.addShapes(triangles);
    }
}
