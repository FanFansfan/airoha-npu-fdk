/* SPDX-License-Identifier: MIT */
#ifndef NPU_WIFI_WLAN_TARGET_H
#define NPU_WIFI_WLAN_TARGET_H

/*
 * The NPU firmware image is assembled per WLAN chip: the board binding
 * layer (src/an7581/platform/wifi_<chip>_*.c) decides which control plane
 * and which RX ring profile table get linked in.
 *
 * Define NPU_WIFI_TARGET_MT7916 in the build to produce an image that
 * offloads a MT7916 (connac2) instead of a MT7996 (connac3).
 *
 * These are macros and not enumeration constants on purpose: the chip
 * selection is consumed by #if, and the preprocessor does not know about
 * C enumerators (an undefined identifier in #if evaluates to 0).
 */
#define NPU_WIFI_WLAN_CHIP_MT7996 0
#define NPU_WIFI_WLAN_CHIP_MT7916 1

#ifdef NPU_WIFI_TARGET_MT7916
#define NPU_WIFI_WLAN_CHIP NPU_WIFI_WLAN_CHIP_MT7916
#else
#define NPU_WIFI_WLAN_CHIP NPU_WIFI_WLAN_CHIP_MT7996
#endif

#endif
