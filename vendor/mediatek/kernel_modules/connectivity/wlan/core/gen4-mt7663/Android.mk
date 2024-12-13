###############################################################################
# Generally Android.mk can not get KConfig setting
# we can use this way to get
# include the final KConfig
# but there is no $(AUTO_CONF) at the first time (no out folder) when make
#
#ifneq (,$(wildcard $(AUTO_CONF)))
#include $(AUTO_CONF)
#include $(CLEAR_VARS)
#endif

###############################################################################
###############################################################################
# Generally Android.mk can not get KConfig setting                            #
#                                                                             #
# do not have any KConfig checking in Android.mk                              #
# do not have any KConfig checking in Android.mk                              #
# do not have any KConfig checking in Android.mk                              #
#                                                                             #
# e.g. ifeq ($(CONFIG_MTK_COMBO_WIFI), m)                                     #
#          xxxx                                                               #
#      endif                                                                  #
#                                                                             #
# e.g. ifneq ($(filter "MT6632",$(CONFIG_MTK_COMBO_CHIP)),)                   #
#          xxxx                                                               #
#      endif                                                                  #
#                                                                             #
# All the KConfig checking should move to Makefile for each module            #
# All the KConfig checking should move to Makefile for each module            #
# All the KConfig checking should move to Makefile for each module            #
#                                                                             #
###############################################################################
###############################################################################

LOCAL_PATH := $(call my-dir)
ifeq ($(MTK_WLAN_SUPPORT), yes)
ifneq (true,$(strip $(TARGET_NO_KERNEL)))
ifeq (MT7663,$(strip $(MTK_COMBO_CHIP)))
include $(CLEAR_VARS)
LOCAL_MODULE := wlan_drv_gen4_mt7663_prealloc.ko
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_INIT_RC := init.wlan_mt7663_drv.rc
LOCAL_SRC_FILES := $(patsubst $(LOCAL_PATH)/%,%,$(shell find $(LOCAL_PATH) -type f -name '*.[cho]')) Makefile

include $(MTK_KERNEL_MODULE)

include $(CLEAR_VARS)
LOCAL_MODULE := wlan_drv_gen4_mt7663.ko
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_INIT_RC := init.wlan_mt7663_drv.rc
LOCAL_SRC_FILES := $(patsubst $(LOCAL_PATH)/%,%,$(shell find $(LOCAL_PATH) -type f -name '*.[cho]')) Makefile

include $(MTK_KERNEL_MODULE)
endif
endif
endif
