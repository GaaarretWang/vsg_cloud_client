cmake_minimum_required(VERSION 3.7)
set(CMAKE_CUDA_ARCHITECTURES 61)
project(vsgRenderer
        VERSION 1.1.0
        DESCRIPTION "Set of example programs that test and illustrate how to use the VulkanSceneGraph"
        LANGUAGES C CXX CUDA
)

# include(CheckLanguage)
# check_language(CUDA)

# if(CMAKE_CUDA_COMPILER)
#     message(STATUS "Found CUDA compiler: ${CMAKE_CUDA_COMPILER}")
# else(CMAKE_CUDA_COMPILER)
#     message(FATAL_ERROR "CUDA compiler not found")
# endif()


find_package(CUDA REQUIRED)
# enable_language(CUDA)
# set(CMAKE_TOOLCHAIN_FILE "E:/PunkEngine/vcpkg/scripts/buildsystems/vcpkg.cmake"
#   CACHE STRING "Vcpkg toolchain file")

#假设已经安装好grpc了
find_package(Threads REQUIRED)
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf REQUIRED)
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

find_package(vsg)
find_package(vsgQt)
find_package(flatbuffers)
# find the optional vsgXchange that can be used for reading a range of image and 3d model formats and shader compilation
find_package(vsgXchange 1.0.5 QUIET)



# find the optional vsgImGui that can be used for GUI elements added into graphics windows.
find_package(vsgImGui QUIET)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

file(GLOB FORMS "src/ui/*.ui")
qt5_wrap_ui(HEADERS ${FORMS})

#find_package(vsgQt REQUIRED)


# set the use of C++17 globally as all examples require it
set(CMAKE_CXX_STANDARD 17)
aux_source_directory(src SRCS)


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/Video_Codec_SDK_12.0.16/Utils
    ${CUDA_INCLUDE_DIRS}
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/Video_Codec_SDK_12.0.16/Lib/linux/stubs/x86_64
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


add_executable(mainWindowApplication ${HEADERS} ${FORMS} ${SRCS} ${hw_proto_srcs} ${hw_grpc_srcs} ColorSpace.cu)

source_group("Form Files" FILES ${FORMS})


target_link_libraries(mainWindowApplication vsgQt vsg::vsg flatbuffers Qt5::Core Qt5::Widgets 
libnvcuvid.so libnvidia-encode.so ${CUDA_CUDA_LIBRARY}
${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})
