#include <controller/recorderController.h>

#include <iostream>

RecorderController::~RecorderController()
{
    stop();
}

bool RecorderController::start(const std::string& fileName, void* nativeDevice, uint32_t width, uint32_t height, cdc::DeviceType deviceType, cdc::CodecType codecType)
{
    if (m_recording) return false;

    m_file.open(fileName, std::ios::binary);
    if (!m_file.is_open())
    {
        std::cout << "[record]: failed to open file: " << fileName << std::endl;
        return false;
    }

    cdc::CreateParams params;
    params.device     = nativeDevice;
    params.deviceType = deviceType;
    params.width      = width;
    params.height     = height;
    params.codecType  = codecType;
    // default to ABGR for DX12 NVENC buffer format (value chosen by previous quick implementation)
    params.pixelFormat = cdc::PIXEL_FORMAT_BGRA8;

    m_encoder = cdc::CreateEncoder(params);
    if (!m_encoder)
    {
        std::cout << "[record]: failed to create encoder." << std::endl;
        m_file.close();
        return false;
    }

    if (!m_encoder->Initialize(params))
    {
        std::cout << "[record]: failed to initialize encoder." << std::endl;
        m_file.close();
        return false;
    }

    m_recording   = true;
    m_frame_index = 0;
    std::cout << "[record]: recording started: " << fileName << std::endl;
    return true;
}

void RecorderController::stop()
{
    if (!m_recording) return;

    if (!m_encoder) return;

    cdc::CodecPacket packet;
    while (m_encoder->Flush(packet))
    {
        if (packet.data && packet.size > 0 && m_file.is_open())
        {
            m_file.write(reinterpret_cast<const char*>(packet.data), packet.size);
        }
        if (packet.data) delete[] static_cast<uint8_t*>(packet.data);
    }

    m_encoder->Destroy();

    if (m_file.is_open()) m_file.close();
    m_recording = false;
    std::cout << "[record]: recording stopped. Record frames: " << m_frame_index << std::endl;
}

void RecorderController::submit_frame(void* native)
{
    if (!m_recording || !m_encoder || !native) return;

    cdc::CodecPacket packet;
    if (!m_encoder->EncodeFrame(native, packet)) return;

    if (packet.data && packet.size > 0 && m_file.is_open())
    {
        m_file.write(reinterpret_cast<const char*>(packet.data), packet.size);
        m_file.flush();
    }

    if (packet.data) delete[] static_cast<uint8_t*>(packet.data);

    m_frame_index++;
}
