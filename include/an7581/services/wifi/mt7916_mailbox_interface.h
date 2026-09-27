/* SPDX-License-Identifier: MIT */
#ifndef NPU_WIFI_MT7916_MAILBOX_INTERFACE_H
#define NPU_WIFI_MT7916_MAILBOX_INTERFACE_H

/*
 * Mailbox interface numbering for the MT7916 host driver.
 *
 * MT7916 is connac2: it has no RRO engine, so there is no MSDU-page ring,
 * no RRO indication ring and no RXDMAD_C ring. The two WFDMA data rings are
 * published here as plain EAGLE_DATA rings and the reorder state lives in
 * the NPU's own iNode tables instead of host-allocated DRAM tables.
 *
 * The MT7996 host driver reuses interface numbers across commands, and so
 * does the MT7916 one: keep each command-specific role named even when two
 * roles share a value.
 *
 * NOTE on interfaces 6 and 11: on MT7996 these carry the MSDU-page band1
 * descriptors and their publication address. MT7916 has no MSDU-page ring at
 * all, so they are re-purposed here as the band1 TX free-pointer and TX
 * packet interfaces. This is what lets the MT7916 host driver keep using the
 * band / band+5 / band+10 convention of mt7996_npu_txd_init() for a second
 * band without colliding with an MSDU-page ring -- a collision the MT7992
 * path does have when running against an MT7996-profile firmware image.
 */
enum npu_wifi_mt7916_mailbox_interface {
  NPU_WIFI_MT7916_RX_DATA_BAND0_INTERFACE = 0,
  NPU_WIFI_MT7916_TX_QUEUE_BAND0_INTERFACE = 0,
  NPU_WIFI_MT7916_TX_DONE_DESCRIPTOR_INTERFACE = 0,
  NPU_WIFI_MT7916_DEBUG_COUNTER_BAND0_INTERFACE = 0,
  NPU_WIFI_MT7916_RX_DATA_BAND1_INTERFACE = 1,
  NPU_WIFI_MT7916_TX_QUEUE_BAND1_INTERFACE = 1,
  NPU_WIFI_MT7916_DEBUG_COUNTER_BAND1_INTERFACE = 1,
  NPU_WIFI_MT7916_RRO_INFORMATION_INTERFACE = 3,
  NPU_WIFI_MT7916_TX_FREE_POINTER_BAND0_INTERFACE = 5,
  NPU_WIFI_MT7916_TX_FREE_POINTER_BAND1_INTERFACE = 6,
  NPU_WIFI_MT7916_RX_TX_DONE_INTERFACE = 10,
  NPU_WIFI_MT7916_TX_DONE_REGISTER_INTERFACE = 10,
  NPU_WIFI_MT7916_TX_PACKET_BAND0_INTERFACE = 10,
  NPU_WIFI_MT7916_TX_PACKET_BAND1_INTERFACE = 11,
  NPU_WIFI_MT7916_RRO_CPU_INDEX_INTERFACE = 15,
};

#endif
