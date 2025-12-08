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
    int transport = cfg.value("transport", 4);
    int gopLength = cfg.value("gop_length", 2);
    int performance = cfg.value("performance", 0);
    int profile = cfg.value("profile", 100);
    int entropyMode = cfg.value("entropy_mode", 1);
    int pictureMode = cfg.value("picture_mode", 1);
    int bitrateMode = cfg.value("bitrate_mode", 2);
    bool multicast = cfg.value("multicast", false);

    std::string sinkConfig;
    if (transport == 1) {
        if (multicast) {
            sinkConfig = "enwsink transport=1 host=239.255.0.1 port=" + std::to_string(outputPort) +
                        " interface=" + outputIP;
        } else {
            sinkConfig = "enwsink transport=1 host=" + outputIP + " port=" + std::to_string(outputPort);
        }
    } else if (transport == 4) {
        sinkConfig = "enwsink transport=4 port=" + std::to_string(outputPort) + " interface=0.0.0.0 rc-mode=0";
    } else {
        std::cout << "Invalid transport mode. Use 1 for UDP or 4 for SRT\n";
        return 1;
    }

    std::string command =
        "gst-launch-1.0 "
        "pulsesrc device=\"" + audioDevice + "\" ! "
        "audioconvert ! audioresample ! "
        "eaacenc bitrate=128000 ! "
        "mux. "
        "v4l2src device=" + videoDevice + " ! "
        "videoconvert ! "
        "eavcenc profile=" + std::to_string(profile) + " "
        "entropy-mode=" + std::to_string(entropyMode) + " "
        "picture-mode=" + std::to_string(pictureMode) + " "
        "bitrate-mode=" + std::to_string(bitrateMode) + " "
        "bitrate-avg=" + std::to_string(videoBitrate) + " "
        "gop-max-length=" + std::to_string(gopLength) + " "
        "performance=" + std::to_string(performance) + " ! "
        "mux. "
        "empegmux name=mux ! "
        + sinkConfig;

    std::cout << "Running command:\n" << command << "\n\n";

    int result = std::system(command.c_str());

    if (result == 0) {
        std::cout << "Pipeline running successfully\n";
    } else {
        std::cout << "Pipeline error, exit code: " << result << "\n";
    }

    return 0;
}
