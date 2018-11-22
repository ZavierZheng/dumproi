ifndef ANDROID_NDK_ROOT
$(error ANDROID_NDK_ROOT is not set. Did you do ANDROID_NDK_ROOT = <your root> make ?)
endif

build:
	$(ANDROID_NDK_ROOT)/ndk-build

push:
ifndef ARCH
	$(error ARCH variable is not set)
endif
	adb remount
	adb push -p libs/$(ARCH)/dumproi /system/bin

all: build push

clean:
	$(ANDROID_NDK_ROOT)/ndk-build clean
	rm -vrf libs obj
