#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>

int main() {
    std::ifstream cfgFile("config.json");
    if (!cfgFile.is_open()) {
        std::cerr << "config.json not in current directory\n";
        return 1;
    }

    nlohmann::json cfg;

    try {
        cfgFile >> cfg;
    } catch (const std::exception& e) {
        std::cerr << "error parsing config.json: " << e.what() << "\n";
        return 1;
    }

    std::string audioDevice = cfg.value("audio_device", std::string("OBS_Audio"));
    std::string videoDevice = cfg.value("video_device", std::string("/dev/video2"));
    int videoBitrate = cfg.value("video_bitrate", 8000000);
    std::string outputIP = cfg.value("output_ip", std::string("127.0.0.1"));
    int outputPort = cfg.value("output_port", 9000);

    std::string command =
        "gst-launch-1.0 "
        "pulsesrc device=\"" + audioDevice + "\" ! "
        "audioconvert ! audioresample ! "
        "eaacenc bitrate=128000 ! "
        "mux. "
        "v4l2src device=" + videoDevice + " ! "
        "videoconvert ! "
        "eavcenc profile=100 entropy-mode=1 picture-mode=1 "
        "bitrate-mode=2 bitrate-avg=" + std::to_string(videoBitrate) + " "
        "gop-max-length=24 performance=0 ! "
        "mux. "
        "empegmux name=mux ! "
        "enwsink transport=4 port=" + std::to_string(outputPort) + " "
        "interface=" + outputIP + " rc-mode=0";

    std::cout << "Running command:\n" << command << "\n\n";

    int result = std::system(command.c_str());

    if (result == 0) {
        std::cout << "Pipeline running successfully\n";
    } else {
        std::cerr << "Pipeline error, exit code: " << result << "\n";
    }

    return 0;
}

