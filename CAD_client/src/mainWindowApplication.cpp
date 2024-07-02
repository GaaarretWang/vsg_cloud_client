#include "mainWindowApplication.h"


mainWindowApplication::mainWindowApplication(QWidget *parent)
    : QMainWindow(parent)
{
    
    ui.setupUi(this);

    ///create basic bars///

    // 1. create icon------------------------------------------------------------------------------------------------
    setWindowIcon(QIcon("../data/images/myappiteam.png"));
    this->setWindowTitle(QString::fromLocal8Bit("CAD渲染编辑系统"));
    ui.label->setStyleSheet("color:#2F4F2F;");
    
    
    // 2. create menubar------------------------------------------------------------------------------------------------
    myMenuBar = new QMenuBar(this);
    this->setMenuBar(myMenuBar);// add menubar to main window
    QMenu* menu1 = new QMenu(QString::fromLocal8Bit("文件"), this);
    QAction* menu1_readfb = new QAction(QString::fromLocal8Bit(".fb格式"));
    QAction* menu1_readstep = new QAction(QString::fromLocal8Bit(".step格式"));
    menu1->addAction(menu1_readfb);
    menu1->addAction(menu1_readstep);

    QMenu* menu2 = new QMenu(QString::fromLocal8Bit("视图"), this);
    QMenu* menu2_style = new QMenu(QString::fromLocal8Bit("视图风格"));
    QMenu* menu2_direction = new QMenu(QString::fromLocal8Bit("视图方位"));
    QAction* menu2_direction_1 = new QAction(QString::fromLocal8Bit("顶视图"));
    QAction* menu2_direction_2 = new QAction(QString::fromLocal8Bit("前视图"));
    QAction* menu2_direction_3 = new QAction(QString::fromLocal8Bit("左视图"));
    QAction* menu2_direction_4 = new QAction(QString::fromLocal8Bit("右视图"));
    menu2_direction->addAction(menu2_direction_1);
    menu2_direction->addAction(menu2_direction_2);
    menu2_direction->addAction(menu2_direction_3);
    menu2_direction->addAction(menu2_direction_4);
    menu2->addMenu(menu2_style);
    menu2->addMenu(menu2_direction);

    QMenu* menu3 = new QMenu(QString::fromLocal8Bit("材质库"), this);
    QMenu* menu4 = new QMenu(QString::fromLocal8Bit("帮助"), this);
    myMenuBar->addMenu(menu1);
    myMenuBar->addMenu(menu2);
    myMenuBar->addMenu(menu3);
    myMenuBar->addMenu(menu4);


    // 3. create toolbar------------------------------------------------------------------------------------------------
    myToolBar = new QToolBar(this);
    this->addToolBar(Qt::TopToolBarArea, myToolBar);// add toolbar to main window
    myToolBar->setFloatable(false);
    myToolBar->setMovable(false);
    myToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);//text under icon

    QFont font("Microsoft YaHei", 10, 50, false);
    myToolBar->setFont(font);
    QAction* tool1 = new QAction(QIcon("..\\data\\images\\edit.svg"), QString::fromLocal8Bit("编辑"), this);
    QAction* tool2 = new QAction(QIcon("..\\data\\images\\view-iso.svg"), QString::fromLocal8Bit("线框"), this);
    QAction* tool3 = new QAction(QIcon("..\\data\\images\\xde-assembly.svg"), QString::fromLocal8Bit("爆炸"), this);
    QAction* tool4 = new QAction(QIcon("..\\data\\images\\view-front.svg"), QString::fromLocal8Bit("正视图"), this);
    QAction* tool5 = new QAction(QIcon("..\\data\\images\\view-left.svg"), QString::fromLocal8Bit("侧视图"), this);
    QAction* tool6 = new QAction(QIcon("..\\data\\images\\view-top.svg"), QString::fromLocal8Bit("顶视图"), this);
    QAction* tool7 = new QAction(QIcon("..\\data\\images\\import.svg"), QString::fromLocal8Bit("导入"), this);
    QAction* tool8 = new QAction(QIcon("..\\data\\images\\grid.svg"), QString::fromLocal8Bit("材质库"), this);
    QAction* tool9 = new QAction(QIcon("..\\data\\images\\multiple.svg"), QString::fromLocal8Bit("虚实融合"), this);

    myToolBar->addAction(tool1);
    myToolBar->addSeparator();
    myToolBar->addAction(tool2);
    myToolBar->addSeparator();
    myToolBar->addAction(tool3);
    myToolBar->addSeparator();
    myToolBar->addAction(tool4);
    myToolBar->addSeparator();
    myToolBar->addAction(tool5);
    myToolBar->addSeparator();
    myToolBar->addAction(tool6);
    myToolBar->addSeparator();
    myToolBar->addAction(tool7);
    myToolBar->addSeparator();
    myToolBar->addAction(tool8);
    myToolBar->addSeparator();
    myToolBar->addAction(tool9);

    connect(tool1, SIGNAL(triggered()), this, SLOT(on_Action_Edit()));
    connect(tool2, SIGNAL(triggered()), this, SLOT(on_Action_Wireframe()));
    connect(tool3, SIGNAL(triggered()), this, SLOT(on_Action_Explode()));

    mapper = new QSignalMapper(this); // used for mapping different parameter of the slot doCameraUpdate(int)
    connect(tool4, SIGNAL(triggered()), mapper, SLOT(map()));
    connect(tool5, SIGNAL(triggered()), mapper, SLOT(map()));
    connect(tool6, SIGNAL(triggered()), mapper, SLOT(map()));
    mapper->setMapping(tool4, 2);
    mapper->setMapping(tool5, 0);
    mapper->setMapping(tool6, 3);
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(on_Action_CameraUpdate(int)));

    connect(tool8, SIGNAL(triggered()), this, SLOT(on_Action_MaterialLib()));
    connect(tool9, SIGNAL(triggered()), this, SLOT(on_Action_AR()));
    connect(menu1_readfb, SIGNAL(triggered()), this, SLOT(on_Action_Read_fb()));
    connect(menu1_readstep, SIGNAL(triggered()), this, SLOT(on_Action_Read_step()));


    // 4. create statusbar------------------------------------------------------------------------------------------------
    myStatusBar = new QStatusBar(this);
    myStatusBar->setObjectName(QString::fromLocal8Bit("状态栏"));
    myStatusBar->setStyleSheet("QStatusBar::item{border: 0px}"); //设置不显示label的边框
    myStatusBar->setFont(font);
    this->setStatusBar(myStatusBar);
    myStatusBar->showMessage(QString::fromLocal8Bit("欢迎使用CAD编辑系统"), 10000);//showing message for 10 seconds

    ///set up content for widgets in main window///
    setUpWidgets();

}

mainWindowApplication::~mainWindowApplication()
{}


void mainWindowApplication::setUpWidgets()
{
    // 1. set up style sheet---------------------------------------------------------------------------------
    //ui.secondaryWidget->setStyleSheet(QString::fromUtf8("border:1px solid green"));
    ui.info->setReadOnly(true);

    // 2. pointer initialization
    vsg_scene_main = vsg::Group::create();
    vsg_scene_second = vsg::Group::create();
    viewer_main = vsgQt::Viewer::create();
    viewer_second = vsgQt::Viewer::create();
    windowTraits = vsg::WindowTraits::create();
    windowTraits->deviceExtensionNames.push_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
#ifdef _WIN32
    windowTraits->deviceExtensionNames.push_back(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME);
    windowTraits->deviceExtensionNames.push_back(VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME);
#else
    windowTraits->deviceExtensionNames.push_back(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME);
    windowTraits->deviceExtensionNames.push_back(VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME);
    windowTraits->deviceExtensionNames.push_back(VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME);
    windowTraits->deviceExtensionNames.push_back(VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME);
#endif

    // 3. load CAD model to vsg_scene_main 
    vsg::GeometryInfo geomInfo;
    vsg::StateInfo stateInfo;
    bool fullNormal = 1;

    receiveServer = ReceiveServer::create(640, 480);

    //mesh.buildnode(geomInfo, stateInfo, filePath, fullNormal, vsg_scene_main); //读取几何信息1
    mesh.buildCloudIntgNode(vsg_scene_main, mergeShader, receiveServer->getTextureData()); 

    // 4. initialize vsg windows
    mainWindowApplication::createMainVsgWindow(window_main, vsg_scene_main, view_main, viewer_main, 
        camera_main, cameraSelect_main, windowTraits, nullptr, "First Window");

    mainWindowApplication::createVsgWindow(window_second, vsg_scene_second, view_second, 
        viewer_second, camera_second, cameraSelect_second, windowTraits, nullptr, "Second Window");

    std::cout << "test1";
    receiveServer->initEncoder(window_main->windowAdapter);
    receiveServer->initCamera(cameraCloud);
    //receiveServer->initImageUtils(window_main->windowAdapter);

    // 5. add state switch events (normal, wireframe) and interaction events
    InsertStateSwitch rgp;
    rgp.mask_1 = 1;
    rgp.mask_2 = 2;
    vsg_scene_main->accept(rgp);
    
    interactionHandler = InteractionHandler::create(camera_main, camera_second, window_main, window_second, vsg_scene_main,
        vsg_scene_second, view_second, viewer_main, viewer_second, mesh, ui.info);
    viewer_main->addEventHandler(interactionHandler);
    
    // 6. from vsg scenes to qt widgets
    ui.mainLayout->addWidget(createWindowContainer(window_main));
    //clearLayout(ui.mainLayout);
    //ui.mainLayout->addWidget(ARPresentWidget);

    ui.secLayout_2->addWidget(createWindowContainer(window_second));
    
    // 7. initialize tree structure of CAD model
    ui.treeWidget->setHeaderHidden(true);
    QTreeWidgetItem* root = new QTreeWidgetItem();
    mesh.presentTreeforQt(root);
    ui.treeWidget->addTopLevelItem(root);
    connect(ui.treeWidget, &QTreeWidget::itemDoubleClicked, this, &mainWindowApplication::doTreeItemSearch);

}

void mainWindowApplication::createVsgWindow(vsgQt::Window*& window,
                                            vsg::ref_ptr<vsg::Group>& scene,
                                            vsg::ref_ptr<vsg::View>& view,
                                            vsg::ref_ptr<vsgQt::Viewer>& viewer,
                                            vsg::ref_ptr<vsg::Camera>& camera,
                                            vsg::ref_ptr<CameraSelector>& cameraSelect,
                                            vsg::ref_ptr<vsg::WindowTraits> traits,
                                            QWindow* parent,
                                            const QString& title)
{

    //main window
    window = new vsgQt::Window(viewer, traits, parent);
    window->setTitle(title);
    window->initializeWindow();

    // if this is the first window to be created, use its device for future window creation.
    if (!traits->device) traits->device = window->windowAdapter->getOrCreateDevice();

    auto builder = vsg::Builder::create();
    vsg::GeometryInfo geomInfo;
    vsg::StateInfo stateInfo;
    geomInfo.color.set(1.0f, 0.0f, 0.0f, 0.0f);
    geomInfo.position.set(0.0f, 0.0f, 0.0f);
    geomInfo.dx.set(10.0f, 0.0f, 0.0f);
    geomInfo.dy.set(0.0f, 10.0f, 0.0f);
    geomInfo.dz.set(0.0f, 0.0f, 10.0f);
    scene->addChild(builder->createCylinder(geomInfo, stateInfo));

    // compute the bounds of the scene graph to help position camera
    uint32_t width = window->traits->width;
    uint32_t height = window->traits->height;
    vsg::ComputeBounds computeBounds;
    scene->accept(computeBounds);
    vsg::dvec3 centre = (computeBounds.bounds.min + computeBounds.bounds.max) * 0.5;
    double radius = vsg::length(computeBounds.bounds.max - computeBounds.bounds.min) * 0.6;
    double nearFarRatio = 0.001;

    // build camera up, camera front, camera left, camera right
    auto scene_cameras = vsg::visit<vsg::FindCameras>(scene).cameras;
    auto root = vsg::Group::create();
    root->addChild(scene);
    auto perspective = vsg::Perspective::create(90.0, 1.0, nearFarRatio * radius, radius * 20.0);
    // left
    {
        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(radius * 3.5, 0.0, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));
        auto camera1 = vsg::Camera::create(perspective, lookAt);
        camera1->name = "Left";
        root->addChild(camera1);
    }
    // right
    {
        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(-radius * 3.5, 0.0, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));
        auto camera2 = vsg::Camera::create(perspective, lookAt);
        camera2->name = "Right";
        root->addChild(camera2);
    }
    // front
    {
        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(0.0, -radius * 3.5, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));
        auto camera3 = vsg::Camera::create(perspective, lookAt);
        camera3->name = "Front";
        root->addChild(camera3);
    }
    // top
    {
        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(0.0, 0.0, radius * 3.5), centre, vsg::dvec3(0.0, 1.0, 0.0));
        auto camera4 = vsg::Camera::create(perspective, lookAt);
        camera4->name = "Top";
        root->addChild(camera4);
    }
    scene = root;
    scene_cameras = vsg::visit<vsg::FindCameras>(scene).cameras; // refresh the scene_cameras list.

    //build main camera
    {
        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(0.0, -radius * 3.5, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));
        vsg::ref_ptr<vsg::ProjectionMatrix> perspective;

        perspective = vsg::Perspective::create(
            30.0,
            static_cast<double>(width) /
                static_cast<double>(height),
            nearFarRatio * radius, radius * 20.0);

        camera = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(VkExtent2D{width, height}));
    }

    //enviromment light
    auto group = vsg::Group::create();
    group->addChild(scene);
    vsg::ref_ptr<vsg::AmbientLight> ambientLight;
    ambientLight = vsg::AmbientLight::create();
    ambientLight->name = "ambient";
    ambientLight->color.set(1.0, 1.0, 1.0);
    ambientLight->intensity = 0.95;
    group->addChild(ambientLight);

    //directional lights
    auto directionalLight = vsg::DirectionalLight::create();
    directionalLight->name = "directional";
    directionalLight->color.set(1.0, 1.0, 1.0);
    directionalLight->intensity = 0.7;
    directionalLight->direction = vsg::normalize(vsg::vec3(0.9, 1.0, -1.0));
    group->addChild(directionalLight);

    auto directionalLight2 = vsg::DirectionalLight::create();
    directionalLight2->name = "2nd directional";
    directionalLight2->color.set(1.0, 1.0, 1.0);
    directionalLight2->intensity = 0.7;
    directionalLight2->direction = vsg::normalize(vsg::vec3(-0.9, -1.0, 1.0));
    group->addChild(directionalLight2);
    scene = group;

    view = vsg::View::create(camera, scene);
    view->mask = 1; //1,normal  2,wireframe
    auto renderGraph = vsg::RenderGraph::create(*window, view);
    renderGraph->clearValues[0].color = {{0.9f, 0.9f, 0.9f, 1.0f}}; //background color
    auto commandGraph = vsg::CommandGraph::create(*window, renderGraph);
    commandGraph->addChild(renderGraph);
    //auto commandGraph = vsg::createCommandGraphForView(*window, camera, scene);
    viewer->addRecordAndSubmitTaskAndPresentation({commandGraph});

    //add trackball event
    auto trackball = vsg::Trackball::create(camera);
    trackball->addWindow(*window);
    viewer->addEventHandler(trackball);

    //add camera switch event
    cameraSelect = CameraSelector::create(camera, scene_cameras);
    viewer->addEventHandler(cameraSelect);

    viewer->compile();
}

void mainWindowApplication::createMainVsgWindow(vsgQt::Window*& window,
                                            vsg::ref_ptr<vsg::Group>& scene,
                                            vsg::ref_ptr<vsg::View>& view,
                                            vsg::ref_ptr<vsgQt::Viewer>& viewer,
                                            vsg::ref_ptr<vsg::Camera>& camera,
                                            vsg::ref_ptr<CameraSelector>& cameraSelect,
                                            vsg::ref_ptr<vsg::WindowTraits> traits,
                                            QWindow* parent,
                                            const QString& title)
{
    //main window
    window = new vsgQt::Window(viewer, traits, parent);
    window->setTitle(title);
    window->initializeWindow();

    // if this is the first window to be created, use its device for future window creation.
    if (!traits->device) traits->device = window->windowAdapter->getOrCreateDevice();

    auto builder = vsg::Builder::create();
    vsg::GeometryInfo geomInfo;
    vsg::StateInfo stateInfo;
    geomInfo.color.set(1.0f, 0.0f, 0.0f, 0.0f);
    geomInfo.position.set(0.0f, 0.0f, 0.0f);
    geomInfo.dx.set(10.0f, 0.0f, 0.0f);
    geomInfo.dy.set(0.0f, 10.0f, 0.0f);
    geomInfo.dz.set(0.0f, 0.0f, 10.0f);
    scene->addChild(builder->createCylinder(geomInfo, stateInfo));

    // compute the bounds of the scene graph to help position camera
    uint32_t width = window->traits->width;
    uint32_t height = window->traits->height;
    vsg::ComputeBounds computeBounds;
    scene->accept(computeBounds);
    vsg::dvec3 centre = (computeBounds.bounds.min + computeBounds.bounds.max) * 0.5;
    double radius = vsg::length(computeBounds.bounds.max - computeBounds.bounds.min) * 0.6;
    double nearFarRatio = 0.001;

    // build camera up, camera front, camera left, camera right
    auto scene_cameras = vsg::visit<vsg::FindCameras>(scene).cameras;
    auto root = vsg::Group::create();
    root->addChild(scene);
    auto perspective = vsg::Perspective::create(60.0, 1.0, 0.1, 20000.0);
    // left
    {
        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(radius * 3.5, 0.0, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));
        auto camera1 = vsg::Camera::create(perspective, lookAt);
        camera1->name = "Left";
        root->addChild(camera1);
    }
    // right
    {
        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(-radius * 3.5, 0.0, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));
        auto camera2 = vsg::Camera::create(perspective, lookAt);
        camera2->name = "Right";
        root->addChild(camera2);
    }
    // front
    {
        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(0.0, -radius * 3.5, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));
        auto camera3 = vsg::Camera::create(perspective, lookAt);
        camera3->name = "Front";
        root->addChild(camera3);
    }
    // top
    {
        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(0.0, 0.0, radius * 3.5), centre, vsg::dvec3(0.0, 1.0, 0.0));
        auto camera4 = vsg::Camera::create(perspective, lookAt);
        camera4->name = "Top";
        root->addChild(camera4);
    }
    scene = root;
    scene_cameras = vsg::visit<vsg::FindCameras>(scene).cameras; // refresh the scene_cameras list.

    //build main camera
    {
        double radius1 = 2000.0; // 固定观察距离
        auto perspective1 = vsg::Perspective::create(60.0, static_cast<double>(traits->width) / static_cast<double>(traits->height), nearFarRatio * radius, radius * 10.0);
        vsg::dvec3 centre1 = {0.0, 0.0, 0.0};    
        vsg::dvec3 mergeEye = centre + vsg::dvec3(0.0, 0.0, radius1); // 固定相机位置
        vsg::dvec3 mergeUp = {0.0, 1.0, 0.0};                        // 固定观察方向
        auto mergelookAt = vsg::LookAt::create(mergeEye, centre1, mergeUp);
        auto viewport = vsg::ViewportState::create(0, 0, traits->width, traits->height);
        camera = vsg::Camera::create(perspective1, mergelookAt, viewport);
    }

    //enviromment light
    auto group = vsg::Group::create();
    group->addChild(scene);
    vsg::ref_ptr<vsg::AmbientLight> ambientLight;
    ambientLight = vsg::AmbientLight::create();
    ambientLight->name = "ambient";
    ambientLight->color.set(1.0, 1.0, 1.0);
    ambientLight->intensity = 0.95;
    group->addChild(ambientLight);

    //directional lights
    auto directionalLight = vsg::DirectionalLight::create();
    directionalLight->name = "directional";
    directionalLight->color.set(1.0, 1.0, 1.0);
    directionalLight->intensity = 0.7;
    directionalLight->direction = vsg::normalize(vsg::vec3(0.9, 1.0, -1.0));
    group->addChild(directionalLight);

    auto directionalLight2 = vsg::DirectionalLight::create();
    directionalLight2->name = "2nd directional";
    directionalLight2->color.set(1.0, 1.0, 1.0);
    directionalLight2->intensity = 0.7;
    directionalLight2->direction = vsg::normalize(vsg::vec3(-0.9, -1.0, 1.0));
    group->addChild(directionalLight2);
    scene = group;

    view = vsg::View::create(camera, scene);
    view->mask = 1; //1,normal  2,wireframe
    auto renderGraph = vsg::RenderGraph::create(*window, view);
    renderGraph->clearValues[0].color = {{0.9f, 0.9f, 0.9f, 1.0f}}; //background color
    auto commandGraph = vsg::CommandGraph::create(*window, renderGraph);
    commandGraph->addChild(renderGraph);
    //auto commandGraph = vsg::createCommandGraphForView(*window, camera, scene);
    viewer->addRecordAndSubmitTaskAndPresentation({commandGraph});

    {
        double radius = 2000.0; // 固定观察距离
        auto viewport = vsg::ViewportState::create(0, 0, traits->width, traits->height);
        auto perspective = vsg::Perspective::create(60.0, static_cast<double>(traits->width) / static_cast<double>(traits->height), nearFarRatio * radius, radius * 10.0);
        vsg::dvec3 centre = {0.0, 0.0, 0.0};                    // 固定观察点
        vsg::dvec3 eye = centre + vsg::dvec3(0.0, radius, 0.0); // 固定相机位置
        vsg::dvec3 up = {0.0, 0.0, 1.0};                        // 固定观察方向
        auto lookAt = vsg::LookAt::create(eye, centre, up);
        cameraCloud = vsg::Camera::create(perspective, lookAt, viewport);
    }

    //add trackball event
    auto trackball = vsg::Trackball::create(cameraCloud);
    trackball->addWindow(*window);
    viewer->addEventHandler(trackball);

    //add camera switch event
    cameraSelect = CameraSelector::create(camera, scene_cameras);
    viewer->addEventHandler(cameraSelect);
    viewer->addEventHandler(receiveServer);

    viewer->compile();
}


void mainWindowApplication::on_Action_Wireframe()
{
    if (view_main->mask == 1)
    {
        view_main->mask = 2;

        //show message on status bar
        std::string str_bar = "展示线框图";
        myStatusBar->showMessage(QString::fromLocal8Bit(str_bar.c_str()), 5000);
    }
    else view_main->mask = 1;

    viewer_main->compile();

}

void mainWindowApplication::on_Action_Explode()
{
    if (!is_explode)
    {
        mesh.explode();
        is_explode = true;

        //show message on status bar
        std::string str_bar = "展示爆炸图";
        myStatusBar->showMessage(QString::fromLocal8Bit(str_bar.c_str()), 5000);
    }
    else
    {
        mesh.recover();
        is_explode = false;
    }
}

void mainWindowApplication::on_Action_Edit()
{
    if (!is_editor)
    {
        is_editor = true;
        interactionHandler->editable = is_editor;
        ui.label->setText(QString::fromLocal8Bit("编辑模式"));

        //clear highlighted
        mesh.repaint(interactionHandler->curNode.transform, 0);
        viewer_main->compile();

        //show message on status bar
        std::string str_bar = "切换为编辑模式";
        myStatusBar->showMessage(QString::fromLocal8Bit(str_bar.c_str()), 5000);
    }
    else
    {
        is_editor = false;
        interactionHandler->editable = is_editor;
        ui.label->setText(QString::fromLocal8Bit("观察模式"));

        //clear highlighted
        mesh.repaint(interactionHandler->curNode.transform, 0);
        viewer_main->compile();

        //clear second window


        //show message on status bar
        std::string str_bar = "切换为观察模式";
        myStatusBar->showMessage(QString::fromLocal8Bit(str_bar.c_str()), 5000);
    }

}

void mainWindowApplication::on_Action_Read_fb()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "C:\\User", tr("Model (*.fb)"));
    if (!filename.isEmpty()) {
        filePath = (const char*)filename.toLocal8Bit();

        //show message on status bar
        std::string str_bar = "读取.fb模型：" + (std::string)filePath;
        myStatusBar->showMessage(QString::fromLocal8Bit(str_bar.c_str()), 5000);
    }
}

void mainWindowApplication::on_Action_Read_step()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "C:\\User", tr("Model (*.step)"));
    if (!filename.isEmpty()) {
        
        filePath = (const char*)filename.toLocal8Bit();
        
        //show message on status bar
        std::string str_bar = "读取.step模型：" + (std::string)filePath;
        myStatusBar->showMessage(QString::fromLocal8Bit(str_bar.c_str()), 5000);
    }

}

void mainWindowApplication::on_Action_CameraUpdate(int status)
{
    cameraSelect_main->selectCameraByIndex(status);

    //show message on status bar
    std::string str_bar = "修改相机方位：";
    if (status == 0)
        str_bar += "左视图";
    else if(status == 1)
        str_bar += "右视图";
    else if (status == 2)
        str_bar += "正视图";
    else if (status == 3)
        str_bar += "顶视图";

    myStatusBar->showMessage(QString::fromLocal8Bit(str_bar.c_str()), 5000); // showing message for 5 seconds

}

void mainWindowApplication::doTreeItemSearch(QTreeWidgetItem* item)
{
    if (item->childCount() == 0) // if leaf node
    {
        std::string str = item->text(0).toStdString();
        treeNode node = mesh.getNode(str);
        interactionHandler->select(str);
        
        //show message on status bar
        std::string str_bar = "拾取零件：" + str;
        myStatusBar->showMessage(QString::fromLocal8Bit(str_bar.c_str()), 5000);
        
    }

}

void mainWindowApplication::on_Action_MaterialLib()
{
    //show message on status bar
    std::string str_bar = "打开材质库";
    myStatusBar->showMessage(QString::fromLocal8Bit(str_bar.c_str()), 5000);

    //initialize material lib
    matLibWindow = new matWindow(windowTraits, mesh, viewer_main, nullptr);
    matLibWindow->show();

}

bool mainWindowApplication::clearLayout(QGridLayout* layout)
{
    QLayoutItem* item = NULL;
    if (!layout)
    {
        return false;
    }
    while (NULL != (item = layout->takeAt(0)))
    {
        item->widget()->close(); //关闭但不释放
        item = NULL;
    }
    return true;
}


void mainWindowApplication::on_Action_AR()
{
    if (!is_AR) // switch to AR mode
    {
        is_AR = true;
        //show message on status bar
        std::string str_bar = "打开虚实融合窗口";
        myStatusBar->showMessage(QString::fromLocal8Bit(str_bar.c_str()), 5000);
        ARPresentWidget = new ARWidget(window_main, windowTraits);

        //initialize AR window
        clearLayout(ui.mainLayout);
        ui.mainLayout->addWidget(ARPresentWidget);
    }
    else // switch to normal mode
    {
        is_AR = false;
        //show message on status bar
        std::string str_bar = "关闭虚实融合窗口";
        myStatusBar->showMessage(QString::fromLocal8Bit(str_bar.c_str()), 5000);

        clearLayout(ui.mainLayout);
        ui.mainLayout->addWidget(createWindowContainer(window_main));

    }

}

