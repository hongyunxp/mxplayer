# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# LOCAL_PREBUILT_LIBS := libffmpeg.so
# LOCAL_LDLIBS+= -L$(SYSROOT)/usr/lib -llog
#
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
#加入此标志，是因为FFmpeg是gcc 编译的 NDK 编译要连接的库是使用 g++ 编译的CPP文件，
#有可能出现‘UINT64_C’ was not declared in this scope的错误，为避免此错误加入此标志
#LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS=1

# 加入此头文件目录，主要考虑 ffmpeg 内部头文件的引用位置
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ffmpeg

# 添加调用的动态链接库  libffmpeg.so log
LOCAL_LDLIBS 	+= -L$(LOCAL_PATH) -lffmpeg -llog

#取消未找到定义的错误
#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

# 模块名称定义，生成是按照模块名称生成库
LOCAL_MODULE    := FFmpegJni

# 要编译的源文件
LOCAL_SRC_FILES := FFmpegJni.c

# 编译类型 [动态 、 静态]
include $(BUILD_SHARED_LIBRARY)