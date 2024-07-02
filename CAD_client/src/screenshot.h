class ScreenshotHandler : public vsg::Inherit<vsg::Visitor, ScreenshotHandler>
{
public:
    vsg::ref_ptr<vsg::CopyImage>* copyImages;
    vsg::ref_ptr<vsg::Image>* images;
    vsg::ref_ptr<vsg::ImageInfo>* imageInfos;
    vsgQt::Window* window1;
    vsgQt::Window* window2;
    vsgQt::Window* window3;
    vsg::ref_ptr<vsgQt::Viewer> viewer;
    vsg::ref_ptr<vsg::CommandGraph> commandGraph;
    bool resize = false;
    QWidget* widget1;
    QWidget* widget2;
    QWidget* widget3;
    QWidget* widget4;
    //构造函数
    ScreenshotHandler(vsg::ref_ptr<vsgQt::Viewer> in_viewer, vsgQt::Window*& in_window1, vsgQt::Window*& in_window2, vsgQt::Window*& in_window3, vsg::ref_ptr<vsg::CopyImage>* in_copyImages, vsg::ref_ptr<vsg::Image>* in_images, vsg::ref_ptr<vsg::ImageInfo>* in_imageInfos) :
        viewer(in_viewer), window1(in_window1), window2(in_window2), window3(in_window3), copyImages(in_copyImages), images(in_images), imageInfos(in_imageInfos)
    {
    }

    void apply(vsg::ConfigureWindowEvent& windowEvent)
    {
        resize = true;
    }


    void apply(vsg::FrameEvent& frame)
    {
        if (window1->windowAdapter && !resize) {
            auto swapchain1 = window1->windowAdapter->getSwapchain();                                                                                                                                                                                                                             //获取与窗口相关的设备、物理设备和交换链
            //get the colour buffer image of the previous rendered frame as the current frame hasn't been rendered yet.  The 1 in window->imageIndex(1) means image from 1 frame ago. 下标要为0 
            if (window1->windowAdapter->getSwapchain()->getExtent().width != window1->windowAdapter->extent2D().width) { std::cout << "11111111111"; }
            auto colorImage1 = window1->windowAdapter->imageView(window1->windowAdapter->imageIndex(0))->image; //获取之前渲染帧的颜色缓冲图像作为当前帧的来源图像 window->imageIndex(1)表示获取之前一帧的图像
            //获取源图像和图像格式 sourceImage 是当前窗口渲染绘制的画面的深度图像
            vsg::ref_ptr<vsg::Image> depthImage1(window1->windowAdapter->getDepthImage());
            copyImages[0]->srcImage = colorImage1;
            copyImages[0]->srcImageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            copyImages[1]->srcImage = depthImage1;
            copyImages[1]->srcImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            auto swapchain2 = window2->windowAdapter->getSwapchain(); //获取与窗口相关的设备、物理设备和交换链
            //std::cout << swapchain2->getExtent().width << "\t" << swapchain2->getExtent().height << "\t";
            auto colorImage2 = window2->windowAdapter->imageView(window2->windowAdapter->imageIndex(0))->image; //获取之前渲染帧的颜色缓冲图像作为当前帧的来源图像 window->imageIndex(1)表示获取之前一帧的图像
            vsg::ref_ptr<vsg::Image> depthImage2(window2->windowAdapter->getDepthImage());
            copyImages[2]->srcImage = colorImage2;
            copyImages[2]->srcImageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            copyImages[3]->srcImage = depthImage2;
            copyImages[3]->srcImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            auto swapchain3 = window3->windowAdapter->getSwapchain();                                          //获取与窗口相关的设备、物理设备和交换链
            //std::cout << swapchain3->getExtent().width << "\t" << swapchain3->getExtent().height << "\n";
            auto colorImage3 = window3->windowAdapter->imageView(window3->windowAdapter->imageIndex(0))->image; //获取之前渲染帧的颜色缓冲图像作为当前帧的来源图像 window->imageIndex(1)表示获取之前一帧的图像
            vsg::ref_ptr<vsg::Image> depthImage3(window3->windowAdapter->getDepthImage());
            copyImages[4]->srcImage = colorImage3;
            copyImages[4]->srcImageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            copyImages[5]->srcImage = depthImage3;
            copyImages[5]->srcImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }
        else
        {
            copyImages[0]->srcImage = copyImages[0]->dstImage;
            copyImages[0]->srcImageLayout = copyImages[0]->dstImageLayout;
            copyImages[1]->srcImage = copyImages[1]->dstImage;
            copyImages[1]->srcImageLayout = copyImages[1]->dstImageLayout;
            copyImages[2]->srcImage = copyImages[2]->dstImage;
            copyImages[2]->srcImageLayout = copyImages[2]->dstImageLayout;
            copyImages[3]->srcImage = copyImages[3]->dstImage;
            copyImages[3]->srcImageLayout = copyImages[3]->dstImageLayout;
            copyImages[4]->srcImage = copyImages[4]->dstImage;
            copyImages[4]->srcImageLayout = copyImages[4]->dstImageLayout;
            copyImages[5]->srcImage = copyImages[5]->dstImage;
            copyImages[5]->srcImageLayout = copyImages[5]->dstImageLayout;
            resize = false;
        }
    }

    //void printInfo(vsg::ref_ptr<vsg::Window> window)
    //{ //打印窗口和设备的信息;接受一个 vsg::Window 对象作为参数，并输出与窗口相关的信息，如设备、交换链、图像视图等
    //    auto device = window->getDevice();
    //    auto physicalDevice = window->getPhysicalDevice();
    //    auto swapchain = window->getSwapchain();
    //    std::cout << "\nNeed to take screenshot " << window << std::endl;
    //    std::cout << "    device = " << device << std::endl;
    //    std::cout << "    physicalDevice = " << physicalDevice << std::endl;
    //    std::cout << "    swapchain = " << swapchain << std::endl;
    //    std::cout << "        swapchain->getImageFormat() = " << swapchain->getImageFormat() << std::endl;
    //    std::cout << "        swapchain->getExtent() = " << swapchain->getExtent().width << ", " << swapchain->getExtent().height << std::endl;

    //    for (auto& imageView : swapchain->getImageViews())
    //    {
    //        std::cout << "        imageview = " << imageView << std::endl;
    //    }

    //    std::cout << "    numFrames() = " << window->numFrames() << std::endl;
    //    for (size_t i = 0; i < window->numFrames(); ++i)
    //    {
    //        std::cout << "        imageview[" << i << "] = " << window->imageView(i) << std::endl;
    //        std::cout << "        framebuffer[" << i << "] = " << window->framebuffer(i) << std::endl;
    //    }

    //    std::cout << "    surfaceFormat() = " << window->surfaceFormat().format << ", " << window->surfaceFormat().colorSpace << std::endl;
    //    std::cout << "    depthFormat() = " << window->depthFormat() << std::endl;
    //}

    //vsg::ref_ptr<vsg::Image> screenshot_image(vsg::ref_ptr<vsg::Window> window)
    //{
    //    auto width = window->extent2D().width;
    //    auto height = window->extent2D().height; //获取窗口的宽度和高度

    //    auto device = window->getDevice();
    //    auto physicalDevice = window->getPhysicalDevice();
    //    auto swapchain = window->getSwapchain(); //获取与窗口相关的设备、物理设备和交换链
    //    VkFormat sourceImageFormat = swapchain->getImageFormat();

    //    // get the colour buffer image of the previous rendered frame as the current frame hasn't been rendered yet.  The 1 in window->imageIndex(1) means image from 1 frame ago.
    //    // 下标要为0
    //    auto sourceImage = window->imageView(window->imageIndex(0))->image; //获取之前渲染帧的颜色缓冲图像作为当前帧的来源图像 window->imageIndex(1)表示获取之前一帧的图像

    //    return sourceImage;
    //}

    //vsg::ref_ptr<vsg::Image> screenshot_depth(vsg::ref_ptr<vsg::Window> window)
    //{
    //    auto width = window->extent2D().width;
    //    auto height = window->extent2D().height; //获取窗口大小

    //    auto device = window->getDevice();
    //    auto physicalDevice = window->getPhysicalDevice(); //获取设备和物理设备

    //    //获取源图像和图像格式 sourceImage 是当前窗口渲染绘制的画面的深度图像
    //    vsg::ref_ptr<vsg::Image> sourceImage(window->getDepthImage());
    //    //sourceImage表示源图像，sourceImageFormat表示源图像的格式 targetImageFormat表示目标图像的格式
    //    VkFormat sourceImageFormat = window->depthFormat();
    //    return sourceImage;
    //}
};