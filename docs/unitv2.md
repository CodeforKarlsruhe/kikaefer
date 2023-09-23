# UnitV2 Basics

## External projects
[Object detection](https://www.hackster.io/naveenbskumar/object-detection-using-m5stack-unitv2-and-edge-impulse-650a2f)

[Face mask detect](https://www.hackster.io/shahizat/tinyml-face-mask-detector-with-m5stack-core2-and-unitv2-0040be)


## Access to UnitV2 via network

Either via LAN over USB or via WiFi access point like M5UV2_xxxx

Note, neither wired not wireless interfaces provide internet access, so make sure that connection is not used as a default connection on your machine.

Both are bridged to the same IP address: addr:10.254.239.1

Open browser at http://addr:10.254.239.1 (no HTTPS).

Starts in **DEMO** mode

![](https://github.com/CodeforKarlsruhe/kikaefer/blob/a9441c28698eb4b5be98f55a78b77213772464e9/docs/imgs/scaled/stream.png)


can switch to **Notebook** mode.

![](https://github.com/CodeforKarlsruhe/kikaefer/blob/d7aaf98730fad695fb3e89d0e5c3d076e5b8cad7/docs/imgs/scaled/switch-to-notbook.png)

![](https://github.com/CodeforKarlsruhe/kikaefer/blob/d7aaf98730fad695fb3e89d0e5c3d076e5b8cad7/docs/imgs/scaled/notebook.png)



Demos:

  * Face detection

![](https://github.com/CodeforKarlsruhe/kikaefer/blob/5bb7a786baa379265e1b5789e347ea19df025561/docs/imgs/scaled/face.png)

  * Object detection

![](https://github.com/CodeforKarlsruhe/kikaefer/blob/d7aaf98730fad695fb3e89d0e5c3d076e5b8cad7/docs/imgs/scaled/object.png)

  * And others ...


## Login

user: m5stack

pwd: 12345678

or

user: root

pwd: 7d219bec161177ba75689e71edc1835422b87be17bf92c3ff527b35052bf7d1f


> ssh m5stack@10.254.239.1

**SSH failures**

Probably incorrect settings on unitv2 prevents direct login via ssh. 
Fix:

 * Open browser on http://10.254.239.1  
 * Switch to notebook mode (reload required)
 * Open new terminal (new -> terminal)
   * ![](https://github.com/CodeforKarlsruhe/kikaefer/blob/d7aaf98730fad695fb3e89d0e5c3d076e5b8cad7/docs/imgs/scaled/unitv2-open-terminal.png)
 * edit /etc/sshd_config
   * Set MaxAuthTries to specific value (e.g. 10)
 * Kill and restart sshd 


## WiFi setup

By default, one of the two WiFi networks is used to provide an access point. The second one can be used to connect as a client in order to access internet.

  * Edit /etc/wpa_supplicant and add a fixed external SSID like so:

```
ctrl_interface=/var/run/wpa_supplicant
eapol_version=1
ap_scan=1
fast_reauth=1

# new
network={
    ssid="karlsruhe.freifunk.net"
    key_mgmt=NONE
}


```

Stop wpa client
> unitv2% sudo start-stop-daemon -n wpa_supplicant -K
> 

Restart wpa client
> sudo start-stop-daemon  -b -x /usr/sbin/wpa_supplicant -S -- -B -Dnl80211 -iwlan0 -c /etc/wpa_supplicant.conf


# Development
We have some of the stuff explained below availabale for download. But read on, befor you grab it ...

<!--
## Compiled Buildroot binaries
With cross development tools fro Linux x86_64 (Fedora 37) available at [OK Lab Cloud](https://cloud.ok-lab-karlsruhe.de/s/FzaRmrF6D2qPKLo)
-->


## Compiled target folder

All libraries ready to copy to /media/sdcard/install available from [OK Lab Cloud](https://cloud.ok-lab-karlsruhe.de/s/6kWGXr7RdtXR8wi)




## Preparations and caveats

Prior to using the new programs, adjust the following settings, assuming the new buildroot target stuff  
has been copied to /media/sdcard/install/cross, 
so libraries are placed at /media/sdcard/install/cross/usr/lib/

> export LD_LIBRARY_PATH=/media/sdcard/install/cross/usr/lib:/lib:/usr/lib

Edit /etc/group to add m5stack to groups video and audio
> unitv2% grep m5stack /etc/group 
> 
> ...
> 
> video\:x\:28\:m5stack
> 
> audio\:x\:29\:m5stack
> 
> ...




Change group of /dev/video to video

> sudo chgrp video /dev/video0  


### Check memory

Most probably you have to kill the jupyter server before running larger models.

If not enough memory, zsh will kill your program. Check /var/log/messages for something like:

```
Aug  1 17:16:33 unitv2 user.err kernel: Out of memory: Kill process 1637 (yolo4ocv
_arm) score 547 or sacrifice child
Aug  1 17:16:33 unitv2 user.err kernel: Killed process 1637 (yolo4ocv_arm) total-v
m:95556kB, anon-rss:65732kB, file-rss:0kB, shmem-rss:0kB

```

### Permissions
You might need to sudo in order to run applications on SD-card. It is mounted as root with no write access for others - chmod and chown don't work on FAT filesystem.



## Directory structure

Default (factory reset): 

```
unitv2% ls payload 
bin                        server_core.py
data                       server_log.txt
images                     server_system_config.json
models                     static
notebook                   templates
run_notebook.py            tools
server.py                  uploads
unitv2% 
```



```
unitv2% ls ml 
framework     mqtt          ncnn          setldpath.sh  yolotest
unitv2% 
```

framework: recompiled application from [https://github.com/m5stack/UnitV2Framework](https://github.com/m5stack/UnitV2Framework)

ncnn: Embedded ML tools from [https://github.com/Tencent/ncnn.git](https://github.com/Tencent/ncnn.git) with assets from [https://github.com/nihui/ncnn-assets.git](https://github.com/nihui/ncnn-assets.git)


tflite: Tensorflow lite from [https://github.com/tensorflow/tensorflow.git](https://github.com/tensorflow/tensorflow.git)


### Development requirements

**Remember** there is no gui on the target, adjust source code for examples like ncnn yolovX (example: don't use cv::imshow)


buildroot: version 2020.08.02 from [https://github.com/buildroot/buildroot](https://github.com/buildroot/buildroot), config for arm cortex-a7, gcc-10, opencv4 with dnn, openmp end other stuff.

* [buildroot-2020.08.2.tar.bz2](https://buildroot.org/downloads/buildroot-2020.08.2.tar.bz2) from [archive](https://buildroot.org/downloads/)
* cp packages over from oklab cloud tar (additionally contains opencv) `rsync --ignore-existing -ravp ../../buildroot-2020.08.2/package package/`
* cp targetBr2020082/.config-with-imagemagic to config in new buildroot
    * optionally check correct settings (arch) with `make menuconfig`
    ```
    Target Architecture (ARM (little endian))  ---> 
    Target Binary Format (ELF)  --->                
    Target Architecture Variant (cortex-A7)  --->   
    Target ABI (EABIhf)  --->                       
    Floating point strategy (NEON)  --->            
    ARM instruction set (ARM)  --->         
    ```
    * if asked, unset vars before compiling

* sources from https://github.com/m5stack/UnitV2Framework.git can be compiled with the resulting arm compiler

*Newer versions not compatible with installed kernel! Need to find how to install complete new system, incl kernel. FSBL (first stage bootloader) instructions for SD-card are missing.*


  * creates arm cross-compiler and libraries. Probably, arm-compiled from Linux distro could be used as well ...
  * use together with sysroot for development
  * copy output to target

#### Building buildroot

**Get compiled target folder first! (see above)**

Get package [2020.08.02](https://buildroot.org/downloads/buildroot-2020.08.2.tar.bz2)


Extract files to buildroot-2020.08.2

copy config file from /comnpiled target folder*/targetBr2020082/.config-with-imagemagic  to buildroot-2020.08.2/.config

Enter buildroot-2020.08.2

> make

You will get errors in two places, one for *c-stack.c" the other for "libfakeroot.c"

Copy the attached files to the proper locations like:

> cp <modified>/libfakeroot.c <workdir>/buildroot-2020.08.2/output/build/host-fakeroot-1.20.2/

> cp <modified>/c-stack.c <workdir>/buildroot-2020.08.2/output/build/host-m4-1.4.18/lib

*Fix with patches to be made ...*

Make again...

Should compile, maybe error at the very end when trying to create a sysroot filesystem. Can be ignored.

cross compiler is at <workdir>/buildroot-2020.08.2/output/host/bin/arm-buildroot-linux-gnueabihf-gcc etc.

sysroot is at <workdir>/buildroot-2020.08.2/output/host/arm-buildroot-linux-gnueabihf/sysroot



#### Using buildroot


Example, assume buildroot is at ../buildroot-2020.08.2


```
#!/bin/bash

# Set the cross-compiler and sysroot paths
CROSS_COMPILER=../buildroot-2020.08.2/output/host/bin/arm-buildroot-linux-gnueabihf-gcc
CROSS_COMPILERPP=../buildroot-2020.08.2/output/host/bin/arm-buildroot-linux-gnueabihf-g++
SYSROOT=../buildroot-2020.08.2/output/host/arm-buildroot-linux-gnueabihf/sysroot

# OCV includes
OCVINCS="-I ${SYSROOT}/usr/include/opencv4"
# OCV libs. maybe more or less are needed, depends on application

OCVLIBS="-l opencv_core -l opencv_dnn -l opencv_dnn_objdetect -l opencv_imgcodecs -l opencv_imgproc -l opencv_videoio -l opencv_video "


# Set the compiler flags
CC="${CROSS_COMPILER} --sysroot=${SYSROOT}"
CPP="${CROSS_COMPILERPP} --sysroot=${SYSROOT}"
CXX="${CROSS_COMPILER} --sysroot=${SYSROOT}"
CFLAGS="-O2 -I${SYSROOT}/usr/include"
CPPFLAGS="-I${SYSROOT}/usr/include -I${SYSROOT}/../include/c++/8.4.0 -I${SYSROOT}/../include/c++/8.4.0/arm-buildroot-linux-gnueabihf ${OCVINCS} -O3 -mfpu=neon-vfpv4 -mcpu=cortex-a7 -ftree-vectorize -funroll-loops -finline-functions -falign-functions -falign-loops -fopenmp"
LDFLAGS="-L ${SYSROOT}/usr/lib ${OCVLIBS} -l pthread"

echo "CPPFLAGS: ${CPPFLAGS}"

# Compile the applications
${CPP} ${CPPFLAGS} ${LDFLAGS} -o cvexample_arm cvexample.cpp


```

To compile ncnn, tflite and framework use Cmake cross-configuration *armTools.cmake like* :

```
[kugel@tux2 unitv2]$ cat docs/armTools.cmake 
# for https://github.com/Tencent/ncnn/wiki/how-to-build#build-for-arm-cortex-a-family-with-cross-compiling
# Specify the CMake version (optional but recommended)
cmake_minimum_required(VERSION 3.0)

# Set the cross-compilation environment
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR arm)

# Set the cross-compilation toolchain prefix
SET(CMAKE_C_COMPILER   /home/kugel/temp/m5/unitv2/buildroot-2020.08.2/output/host/bin/arm-linux-gcc)
SET(CMAKE_CXX_COMPILER /home/kugel/temp/m5/unitv2/buildroot-2020.08.2/output/host/bin/arm-linux-g++)

# Set other toolchain-related variables if needed (e.g., flags, libraries, etc.)
SET(CMAKE_FIND_ROOT_PATH /home/kugel/temp/m5/unitv2/buildroot-2020.08.2/output/host/arm-buildroot-linux-gnueabihf/sysroot/)

# maybe set cflags as well, unclear how to do this ...


```

Build like:
> cd build
> 
> cmake -DCMAKE_TOOLCHAIN_FILE=../armTools.cmake 
> 

For ncnn examples include: 
> -DNCNN_BUILD_EXAMPLES=ON


### Examples 

**NCNN**

```
unitv2% ../mobilenetssd ../../yolotest/data/dog.jpg      
12 = 0.99776 at 137.99 209.13 186.70 x 332.18
7 = 0.99636 at 465.98 72.85 222.67 x 98.30
2 = 0.99473 at 106.52 141.06 467.29 x 273.91
unitv2% 

unitv2% ../mobilenetv2ssdlite ../../yolotest/data/dog.jpg
2 = 0.96774 at 130.60 118.44 436.14 x 315.11
7 = 0.83982 at 465.92 83.44 235.06 x 94.72
12 = 0.83866 at 128.00 212.55 185.20 x 328.66
8 = 0.47114 at 128.00 212.55 185.20 x 328.66
unitv2% 

unitv2% ../yolov3 ../../yolotest/data/dog.jpg
12 = 0.98737 at 127.12 230.85 196.55 x 294.29
7 = 0.98136 at 458.35 82.46 224.45 x 86.46
2 = 0.49455 at 153.89 110.86 422.81 x 351.26
unitv2% 

unitv2% ../yolov4 ../../yolotest/data/dog.jpg        
NCNN Init time 858.99ms
NCNN detection time 4891.71ms
8 = 0.97062 at 461.72 79.59 236.36 x 87.10
17 = 0.77207 at 130.54 230.53 191.31 x 298.96
2 = 0.29866 at 67.89 95.03 524.36 x 392.41
1 = 0.27511 at 60.46 80.28 37.39 x 41.32
NCNN OpenCV draw result time 30.82ms

unitv2% ../nanodet ../../yolotest/data/dog.jpg   
16 = 0.64709 at 130.41 218.59 180.60 x 321.63
1 = 0.46220 at 103.82 117.79 458.14 x 312.60
7 = 0.43779 at 378.56 51.47 308.78 x 113.11
2 = 0.41042 at 378.68 72.54 308.85 x 93.12
unitv2% 

unitv2% ../squeezenet ../../yolotest/data/dog.jpg
537 = 0.424024
249 = 0.124082
248 = 0.072996
unitv2% 

```


**OpenCV4**

Minimal example:
```
#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    std::cout << "Start image conversion." << std::endl;

    // Read the input image
    cv::Mat image = cv::imread("data/dog.jpg");

    // Check if the image was successfully loaded
    if (image.empty()) {
        std::cout << "Failed to read the image." << std::endl;
        return -1;
    }

    // Scale the image to double size
    cv::Mat scaledImage;
    cv::resize(image, scaledImage, cv::Size(), 2.0, 2.0);

    // Convert the image to grayscale
    cv::Mat grayImage;
    cv::cvtColor(scaledImage, grayImage, cv::COLOR_BGR2GRAY);

    // Write the processed images to files
    cv::imwrite("scaled_dog.jpg", scaledImage);
    cv::imwrite("gray_dog.jpg", grayImage);

    return 0;
}

```

YoloV4 with *tiny* paramters from *darknet* dnn model. processing time ~5s
  * See also [https://github.com/pjreddie/darknet/issues/2201](https://github.com/pjreddie/darknet/issues/2201)


**Framework**

Make sure to have GUI functions disabled in the source code.


**TFlite**
Assume tensorflow checked out into subdir tf_src

Build like: 
> mkdir -p tflite_arm/build
> 
> cd tflite_arm/build
> 
> cmake -DCMAKE_TOOLCHAIN_FILE=../armTools.cmake -DCMAKE_INSTALL_PREFIX=`pwd`/out ../../tf_src/tensorflow/lite
>
> make -j8
> 
> make install
>

creates *libtensorflow-lite*


**Nanodet**

Nanodet plus m416 works on desktop. Works on UnitV2 too, provided we 
use "light_mode" in the extractor like so (nanodet.cpp):

```
std::vector<BoxInfo> NanoDet::detect(cv::Mat image, float score_threshold, float nms_threshold)
{
    ncnn::Mat input;
    preprocess(image, input);
    auto ex = this->Net->create_extractor();

    // SET LIGHT MODE HERE!
    ex.set_light_mode(true);

    ex.set_num_threads(2);
```


With NCNN:
  * Make sure to set PATH: 
  >
  > export ncnn_DIR=YOUR_NCNN_PATH/build/install/lib/cmake/ncnn
  >
  > like:  export ncnn_DIR=/home/kugel/temp/m5/unitv2/ncnn/build_arm/install/lib/cmake/ncnn
  > 
  > cmake -DCMAKE_TOOLCHAIN_FILE=../armTools.cmake .. 
  > 
  

**Other tools**

MQTT works, both python (original installation) and c++ (new installation)


**Max Sittinger**

https://github.com/maxsitt/insect-detect-docs/tree/main


## Video recording
FFMPEG works out of the box. Make sure to place video onto the sdc-card at /media/sdcard

Note that by default both sdcard and video are only fully usable by root, so start with 

> 
> sudo su
>
> cd /media/sdcard
>

```
video only
ffmpeg -f v4l2 -i /dev/video0 -c:v mpeg4 -s 640x480 output4.mp4


with audio like:
ffmpeg -f v4l2 -i /dev/video0  -f alsa -i default -c:v mpeg4 -s 640x480 -c:a aac -ar 44100 -ac 2 -b:a 192k output-x.mp4

framerates: 
  * 30fps @ 640x480
  * 20fps @ 1280x720
  * 9fps @ 1920x1080
 

h264 and vp8 should be present, but don't work.

```

## Streaming video

Use ffmpeg to stream to a local network port. Note: destination IP must be given!

Assume, destination IP is 192.168.0.118

>
> ffmpeg -f v4l2 -i /dev/video0 -c:v mpeg4 -s 640x480 -f rtp rtp://192.168.0.118:8000 
> 

After starting, ffmpeg will print the SDP settings like: 

```
SDP:
v=0
o=- 0 0 IN IP4 127.0.0.1
s=No Name
c=IN IP4 192.168.0.118
t=0 0
a=tool:libavformat 58.29.100
m=video 8000 RTP/AVP 96
b=AS:200
a=rtpmap:96 MP4V-ES/90000
a=fmtp:96 profile-level-id=1

```

Copy everything after *SDP:* and save as file unitv2.sdp on receiver machine

Use ffplay to receive stream. Note: machines must be in same netwerk and receiver firewall must pass port 8000 (or whatever selected). sdp descrription must be available

>
> ffplay -protocol_whitelist file,udp,rtp -i unitv2.sdp 
>

## Autostart 

start scripts are located at /etc/init.d/
* you can add your own
* disable S85runpayload by renaming: `mv /etc/init.d/S85runpayload /etc/init.d/_S85runpayload`
* start-stop-daemon 

