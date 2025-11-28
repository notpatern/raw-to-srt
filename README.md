# Raw to SRT
This project allows its user to take in raw video feed and stream it to SRT over any network they might want to stream to.
Linux only.
Uses the Elecard Gstreamer SDK.

# Installation
```shell
$ git clone https://github.com/notpatern/raw-to-srt.git
$ cd raw-to-srt
$ sudo chmod +x compile.sh
$ ./compile.sh
```

# Running the App
## Prerequisites
- [OBS](https://github.com/obsproject/obs-studio)
- [pulseaudio](https://www.freedesktop.org/wiki/Software/PulseAudio/)
- [pipewire](https://pipewire.org/) (for virtual device creation)
- [Elecard Gstreamer SDK](https://elecard.com/products/sdks/gstreamer-codec-sdk)
- [v4l2-utils](https://github.com/gjasny/v4l-utils)
- [nlohmann/json](https://github.com/nlohmann/json)

## Virtual Device
Create a file (and the directories if they do not exist already) such as ```~/.config/pipewire/pipewire.conf.d/obs-virtual-audio.conf```
```json
context.objects = [
    {
        factory = adapter
        args = {
            factory.name     = support.null-audio-sink
            node.name        = "OBS_Audio"
            node.description = "OBS Virtual Audio"
            media.class      = "Audio/Sink"
            object.linger    = 1
            audio.position   = [ FL FR ]
        }
    }
]
```
This will create a virtual audio device, which the program is gonna access to get OBS' audio sources.
After creating this file, restart your pipewire.
```shell
$ systemctl --user restart pipewire
```
This change will persist through restarts.

## OBS
In OBS go to: ```Settings/Audio/Advanced``` and select your newly created virtual audio device as the ```Monitoring Device```. This reroutes designated audio sources to this device.
Once this is done, head back onto your scene and Right Click one of your audio sources and click ```Advanced Audio Properties``` here you will see a dropdown for each audio source that says ```Audio Monitoring```. Set the sources you want to be able to hear in your stream to ```Monitor Only```.
Once you're done, start the ```Virtual Camera```.

## Config
In the raw-to-srt directory you will find a file named ```config.json```. In this file you will be able to set your virtual camera as the video input, your virtual audio device as the audio input, as well as other settings.

## Running
When all of the steps above are done, simply run
```shell
$ ./raw-to-srt
```
