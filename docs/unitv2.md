# UnitV2

## Access to UnitV2 via network

Either via LAN over USB or via WiFi access point like M5UV2_xxxx

Note, neither wired not wireless interfaces provide internet access, so make sure that connection is not used as a default connection on your machine.

Both are bridged to the same IP address: addr:10.254.239.1

Open browser at http://addr:10.254.239.1 (no HTTPS).

Starts in **DEMO** mode, can switch to **Notebook** mode.




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
> video:x:28:m5stack
> 
> audio:x:29:m5stack
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

*Newer versions not compatible with installed kernel! Need to find how to install complete new system, incl kernel. FSBL (first stage bootloader) instructions for SD-card are missing.*


  * creates arm cross-compiler and libraries. Probably, arm-compiled from Linux distro could be used as well ...
  * use together with sysroot for development
  * copy output to target

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



## Compiled Buildroot binaries

Available at [OK Lab Cloud](https://cloud.ok-lab-karlsruhe.de/index.php/s/b5ZizSMweZxsjPa)


