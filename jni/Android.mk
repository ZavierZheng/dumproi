LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie
LOCAL_SRC_FILES:= dumproi.c
LOCAL_MODULE := dumproi
include $(BUILD_EXECUTABLE)
