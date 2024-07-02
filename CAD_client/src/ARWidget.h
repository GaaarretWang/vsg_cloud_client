#pragma once
#include <QtWidgets>

#include <vsg/all.h>
#include <vsgQt/Viewer.h>
#include <vsgQt/Window.h>

#include <CADMesh.h>
#include <screenshot.h>
#include <iostream>
#include <ui_ARWidget.h>

class ARWidget : public QWidget
{
    Q_OBJECT

public:
    ARWidget(vsgQt::Window* in_window = nullptr,
             vsg::ref_ptr<vsg::WindowTraits> in_traits = vsg::WindowTraits::create(),
             QWidget* parent = nullptr);
    ~ARWidget();

    vsg::ref_ptr<vsg::WindowTraits> createWindowTraits();

    void updateLightInfo();
    void createCamera();

    void createVsgARWindow_1(vsgQt::Window*& window, QWindow* parent);
    void createVsgARWindow_2(vsgQt::Window*& window, QWindow* parent);
    void createVsgARWindow_3(vsgQt::Window*& window, QWindow* parent);
    void createVsgARWindow_4(vsgQt::Window*& window, QWindow* parent);

public:
    Ui::ARWidget ui;

    vsg::ref_ptr<vsg::WindowTraits> traits = vsg::WindowTraits::create();
    double nearFarRatio = 0.001;    //近平面和远平面之间的比例
    int numShadowMapsPerLight = 10; //每个光源的阴影贴图数量
    int numLights = 2;              //光源数量
    bool fullNormal = 1;
    const std::string& path1 = "../data/FBDataOut/运输车.fb";
    //const std::string& path1 = "../data/geos/3ED_827.fb";

    CADMesh cad;
    CADMesh shadow;
    vsg::ref_ptr<vsg::ShaderSet> phongShader = cad.buildShader("../data/shaders/phong.vert", "../data/shaders/phong.frag");
    vsg::ref_ptr<vsg::ShaderSet> planeShader = cad.buildShader("../data/shaders/plane.vert", "../data/shaders/plane.frag");
    vsg::ref_ptr<vsg::ShaderSet> shadowShader = cad.buildShader("../data/shaders/shadow.vert", "../data/shaders/shadow.frag");
    vsg::ref_ptr<vsg::ShaderSet> mergeShader = cad.buildIntgShader("../data/shaders/merge.vert", "../data/shaders/merge.frag");

    vsg::ref_ptr<vsg::Group> cadScenegraph = vsg::Group::create();
    vsg::ref_ptr<vsg::Group> envScenegraph = vsg::Group::create();
    vsg::ref_ptr<vsg::Group> shadowScenegraph = vsg::Group::create();
    vsg::ref_ptr<vsg::Group> intgScenegraph = vsg::Group::create();

    vsg::ref_ptr<vsg::DirectionalLight> directionalLight = vsg::DirectionalLight::create(); //定向光源 ref_ptr智能指针
    vsg::ref_ptr<vsg::AmbientLight> ambientLight = vsg::AmbientLight::create();             //环境光
    vsg::ref_ptr<vsg::DirectionalLight> directionalLight2 = vsg::DirectionalLight::create();

    vsg::ref_ptr<vsg::Camera> camera1;
    vsg::ref_ptr<vsg::Camera> camera2;
    vsg::ref_ptr<vsg::Camera> camera3;
    vsg::ref_ptr<vsg::Camera> mergeCamera;
    vsg::ref_ptr<vsg::Trackball> trackball1;
    vsg::ref_ptr<vsg::Trackball> trackball2;
    vsg::ref_ptr<vsg::Trackball> trackball3;

    vsg::ref_ptr<vsg::Image> storageImages[6];
    vsg::ref_ptr<vsg::CopyImage> copyImages[6];
    vsg::ref_ptr<vsg::ImageInfo> imageInfos[6];

    
    QWidget* widget1;
    QWidget* widget2;
    QWidget* widget3;
    QWidget* widget4;

    vsgQt::Window* inWindow;
    vsgQt::Window* window1;
    vsgQt::Window* window2;
    vsgQt::Window* window3;
    vsgQt::Window* window4;

    vsg::ref_ptr<vsgQt::Viewer> viewer1 = vsgQt::Viewer::create();
    vsg::ref_ptr<vsgQt::Viewer> viewer2 = vsgQt::Viewer::create();
    vsg::ref_ptr<vsgQt::Viewer> viewer3 = vsgQt::Viewer::create();
    vsg::ref_ptr<vsgQt::Viewer> viewer4 = vsgQt::Viewer::create();
};
