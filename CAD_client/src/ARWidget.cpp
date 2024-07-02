#include "ARWidget.h"

ARWidget::ARWidget(vsgQt::Window* in_window, vsg::ref_ptr<vsg::WindowTraits> in_traits, QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);


    //window4 = in_window;
    inWindow = in_window;
    traits = createWindowTraits();
    //traits = in_traits;
    traits->device = in_window->windowAdapter->getOrCreateDevice();
    updateLightInfo();
    createCamera();
    cad.buildnode(path1, fullNormal, cadScenegraph, phongShader);        //读取几何信息1
    shadow.buildnode(path1, fullNormal, shadowScenegraph, shadowShader); //读取几何信息1
    cad.buildPlaneNode(shadowScenegraph, planeShader);
    cad.buildPlaneNode(envScenegraph, phongShader);
    trackball1 = vsg::Trackball::create(camera1);
    trackball2 = vsg::Trackball::create(camera2);
    trackball3 = vsg::Trackball::create(camera3);


    createVsgARWindow_1(window1, nullptr);

    createVsgARWindow_2(window2, nullptr);

    createVsgARWindow_3(window3, nullptr);

    createVsgARWindow_4(window4, nullptr);
}

ARWidget::~ARWidget()
{}

vsg::ref_ptr<vsg::WindowTraits> ARWidget::createWindowTraits()
{
    auto windowTraits = vsg::WindowTraits::create();
    windowTraits->width = 640;
    windowTraits->height = 480;
    windowTraits->swapchainPreferences.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    windowTraits->depthImageUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    windowTraits->depthFormat = VK_FORMAT_D32_SFLOAT;
    windowTraits->deviceExtensionNames = {
        VK_KHR_MULTIVIEW_EXTENSION_NAME,
        VK_KHR_MAINTENANCE2_EXTENSION_NAME,
        VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
        VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME};
    return windowTraits;
}

void ARWidget::createCamera()
{
    double radius = 2000.0; // 固定观察距离
    auto viewport = vsg::ViewportState::create(0, 0, traits->width, traits->height);
    auto perspective = vsg::Perspective::create(60.0, static_cast<double>(traits->width) / static_cast<double>(traits->height), nearFarRatio * radius, radius * 10.0);
    vsg::dvec3 centre = {0.0, 0.0, 0.0};                    // 固定观察点
    vsg::dvec3 eye = centre + vsg::dvec3(0.0, radius, 0.0); // 固定相机位置
    vsg::dvec3 up = {0.0, 0.0, 1.0};                        // 固定观察方向
    auto lookAt = vsg::LookAt::create(eye, centre, up);
    camera1 = vsg::Camera::create(perspective, lookAt, viewport);
    auto viewport2 = vsg::ViewportState::create(0, 0, traits->width, traits->height);
    camera2 = vsg::Camera::create(perspective, lookAt, viewport2);
    auto viewport3 = vsg::ViewportState::create(0, 0, traits->width, traits->height);
    camera3 = vsg::Camera::create(perspective, lookAt, viewport3);

    vsg::dvec3 mergeEye = centre + vsg::dvec3(0.0, 0.0, radius); // 固定相机位置
    vsg::dvec3 mergeUp = {0.0, 1.0, 0.0};                                  // 固定观察方向
    auto mergelookAt = vsg::LookAt::create(mergeEye, centre, mergeUp);
    auto viewport4 = vsg::ViewportState::create(0, 0, traits->width, traits->height);
    mergeCamera = vsg::Camera::create(perspective, mergelookAt, viewport4);
}

void ARWidget::updateLightInfo()
{
    auto group = vsg::Group::create();
    auto Env_group = vsg::Group::create();
    auto Shadow_group = vsg::Group::create();
    group->addChild(cadScenegraph);
    Env_group->addChild(envScenegraph); //加在这里则scenegraph中也有背景内容
    Shadow_group->addChild(shadowScenegraph);
    if (numLights >= 1)
    {
        std::cout << "-----------------DirectionalLight--------" << std::endl;
        directionalLight->name = "directional";
        directionalLight->color.set(1.0, 1.0, 1.0);
        directionalLight->intensity = 1.3;
        directionalLight->direction = vsg::normalize(vsg::vec3(1.0, -1.0, 1.0));
        directionalLight->shadowMaps = numShadowMapsPerLight;
        group->addChild(directionalLight);
        Env_group->addChild(directionalLight);
        Shadow_group->addChild(directionalLight);
    }

    if (numLights >= 2)
    {
        std::cout << "-----------------AmbientLight--------" << std::endl;
        ambientLight->name = "ambient";
        ambientLight->color.set(1.0, 1.0, 1.0);
        ambientLight->intensity = 0.2;
        group->addChild(ambientLight);
        Env_group->addChild(ambientLight);
        Shadow_group->addChild(ambientLight);
    }

    if (numLights >= 3)
    {
        std::cout << "-----------------DirectionalLight2--------" << std::endl;
        directionalLight->intensity = 0.7;
        ambientLight->intensity = 0.1;

        directionalLight2->name = "2nd directional";
        directionalLight2->color.set(1.0, 1.0, 0.0);
        directionalLight2->intensity = 0.7;
        directionalLight2->direction = vsg::normalize(vsg::vec3(0.9, 1.0, -1.0));
        directionalLight2->shadowMaps = numShadowMapsPerLight;
        group->addChild(directionalLight2);
        Env_group->addChild(directionalLight2);
        Shadow_group->addChild(directionalLight2);
    }
    cadScenegraph = group;
    envScenegraph = Env_group;
    shadowScenegraph = Shadow_group;
}

void ARWidget::createVsgARWindow_1(vsgQt::Window*& window, QWindow* parent)
{
    //main window
    //QWindow* parent = nullptr;
    window = new vsgQt::Window(viewer1, traits, parent);
    window->initializeWindow();
    auto view = vsg::View::create(camera1, cadScenegraph);
    auto renderGraph = vsg::RenderGraph::create(*window, view);
    renderGraph->clearValues[0].color = {{0.8f, 0.8f, 0.8f, 1.f}};
    auto commandGraph = vsg::CommandGraph::create(*window, renderGraph);
    commandGraph->addChild(renderGraph);
    viewer1->assignRecordAndSubmitTaskAndPresentation({commandGraph});
    viewer1->addEventHandler(trackball1);
    viewer1->addEventHandler(trackball2);
    viewer1->addEventHandler(trackball3);
    viewer1->compile(); //编译命令图。接受一个可选的`ResourceHints`对象作为参数，用于提供编译时的一些提示和配置。通过调用这个函数，可以将命令图编译为可执行的命令。

    widget1 = createWindowContainer(window);
    ui.layout1->addWidget(widget1);
}

void ARWidget::createVsgARWindow_2(vsgQt::Window*& window, QWindow* parent)
{
    //main window
    //QWindow* parent = nullptr;
    window = new vsgQt::Window(viewer2, traits, parent);
    window->initializeWindow();
    auto view = vsg::View::create(camera2, envScenegraph);
    auto renderGraph = vsg::RenderGraph::create(*window, view);
    renderGraph->clearValues[0].color = {{0.8f, 0.8f, 0.8f, 1.f}};
    auto commandGraph = vsg::CommandGraph::create(*window, renderGraph);
    commandGraph->addChild(renderGraph);
    viewer2->assignRecordAndSubmitTaskAndPresentation({commandGraph});
    viewer2->addEventHandler(trackball1);
    viewer2->addEventHandler(trackball2);
    viewer2->addEventHandler(trackball3);
    viewer2->compile(); //编译命令图。接受一个可选的`ResourceHints`对象作为参数，用于提供编译时的一些提示和配置。通过调用这个函数，可以将命令图编译为可执行的命令。

    widget2 = createWindowContainer(window);
    ui.layout2->addWidget(widget2);
}

void ARWidget::createVsgARWindow_3(vsgQt::Window*& window, QWindow* parent)
{
    //main window
    //QWindow* parent = nullptr;
    window = new vsgQt::Window(viewer3, traits, parent);
    window->initializeWindow();
    auto view = vsg::View::create(camera3, shadowScenegraph);
    auto renderGraph = vsg::RenderGraph::create(*window, view);
    renderGraph->clearValues[0].color = {{1.f, 1.f, 1.f, 1.f}};
    auto commandGraph = vsg::CommandGraph::create(*window, renderGraph);
    commandGraph->addChild(renderGraph);
    viewer3->assignRecordAndSubmitTaskAndPresentation({commandGraph});
    viewer3->addEventHandler(trackball1);
    viewer3->addEventHandler(trackball2);
    viewer3->addEventHandler(trackball3);
    viewer3->compile(); //编译命令图。接受一个可选的`ResourceHints`对象作为参数，用于提供编译时的一些提示和配置。通过调用这个函数，可以将命令图编译为可执行的命令。

    widget3 = createWindowContainer(window);
    ui.layout3->addWidget(widget3);
}

void ARWidget::createVsgARWindow_4(vsgQt::Window*& window, QWindow* parent)
{
    //main window
    //QWindow* parent = nullptr;
    window = new vsgQt::Window(viewer4, traits, parent);
    window->initializeWindow();
    widget4 = createWindowContainer(window);

    for (int i = 0; i < 6; i++)
    {
        vsg::ref_ptr<vsg::Image> storageImage = vsg::Image::create();
        storageImage->imageType = VK_IMAGE_TYPE_2D;
        if (i % 2 == 0)
            storageImage->format = VK_FORMAT_B8G8R8A8_UNORM; //VK_FORMAT_R8G8B8A8_UNORM;
        else
            storageImage->format = VK_FORMAT_D32_SFLOAT; //VK_FORMAT_R8G8B8A8_UNORM;
        storageImage->extent.width = window->traits->width;
        storageImage->extent.height = window->traits->height;
        storageImage->extent.depth = 1;
        storageImage->mipLevels = 1;
        storageImage->arrayLayers = 1;
        storageImage->samples = VK_SAMPLE_COUNT_1_BIT;
        storageImage->tiling = VK_IMAGE_TILING_OPTIMAL;
        storageImage->usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // sample和storage一定要标
        storageImage->initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        storageImage->flags = 0;
        storageImage->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        storageImages[i] = storageImage;
        auto context = vsg::Context::create(window->windowAdapter->getOrCreateDevice());
        imageInfos[i] = vsg::ImageInfo::create(vsg::Sampler::create(), vsg::createImageView(*context, storageImage, VK_IMAGE_ASPECT_COLOR_BIT), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        copyImages[i] = vsg::CopyImage::create();
        copyImages[i]->srcImage = storageImage;
        copyImages[i]->srcImageLayout = imageInfos[i]->imageLayout;
        copyImages[i]->dstImage = storageImage;
        copyImages[i]->dstImageLayout = imageInfos[i]->imageLayout;
        VkImageCopy copyRegion = {};
        copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.srcSubresource.layerCount = 1;
        copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.dstSubresource.layerCount = 1;
        copyRegion.extent.width = window->traits->width;
        copyRegion.extent.height = window->traits->height;
        copyRegion.extent.depth = 1;
        copyImages[i]->regions.push_back(copyRegion);
    }
    cad.buildIntgNode(intgScenegraph, mergeShader, imageInfos); 
    auto view = vsg::View::create(mergeCamera, intgScenegraph);
    auto renderGraph = vsg::RenderGraph::create(*window, view);
    renderGraph->clearValues[0].color = {{0.8f, 0.8f, 0.8f, 1.f}};
    auto commandGraph = vsg::CommandGraph::create(*window);
    for (int i = 0; i < 6; i++)
        commandGraph->addChild(copyImages[i]);
    commandGraph->addChild(renderGraph);
    viewer4->assignRecordAndSubmitTaskAndPresentation({commandGraph});
    viewer4->addEventHandler(trackball1);
    viewer4->addEventHandler(trackball2);
    viewer4->addEventHandler(trackball3);
    auto screenshotHandler = ScreenshotHandler::create(viewer4, window1, window2, window3, copyImages, storageImages, imageInfos);
    screenshotHandler->widget1 = widget1;
    screenshotHandler->widget2 = widget2;
    screenshotHandler->widget3 = widget3;
    screenshotHandler->widget4 = widget4;
    viewer4->addEventHandler(screenshotHandler);
    viewer4->compile(); //编译命令图。接受一个可选的`ResourceHints`对象作为参数，用于提供编译时的一些提示和配置。通过调用这个函数，可以将命令图编译为可执行的命令。

    ui.layout4->addWidget(widget4);

}

