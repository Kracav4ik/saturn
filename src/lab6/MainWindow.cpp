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

    timer.setInterval(100);
    timer.start();

    auto lightMat = ll::Matrix4x4::identity();


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

    auto initFunc = [&](ll::DrawAPI& drawAPi, bool isLight) {
        if (isLight) {
            lightMat = drawAPi.getMatrix();
        }
        drawAPi.setFragmentShader([&](const ll::Fragment& vert, const ll::Sampler* sampler, std::shared_ptr<ll::Framebuffer> lightFb) {
            if (!zBuffer->isChecked()) {
                return vert.color;
            }
            float d = ((-vert.z) - 0.77f) * 1.7f;
            if (d < 0.5) {
                return 2 * d * ll::Color(0, 1, 0) + (1 - 2 * d) * ll::Color(0, 0, 1);
            }
            d -= 0.5;
            return 2 * d * ll::Color(1, 0, 0) + (1 - 2 * d) * ll::Color(0, 1, 0);
        });
    };

    auto drawFunc = [&](ll::DrawAPI& drawAPi, bool isLight, float angle){
        if (isLight) {
            drawAPi.setFragmentShader([&](const ll::Fragment& vert, const ll::Sampler* sampler, std::shared_ptr<ll::Framebuffer> lightFb) {
                float d = ((-vert.z) - 0.77f) * 1.7f;
                if (d < 0.5) {
                    return 2 * d * ll::Color(1, 1, 1) + (1 - 2 * d) * ll::Color(0.5, 0.5, 0.5);
                }
                d -= 0.5;
                return 2 * d * ll::Color(0.5, 0.5, 0.5) + (1 - 2 * d) * ll::Color(0, 0, 0);
            });
        } else {
            drawAPi.setFragmentShader([&](const ll::Fragment& vert, const ll::Sampler* sampler, const std::shared_ptr<ll::Framebuffer>& lightFb) {
//                auto lightFrag = drawAPi.getMatrix().inverse() * lightMat * ll::Vector4::position(vert.x, vert.y, vert.z);
//                if (lightFb.getZ(lightFrag.x, lightFrag.y) - lightFrag.z < 0.5) {
//                    return vert.color;
//                }
                return ll::Color(0, 0, 0);
            });

            {
                auto wrapper = drawAPi.saveTransform();
                drawAPi.pushMatrix(ll::Matrix4x4::translation(0, 2, 0));
                drawAPi.pushMatrix(ll::Matrix4x4::rotY(M_PI * angle * 0.3));
                drawAPi.pushMatrix(ll::Matrix4x4::translation(0, 0, 1));
                drawAPi.drawCube(ll::Vector4::position(0, 0, 0), 0.25, ll::Color(1, 1, 1));
            }
        }

        {
            auto wrapper = drawAPi.saveTransform();
            drawAPi.pushMatrix(ll::Matrix4x4::rotY(M_PI_2));
            scene.draw(drawAPi);
        }

        {
            ll::Vertex v000{ll::Color(0, 0, 0), ll::Vector4::position(0, 0, 0)};
            ll::Vertex v001{ll::Color(0, 0, 1), ll::Vector4::position(0, 0, 1.5)};
            ll::Vertex v010{ll::Color(0, 1, 0), ll::Vector4::position(0, 1.5, 0)};
            ll::Vertex v100{ll::Color(1, 0, 0), ll::Vector4::position(1.5, 0, 0)};
            drawAPi.addShapes<ll::Line>({
                    { v000, v001 },
                    { v000, v010 },
                    { v000, v100 },
            });
        }
    };

    drawArea1->setIniter(initFunc);
    drawArea1->setDrawer(drawFunc);
    drawArea2->setIniter(initFunc);
    drawArea2->setDrawer(drawFunc);

    connect(&timer, &QTimer::timeout, [this]() {
        static float a = 0;

        auto lookAt = ll::Matrix4x4::lookAt(
                ll::Vector4::position(0, 0, 3),
                ll::Vector4::position(0, 0, 0),
                ll::Vector4::direction(0, 1, 0)
        );

        auto perspective =  ll::Matrix4x4::perspective(M_PI / 2 * (- std::pow(1.3, zoomSB->value())), 320.f / 240.f, 3, -4);
        auto ortho =  ll::Matrix4x4::ortho(2.5 * 320.f / 240.f, 2.5, -0.5 * 320.f / 240.f, -0.5, 3, -3);

        drawArea1->drawFrame(perspective, lookAt, ll::Matrix4x4::rotX(M_PI_4) * ll::Matrix4x4::rotY(-M_PI_4), a);

        auto sunLookAt = ll::Matrix4x4::lookAt(
            ll::Vector4::position(0, 2, 1),
            ll::Vector4::position(0, 0, 0),
            ll::Vector4::direction(0, 1, 0)
    );
        auto sunPerspective =  ll::Matrix4x4::perspective(M_PI / 2 * (- std::pow(1.3, 3.6f)), 320.f / 240.f, 3, -4);

        drawArea2->drawFrame(sunPerspective, sunLookAt, ll::Matrix4x4::translation(0, 2, 0) * ll::Matrix4x4::rotY(-M_PI * a * 0.3) * ll::Matrix4x4::translation(0, -2, 0), a);

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
