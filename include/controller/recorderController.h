#pragma once

#include <codec/codec.h>

#include <string>
#include <memory>
#include <fstream>

class RecorderController
{
    cdc::Encoder* m_encoder;
    std::ofstream m_file;
    bool          m_recording   = false;
    uint32_t      m_frame_index = 0;

public:
    RecorderController() = default;

    ~RecorderController();

    // Start recording. nativeDevice is an opaque pointer to the native device (e.g. ID3D12Device*).
    bool start(const std::string& fileName, void* nativeDevice, uint32_t width, uint32_t height, cdc::DeviceType deviceType = cdc::DEVICE_TYPE_DX12, cdc::CodecType codecType = cdc::CODEC_TYPE_H264);

    // Stop recording and flush pending packets.
    void stop();

    // Submit a frame. native is an opaque pointer to the native frame resource (e.g. ID3D12Resource*).
    void submit_frame(void* native);

    bool is_recording() const { return m_recording; }
};
