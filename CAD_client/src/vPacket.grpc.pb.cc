// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: vPacket.proto

#include "vPacket.pb.h"
#include "vPacket.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace videoencoding {

static const char* VideoEncodingService_method_names[] = {
  "/videoencoding.VideoEncodingService/GetString",
};

std::unique_ptr< VideoEncodingService::Stub> VideoEncodingService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< VideoEncodingService::Stub> stub(new VideoEncodingService::Stub(channel, options));
  return stub;
}

VideoEncodingService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_GetString_(VideoEncodingService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status VideoEncodingService::Stub::GetString(::grpc::ClientContext* context, const ::videoencoding::Request& request, ::videoencoding::Response* response) {
  return ::grpc::internal::BlockingUnaryCall< ::videoencoding::Request, ::videoencoding::Response, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetString_, context, request, response);
}

void VideoEncodingService::Stub::async::GetString(::grpc::ClientContext* context, const ::videoencoding::Request* request, ::videoencoding::Response* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::videoencoding::Request, ::videoencoding::Response, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetString_, context, request, response, std::move(f));
}

void VideoEncodingService::Stub::async::GetString(::grpc::ClientContext* context, const ::videoencoding::Request* request, ::videoencoding::Response* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetString_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::videoencoding::Response>* VideoEncodingService::Stub::PrepareAsyncGetStringRaw(::grpc::ClientContext* context, const ::videoencoding::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::videoencoding::Response, ::videoencoding::Request, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetString_, context, request);
}

::grpc::ClientAsyncResponseReader< ::videoencoding::Response>* VideoEncodingService::Stub::AsyncGetStringRaw(::grpc::ClientContext* context, const ::videoencoding::Request& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetStringRaw(context, request, cq);
  result->StartCall();
  return result;
}

VideoEncodingService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      VideoEncodingService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< VideoEncodingService::Service, ::videoencoding::Request, ::videoencoding::Response, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](VideoEncodingService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::videoencoding::Request* req,
             ::videoencoding::Response* resp) {
               return service->GetString(ctx, req, resp);
             }, this)));
}

VideoEncodingService::Service::~Service() {
}

::grpc::Status VideoEncodingService::Service::GetString(::grpc::ServerContext* context, const ::videoencoding::Request* request, ::videoencoding::Response* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace videoencoding

