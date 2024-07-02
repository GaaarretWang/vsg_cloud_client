#pragma once
#include <QtWidgets>
#include <QtWidgets/QMainWindow>
#ifdef _WIN32
#include <vsg/platform/win32/Win32_Window.h>
#else
#include <vsg/platform/xcb/Xcb_Window.h>
#endif
#include "ui_mainWindowApplication.h"
#include "matWindow.h"
#include "ARWidget.h"

#include "interactionHandler.cpp"
#include "cameraHandler.cpp"
#include "stateSwitch.cpp"

#include "receiveServer.h"

class mainWindowApplication : public QMainWindow
{
    Q_OBJECT

public:
    mainWindowApplication(QWidget *parent = nullptr);
    ~mainWindowApplication();

private slots:
    void on_Action_CameraUpdate(int status); // switch view(up,front,left,right) according to input status 
    void on_Action_Edit(); // switch mode(observer, editor)
    void on_Action_Wireframe(); // switch representing mode(normal, wireframe)
    void on_Action_Explode(); //switch representing mode(normal, explosion)
    void on_Action_Read_fb(); // read .fb file
    void on_Action_Read_step(); // read .step file
    void on_Action_MaterialLib(); // initialize material lib window
    void on_Action_AR(); // Fusion of virtual and real
    
private:

    // 1. qt
    Ui::mainWindowApplicationClass ui; //mainWindow ui
    matWindow *matLibWindow; // material lib window pointer
    ARWidget* ARPresentWidget; // AR Window pointer

    QMenuBar* myMenuBar;
    QToolBar* myToolBar;
    QStatusBar* myStatusBar;
    QSignalMapper* mapper; // used for mapping different parameters to a single slot

    // 2. vsg
    vsg::ref_ptr<InteractionHandler> interactionHandler; // used for handling interection events
    vsg::ref_ptr<vsg::WindowTraits> windowTraits; 
    vsg::ref_ptr<vsg::Group> vsg_scene_main; //_main: main CAD model widget on the right side
    vsg::ref_ptr<vsg::Group> vsg_scene_second; //_second: second CAD element widgetv on left top 
    vsg::ref_ptr<vsg::View> view_main;
    vsg::ref_ptr<vsg::View> view_second;
    vsg::ref_ptr<vsgQt::Viewer> viewer_main;
    vsg::ref_ptr<vsgQt::Viewer> viewer_second;
    vsg::ref_ptr<vsg::Camera> camera_main;
    vsg::ref_ptr<vsg::Camera> camera_second;
    vsg::ref_ptr<vsg::Camera> cameraCloud;
    vsg::ref_ptr<CameraSelector> cameraSelect_main;
    vsg::ref_ptr<CameraSelector> cameraSelect_second;
    vsg::ref_ptr<ReceiveServer> receiveServer;

    // 3. vsgqt
    vsgQt::Window* window_main;
    vsgQt::Window* window_second;


    // 4. CADMesh related
    CADMesh mesh;
    vsg::ref_ptr<vsg::ShaderSet> mergeShader = mesh.buildIntgShader("../data/shaders/mergeCloud.vert", "../data/shaders/mergeCloud.frag");
    std::string filePath = "../data/FBDataOut/ÔËÊä³µ.fb"; //default model path
    bool is_editor = false; //false: can not edite
    bool is_explode = false;
    bool is_wireFrame = false;
    bool is_AR = false;


    // 5. methods
    void createVsgWindow(vsgQt::Window*& window,
                         vsg::ref_ptr<vsg::Group>& scene,
                         vsg::ref_ptr<vsg::View>& view,
                         vsg::ref_ptr<vsgQt::Viewer>& viewer,
                         vsg::ref_ptr<vsg::Camera>& camera,
                         vsg::ref_ptr<CameraSelector>& cameraSelect,
                         vsg::ref_ptr<vsg::WindowTraits> traits,
                         QWindow* parent,
                         const QString& title);

    void createMainVsgWindow(vsgQt::Window*& window,
                         vsg::ref_ptr<vsg::Group>& scene,
                         vsg::ref_ptr<vsg::View>& view,
                         vsg::ref_ptr<vsgQt::Viewer>& viewer,
                         vsg::ref_ptr<vsg::Camera>& camera,
                         vsg::ref_ptr<CameraSelector>& cameraSelect,
                         vsg::ref_ptr<vsg::WindowTraits> traits,
                         QWindow* parent,
                         const QString& title);

    void setUpWidgets();
    void doTreeItemSearch(QTreeWidgetItem* item);
    bool clearLayout(QGridLayout* layout);


};
