cmake_minimum_required(VERSION 3.7)

project(vsgRenderer
        VERSION 1.1.0
        DESCRIPTION "Set of example programs that test and illustrate how to use the VulkanSceneGraph"
        LANGUAGES CXX C
)
find_package(CUDA REQUIRED)
set(CMAKE_CUDA_ARCHITECTURES 100)

#假设已经安装好grpc了
find_package(Threads REQUIRED)

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)


# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)

# find the vsg
if (VULKAN_SDK)
    set(ENV{VULKAN_SDK} ${VULKAN_SDK})
endif()

find_package(vsg 1.1.0)
# find the optional vsgXchange that can be used for reading a range of image and 3d model formats and shader compilation
find_package(vsgXchange 1.0.5 QUIET)

# find the optional vsgImGui that can be used for GUI elements added into graphics windows.
find_package(vsgImGui QUIET)

# set the use of C++17 globally as all examples require it
set(CMAKE_CXX_STANDARD 17)
aux_source_directory(src SRCS)

find_package(flatbuffers)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/Video_Codec_SDK_12.0.16/Utils
    ${CUDA_INCLUDE_DIRS}
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/Video_Codec_SDK_12.0.16/Lib/linux/stubs/x86_64
)

add_executable(vsgRenderer ${SRCS} ${hw_proto_srcs} ${hw_grpc_srcs})

target_link_libraries(vsgRenderer 
    vsg::vsg 
    flatbuffers
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    libnvcuvid.so libnvidia-encode.so ${CUDA_CUDA_LIBRARY})
