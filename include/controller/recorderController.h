#pragma once

#include <string>
#include <memory>
#include <fstream>

#include <codec/codec.h>

class RecorderController
{
    std::unique_ptr<cdc::Encoder> m_encoder;
    std::ofstream                 m_file;
    bool                          m_recording = false;

public:
    RecorderController() = default;

    ~RecorderController();

    // Start recording. nativeDevice is an opaque pointer to the native device (e.g. ID3D12Device*).
    bool start(const std::string& fileName, void* nativeDevice, uint32_t width, uint32_t height, cdc::DeviceType deviceType = cdc::DEVICE_TYPE_DX12, cdc::CodecType codecType = cdc::CODEC_TYPE_H264);

    // Stop recording and flush pending packets.
    void stop();

    // Submit a frame. nativeResource is an opaque pointer to the native frame resource (e.g. ID3D12Resource*).
    bool submit_frame(void* nativeResource);

    bool is_recording() const { return m_recording; }
};
