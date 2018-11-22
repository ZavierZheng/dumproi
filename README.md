## dumproi

This directory contains a tool for dumping the ROI capacitive image. In order to remove any Mac/Linux/Windows shell restriction, the tool was designed to be executed directly on the Android device.

### Setup

To build the tool run the following command:
```
ANDROID_NDK_ROOT=<your_ndk_root> make build
```

To push the tool on the Android device run the following command:
```
ANDROID_NDK_ROOT=<your_ndk_root> ARCH=<arch> make push
```

To build and deploy the tool on the android device run the following command:
```
ANDROID_NDK_ROOT=<your_ndk_root> ARCH=<arch> make all
```

### Examples

##### Read 7x7 ROI every 5ms
```
dumproi -l /sys/devices/platform/huawei_touch/roi_data \
        -d 5000
```

##### Read 10x10 ROI capimage
```
dumproi -l /sys/devices/platform/huawei_touch/roi_data \
        -c 10 -r 10
```
