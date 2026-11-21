#pragma once

#include "FMBrain.h"
#include "Trainer.h"

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace XboxOTFM {

class WebBridge {
public:
    static constexpr size_t kMaxPayloadBytes = 16ULL * 1024ULL * 1024ULL; // 16 MB hard cap

    WebBridge(FMBrain& brain, Trainer& trainer, std::atomic<uint32_t>& activeResolution, std::atomic<bool>& isTraining);
    ~WebBridge();

    bool Start(uint16_t port = 8080);
    void Stop();

    std::string LocalEndpointUrl() const;
    void AppendLogLine(const std::string& line);
    std::string ReadCrashAndConsoleLogs() const;

    static void WriteCrashLogToDisk(const std::string& entry);
    static std::string CrashLogFilePath();

private:
    void ServerThreadLoop();
    void HandleClient(uintptr_t clientSocket);

    std::string DetectLanIPv4() const;
    bool DecodeImageBytesToCHW(const uint8_t* data, size_t size, std::vector<float>& outCHW, uint32_t& outW, uint32_t& outH);
    uint32_t ProcessUploadPayload(const std::string& contentType, const std::vector<uint8_t>& body);
    std::vector<uint8_t> BuildDreamBmp() const;
    std::string BuildMobileIndexHtml() const;

    FMBrain& m_brain;
    Trainer& m_trainer;
    std::atomic<uint32_t>& m_activeResolution;
    std::atomic<bool>& m_isTraining;

    uint16_t m_port = 8080;
    std::string m_lanIp = "127.0.0.1";
    std::atomic<bool> m_running{ false };
    uintptr_t m_listenSocket = static_cast<uintptr_t>(-1);
    std::thread m_thread;

    mutable std::mutex m_logMutex;
    std::vector<std::string> m_consoleLogs;
};

} // namespace XboxOTFM
