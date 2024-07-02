#include <iostream>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "vPacket.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using videoencoding::Request;
using videoencoding::Response;
using videoencoding::VideoEncodingService;

class VideoEncodingClient {
public:
    VideoEncodingClient(std::shared_ptr<Channel> channel) : stub_(VideoEncodingService::NewStub(channel)) {}

    std::string GetStringFromServer(std::vector<double>& viewMatrixVector, std::vector<uint8_t>& color_buffer, std::vector<uint8_t>& depth_buffer)
    {
        Request request;

        for (int i = 0; i < 9; ++i)
        {
            request.add_viewmatrix(viewMatrixVector[i]);
            //std::cout << viewMatrixVector[i] << " ";
        }
        request.set_real_color(reinterpret_cast<const char *>(color_buffer.data()), color_buffer.size());
        request.set_real_depth(reinterpret_cast<const char *>(depth_buffer.data()), depth_buffer.size());

        //std::cout << request.viewmatrix_size();
        Response response;
        ClientContext context;

        Status status = stub_->GetString(&context, request, &response);
        if (status.ok()) {
            const std::string& color = response.color_image();
            // std::cout << "receive : ";
            // std::cout << "size" << size << std::endl;
            // const char* color_char = color.c_str();
            // for(int i = 0; i < size; i ++)
            //     std::cout << (int)color_char[i] << " ";
            // std::cout << std::endl;
            // const uint8_t* color_uint = reinterpret_cast<const uint8_t*>(color_char);
            // for(int i = 0; i < size; i ++)
            //     std::cout << (int)color_uint[i] << " ";
            // std::cout << std::endl;

            return color;
        } else {
            std::cerr << "RPC failed with error code: " << status.error_code() << ": " << status.error_message() << std::endl;
            return NULL;
        }
    }
private:
    std::unique_ptr<VideoEncodingService::Stub> stub_;
};
