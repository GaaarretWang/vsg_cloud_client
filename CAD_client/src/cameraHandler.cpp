#include <vsg/all.h>
#include <iostream>

class CameraSelector : public vsg::Inherit<vsg::Visitor, CameraSelector>
{
public:
    using Cameras = decltype(vsg::FindCameras::cameras);

    vsg::ref_ptr<vsg::Camera> main_camera;
    vsg::ref_ptr<vsg::LookAt> original_viewMatrix;
    Cameras cameras;
    int currentCameraIndex = 0;

    CameraSelector(vsg::ref_ptr<vsg::Camera> in_camera, const Cameras& in_cameras) :
        main_camera(in_camera),
        cameras(in_cameras)
    {
        auto lookAt = main_camera->viewMatrix.cast<vsg::LookAt>();
        if (lookAt) original_viewMatrix = vsg::LookAt::create(*lookAt);
    }

    void apply(vsg::KeyPressEvent& keyPress) override
    {
        if (keyPress.keyBase == '0')
        {
            if (original_viewMatrix)
            {
                auto lookAt = main_camera->viewMatrix.cast<vsg::LookAt>();
                if (lookAt)
                {
                    *(lookAt) = *original_viewMatrix;
                }
            }
        }
        else if ((keyPress.keyBase >= '1') && (keyPress.keyBase <= '9'))
        {
            currentCameraIndex = keyPress.keyBase - '1';
            selectCameraByIndex(currentCameraIndex);
        }
    }

    void selectCameraByIndex(int index)
    {
        int cameraIndex = 0;
        for (auto& [nodePath, camera] : cameras)
        {
            if (cameraIndex == index)
            {
                auto begin = nodePath.begin();
                auto end = nodePath.end();
                if (begin != end) --end;

                // auto matrix = vsg::computeTransform(nodePath);
                //auto matrix = vsg::visit<vsg::ComputeTransform>(begin, end).matrix;

                //std::cout << "Matched: " << camera->name << " " << matrix << std::endl;

                auto selected_lookAt = camera->viewMatrix.cast<vsg::LookAt>();
                auto main_lookAt = main_camera->viewMatrix.cast<vsg::LookAt>();
                if (main_lookAt)
                {
                    *main_lookAt = *selected_lookAt;
                    //main_lookAt->transform(matrix);
                }
                break;
            }
            ++cameraIndex;
        }
    }
};

