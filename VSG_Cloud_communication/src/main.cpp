#include "vsgRenderer.h"
#include <iostream>
#include <string>
#include <grpcpp/grpcpp.h>
#include "vPacket.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using videoencoding::Request;
using videoencoding::Response;
using videoencoding::VideoEncodingService;

class VideoEncodingServiceImpl final : public VideoEncodingService::Service {
public:
    vsgRenderer renderer;

    Status GetString(ServerContext* context, const Request* request, Response* response) override {
        std::vector<double> lookAtVector(9);
        for (int i = 0; i < 9; ++i) {
            lookAtVector[i] = request->viewmatrix(i);
        }

        std::vector<std::vector<uint8_t>> color_image;
        size_t size = 0;
        const std::string& real_color = request->real_color();
        const std::string& real_depth = request->real_depth();

        renderer.render(color_image, lookAtVector, real_color, real_depth);
        if(color_image.size() > 0)
            response->set_color_image(reinterpret_cast<const char *>(color_image[0].data()), color_image[0].size());
        else
            response->set_color_image(nullptr, 0);

        return Status::OK;
    }
    void initRenderer(int argc, char** argv){
        renderer.initRenderer(argc, argv);
    }

};

int main(int argc, char** argv) {
    //std::string server_address("10.102.32.173:50111");    
    std::string server_address("127.0.0.1:50111");
    VideoEncodingServiceImpl service;
    service.initRenderer(argc, argv);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
    return 0;
}
