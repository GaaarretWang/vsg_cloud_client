#pragma once
#include "VideoEncodingClient.h"
#include "encoder.h"
#include "ColorSpace.h"
#include "chrono"
#include "fstream"

class ReceiveServer : public vsg::Inherit<vsg::Visitor, ReceiveServer>
{
public:
    std::shared_ptr<Channel> channel;
    VideoEncodingClient* client;
    vsg::ref_ptr<vsg::Data> textureData;
    vsg::ref_ptr<vsg::Camera> camera;
    int width;
    int height;
    CUdeviceptr dpFrame = 0;
    std::vector<std::vector<double>> camera_pos;
    int frame =  0;
    NvEncoderWrapper* m_encoder = new NvEncoderWrapper();
    int64_t frame_count = 0;

    vsg::ref_ptr<vsg::Data> getTextureData() {
        return textureData;
    }

    void initCamera(vsg::ref_ptr<vsg::Camera> camera)
    {
        this->camera = camera;
    }

    ReceiveServer(int width, int height)
    {
        this->width = width;
        this->height = height;
        // channel = grpc::CreateChannel("10.102.32.173:50121", grpc::InsecureChannelCredentials());
        channel = grpc::CreateChannel("127.0.0.1:50111", grpc::InsecureChannelCredentials());
        client = new VideoEncodingClient(channel);

        textureData = vsg::ubvec4Array2D::create(width, height);
        textureData->properties.format = VK_FORMAT_B8G8R8A8_UNORM;
        textureData->properties.dataVariance = vsg::DYNAMIC_DATA;
        for (int i = 0; i < width * height; i++)
        {
            auto color = static_cast<vsg::ubvec4*>(textureData->dataPointer(i));
            color->w = 255;
        }
        cuMemAlloc(&dpFrame, width * height * 4);

        std::ifstream inf;//文件读操作
        std::string line;
        inf.open("../data/cameraPose/vsg_pose.txt");         

        while (getline(inf, line))      //getline(inf,s)是逐行读取inf中的文件信息
        {    
            std::istringstream Linestream(line);
            std::vector<double> values;
            std::vector<double> lookatvalues;
            std::string value;
            while(Linestream >> value){
                // std::cout << "value" << value << "=" << std::stof(value);
                values.push_back(std::stod(value));
            }
            // pos, up, forward => centre, eye, up
            lookatvalues.push_back(values[0] + values[6]);
            lookatvalues.push_back(values[1] + values[7]);
            lookatvalues.push_back(values[2] + values[8]);
            lookatvalues.push_back(values[0]);
            lookatvalues.push_back(values[1]);
            lookatvalues.push_back(values[2]);
            lookatvalues.push_back(values[3]);
            lookatvalues.push_back(values[4]);
            lookatvalues.push_back(values[5]);

            camera_pos.push_back(lookatvalues);
        }
        inf.close();
    }

    void initEncoder(vsg::ref_ptr<vsg::Window> window)
    {
        VkExtent2D extent = {};
        extent.width = width;
        extent.height = height;
        m_encoder->initCuda(window->getOrCreateDevice()->getInstance(), window);
        // m_encoder->initEncoder(extent);
        m_encoder->initDecoder();
        std::cout << "Encoder Init succeed!" << std::endl;

    }

    void apply(vsg::FrameEvent& frameEvent) override
    {
        std::vector<double> lookAtSend(9);
        auto lookAtCamera = camera->viewMatrix.cast<vsg::LookAt>();
        for (int i = 0; i < 3; i++)
        {
            lookAtSend[0 + i] = lookAtCamera->center[i];
            lookAtSend[3 + i] = lookAtCamera->eye[i];
            lookAtSend[6 + i] = lookAtCamera->up[i];
        }

        std::ifstream color_file("../data/slamData/color/0.png", std::ios::binary | std::ios::app);
        std::vector<uint8_t> color_buffer((std::istreambuf_iterator<char>(color_file)), std::istreambuf_iterator<char>());
        std::ifstream depth_file("../data/slamData/depth/0.png", std::ios::binary | std::ios::app);
        std::vector<uint8_t> depth_buffer((std::istreambuf_iterator<char>(depth_file)), std::istreambuf_iterator<char>());

        auto start = std::chrono::high_resolution_clock::now();
        std::cout << camera_pos.size() << "\t";
        // std::string color_string = client->GetStringFromServer(lookAtSend, color_buffer, depth_buffer);
        std::string color_string = client->GetStringFromServer(camera_pos[frame], color_buffer, depth_buffer);
        frame ++;
        if(frame >= camera_pos.size())
            frame = 0;
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << "Time" << duration.count() << "mil seconds" << std::endl;
        const uint8_t* color_image = reinterpret_cast<const uint8_t*>(color_string.c_str());
        size_t size = color_string.size();

        // for(int i = 0; i < size; i ++)
        //     std::cout << (int)color_image[i] << " ";
        // std::cout << std::endl;

        //std::cout << "size = " << size << std::endl;
        //std::cout << "frame_count = " << frame_count << std::endl;
        std::cout << "size" << size << std::endl;
        int nFrameReturned = m_encoder->getDecoder()->Decode(color_image, size, 0, frame_count);
        std::cout << "nFrameReturned" << nFrameReturned << std::endl;

        cuMemAlloc(&dpFrame, width * height * 4);
        for (int i = 0; i < nFrameReturned; i++)
        {
            int64_t timestamp = 0;
            uint8_t* pFrame = m_encoder->getDecoder()->GetFrame(&timestamp);
            int iMatrix = m_encoder->getDecoder()->GetVideoFormatInfo().video_signal_description.matrix_coefficients;
            if (m_encoder->getDecoder()->GetBitDepth() == 8)
            {
               if (m_encoder->getDecoder()->GetOutputFormat() == cudaVideoSurfaceFormat_YUV444)
                   YUV444ToColor32<BGRA32>(pFrame, m_encoder->getDecoder()->GetWidth(), (uint8_t*)dpFrame, 4 * width, m_encoder->getDecoder()->GetWidth(), m_encoder->getDecoder()->GetHeight(), iMatrix);
               else // default assumed as NV12
                   Nv12ToColor32<BGRA32>(pFrame, m_encoder->getDecoder()->GetWidth(), (uint8_t*)dpFrame, 4 * width, m_encoder->getDecoder()->GetWidth(), m_encoder->getDecoder()->GetHeight(), iMatrix);
            }
            else
            {
               if (m_encoder->getDecoder()->GetOutputFormat() == cudaVideoSurfaceFormat_YUV444_16Bit)
                   YUV444P16ToColor32<BGRA32>(pFrame, 2 * m_encoder->getDecoder()->GetWidth(), (uint8_t*)dpFrame, 4 * width, m_encoder->getDecoder()->GetWidth(), m_encoder->getDecoder()->GetHeight(), iMatrix);
               else // default assumed as P016
                   P016ToColor32<BGRA32>(pFrame, 2 * m_encoder->getDecoder()->GetWidth(), (uint8_t*)dpFrame, 4 * width, m_encoder->getDecoder()->GetWidth(), m_encoder->getDecoder()->GetHeight(), iMatrix);
            }

            CUDA_ARRAY_DESCRIPTOR arrayDesc;
            memset(&arrayDesc, 0, sizeof(arrayDesc));
            arrayDesc.Format = CU_AD_FORMAT_UNSIGNED_INT8;
            arrayDesc.NumChannels = 4;
            arrayDesc.Width = width;
            arrayDesc.Height = height;

            CUarray cuArray;
            CUresult result = cuArrayCreate(&cuArray, &arrayDesc);
            if (result != CUDA_SUCCESS) {
                throw std::runtime_error("Failed cuArrayCreate");
            }

            CUDA_MEMCPY2D copy = {};
            copy.srcMemoryType = CU_MEMORYTYPE_DEVICE;
            copy.srcDevice = dpFrame;
            copy.dstMemoryType = CU_MEMORYTYPE_ARRAY;
            copy.dstArray = cuArray;
            copy.WidthInBytes = 4 * width;
            copy.Height = height;

            result = cuMemcpy2D(&copy);
            if (result != CUDA_SUCCESS) {
                const char* errorString;
                cuGetErrorString(result, &errorString);
                printf("CUDA Error: %s\n", errorString);

                throw std::runtime_error("Failed cuMemcpy2D");
            }

            void* hostData = malloc(width * height * 4);
            copy.srcMemoryType = CU_MEMORYTYPE_ARRAY;
            copy.srcArray = cuArray;
            copy.dstMemoryType = CU_MEMORYTYPE_HOST;
            copy.dstHost = hostData;
            copy.WidthInBytes = 4 * width;
            copy.Height = height;

            result = cuMemcpy2D(&copy);
            if (result != CUDA_SUCCESS) {
                const char* errorString;
                cuGetErrorString(result, &errorString);
                printf("CUDA Error: %s\n", errorString);

                throw std::runtime_error("Failed cuMemcpy2D");
            }
            cudaFree(&dpFrame);
            //    //void* hostData = malloc(mWidth * mHeight * 4);
            //    //copy.srcMemoryType = CU_MEMORYTYPE_ARRAY;
            //    //copy.srcArray = cuArray;
            //    //copy.dstMemoryType = CU_MEMORYTYPE_HOST;
            //    //copy.dstHost = hostData;
            //    //copy.WidthInBytes = 4 * mWidth;
            //    //copy.Height = mHeight;
            //
            //    //result = cuMemcpy2D(&copy);
            //    //if (result != CUDA_SUCCESS) {
            //    //    const char* errorString;
            //    //    cuGetErrorString(result, &errorString);
            //    //    printf("CUDA Error: %s\n", errorString);
            //
            //    //    throw std::runtime_error("Failed cuMemcpy2D");
            //    //}
            //    //fpOut.write(reinterpret_cast<char*>(hostData), mWidth * mHeight * 4);

            frame_count++;
            for (int i = 0; i < width * height; i++)
            {
                auto color = static_cast<vsg::ubvec4*>(textureData->dataPointer(i));

                color->x = reinterpret_cast<uint8_t*>(hostData)[i * 4];
                color->y = reinterpret_cast<uint8_t*>(hostData)[i * 4 + 1];
                color->z = reinterpret_cast<uint8_t*>(hostData)[i * 4 + 2];
                //color->x = 255;
                //color->y = 255;
                //color->z = 255;
                //color->w = 255;
            }
            textureData->dirty();
        }


        //CUarray inputArray = (CUarray)m_encoder->getEncoder()->GetNextInputFrame()->inputPtr;
        //const DeviceAlloc* inputSurf = m_encoder->mapCUarrayToDeviceAlloc[inputArray];
        // imageutils->beginSingleTimeCommands();
        //updateEncodeData(inputSurf, m_encoder->getEncodeBuffer());
        //inputSurf->cudaSemaphore->wait();
        //std::vector<std::vector<uint8_t>> vPacket;
        //m_encoder->getEncoder()->EncodeFrame(vPacket);

    }
};
