/* SPDX-License-Identifier: MIT */
#ifndef AN7581_WIFI_MT7996_RRO_CONTROL_LIFECYCLE_H
#define AN7581_WIFI_MT7996_RRO_CONTROL_LIFECYCLE_H

#include "an7581/platform/core56_dispatch.h"
#include "an7581/services/wifi/mt7996_control_plane.h"
#include "an7581/services/wifi/mt7996_rro_pipeline.h"
#include "an7581/services/wifi/rro_metadata.h"
#include "an7581/services/wifi/wlan_target.h"

/*
 * The board binding layer is shared, but the control plane it drives is not:
 * an MT7916 image must own the MT7916 control plane (two EAGLE_DATA rings, no
 * MSDU-page arena) instead of the MT7996 one. Both control planes keep a
 * layout-compatible config/state pair, so the switch is a typedef plus three
 * function aliases and the lifecycle code itself is unchanged.
 */
#if NPU_WIFI_WLAN_CHIP == NPU_WIFI_WLAN_CHIP_MT7916
#include "an7581/services/wifi/mt7916_control_plane.h"

typedef struct npu_wifi_mt7916_control_plane an7581_wifi_control_plane;
typedef struct npu_wifi_mt7916_control_plane_config
    an7581_wifi_control_plane_config;
#define AN7581_WIFI_CONTROL_PLANE_ADDITIONAL_BACKEND_LIMIT                     \
  NPU_WIFI_MT7916_CONTROL_ADDITIONAL_BACKEND_LIMIT
#define an7581_wifi_control_plane_initialize                                   \
  npu_wifi_mt7916_control_plane_initialize
#define an7581_wifi_control_plane_bind_backends                                \
  npu_wifi_mt7916_control_plane_bind_backends
#define an7581_wifi_control_plane_prepare_reinitialization                     \
  npu_wifi_mt7916_control_plane_prepare_reinitialization
#else
typedef struct npu_wifi_mt7996_control_plane an7581_wifi_control_plane;
typedef struct npu_wifi_mt7996_control_plane_config
    an7581_wifi_control_plane_config;
#define AN7581_WIFI_CONTROL_PLANE_ADDITIONAL_BACKEND_LIMIT                     \
  NPU_WIFI_MT7996_CONTROL_ADDITIONAL_BACKEND_LIMIT
#define an7581_wifi_control_plane_initialize                                   \
  npu_wifi_mt7996_control_plane_initialize
#define an7581_wifi_control_plane_bind_backends                                \
  npu_wifi_mt7996_control_plane_bind_backends
#define an7581_wifi_control_plane_prepare_reinitialization                     \
  npu_wifi_mt7996_control_plane_prepare_reinitialization
#endif

#define AN7581_WIFI_MT7996_RRO_CONTROL_WORKER_HART_MASK                        \
  ((UINT32_C(1) << AN7581_CORE5_HART) | (UINT32_C(1) << AN7581_CORE6_HART))

enum an7581_wifi_mt7996_rro_control_lifecycle_state {
  AN7581_WIFI_MT7996_RRO_CONTROL_LIFECYCLE_UNINITIALIZED = 0,
  AN7581_WIFI_MT7996_RRO_CONTROL_LIFECYCLE_WAITING_FOR_CONFIGURATION,
  AN7581_WIFI_MT7996_RRO_CONTROL_LIFECYCLE_ACTIVATION_GATED,
  AN7581_WIFI_MT7996_RRO_CONTROL_LIFECYCLE_INITIALIZING_PIPELINE,
  AN7581_WIFI_MT7996_RRO_CONTROL_LIFECYCLE_BINDING_BACKENDS,
  AN7581_WIFI_MT7996_RRO_CONTROL_LIFECYCLE_INITIALIZING_CONTROL_PLANE,
  AN7581_WIFI_MT7996_RRO_CONTROL_LIFECYCLE_PUBLISHING_RUNTIME,
  AN7581_WIFI_MT7996_RRO_CONTROL_LIFECYCLE_WAKING_WORKERS,
  AN7581_WIFI_MT7996_RRO_CONTROL_LIFECYCLE_RETRYABLE_FAILURE,
  AN7581_WIFI_MT7996_RRO_CONTROL_LIFECYCLE_ACTIVE,
};

typedef enum npu_runtime_result (*an7581_wifi_mt7996_rro_control_worker_wake)(
    void *context, uint32_t hart_mask);

struct an7581_wifi_mt7996_rro_control_platform_config {
  const struct npu_wifi_mt7996_rro_pipeline_config *pipeline;
  const an7581_wifi_control_plane_config *control_plane;
  struct an7581_core56_dispatch *dispatch;
  an7581_wifi_mt7996_rro_control_worker_wake wake_workers;
  void *wake_context;
};

struct an7581_wifi_mt7996_rro_control_platform {
  struct npu_wifi_mt7996_rro_pipeline_config pipeline_config;
  an7581_wifi_control_plane_config control_plane_config;
  struct npu_wifi_backend_binding
      additional_backends[AN7581_WIFI_CONTROL_PLANE_ADDITIONAL_BACKEND_LIMIT];
  struct npu_wifi_mt7996_rro_pipeline pipeline;
  an7581_wifi_control_plane control_plane;
  struct an7581_core56_dispatch *dispatch;
  an7581_wifi_mt7996_rro_control_worker_wake wake_workers;
  void *wake_context;
  size_t external_backend_count;
  size_t rro_backend_count;
#if NPU_WIFI_WLAN_CHIP == NPU_WIFI_WLAN_CHIP_MT7916
  /* MT7916 has no chip RRO engine, so the NPU owns reordering and needs
   * the reorder machinery that the MT7996 ASIC provides in hardware:
   *   - icv_error_table: one bit per (wcid, tid) marking stations whose
   *     frames failed the integrity check and must not take the fast path
   *   - rro_table_backend: the per-BA-session reorder windows
   */
  struct npu_wifi_rro_table_backend rro_table_backend;
  volatile uint32_t *rro_icv_error_table;
  uint32_t rro_page_pool_address;
  bool rro_page_pool_address_valid;
#endif
  bool initialized;
};

struct an7581_wifi_mt7996_rro_control_lifecycle_config {
  struct npu_wifi_configuration *configuration;
  struct an7581_wifi_mt7996_rro_control_platform *platform;
  bool activation_allowed;
};

struct an7581_wifi_mt7996_rro_control_lifecycle {
  struct npu_wifi_configuration *configuration;
  struct an7581_wifi_mt7996_rro_control_platform *platform;
  enum an7581_wifi_mt7996_rro_control_lifecycle_state state;
  enum npu_runtime_result last_status;
  uint32_t step_count;
  uint32_t configuration_wait_count;
  uint32_t activation_gate_count;
  uint32_t pipeline_attempt_count;
  uint32_t backend_bind_attempt_count;
  uint32_t control_plane_attempt_count;
  uint32_t publication_attempt_count;
  uint32_t wake_attempt_count;
  uint32_t retryable_failure_count;
  bool activation_allowed;
  bool pipeline_initialized;
  bool backends_bound;
  bool control_plane_initialized;
  bool runtime_published;
  bool workers_woken;
  bool initialized;
};

struct an7581_wifi_mt7996_rro_control_lifecycle_result {
  enum an7581_wifi_mt7996_rro_control_lifecycle_state state;
  enum npu_runtime_result status;
  bool waiting_for_configuration;
  bool activation_gated;
  bool pipeline_initialized;
  bool backends_bound;
  bool control_plane_initialized;
  bool runtime_published;
  bool workers_woken;
  bool active;
};

enum npu_runtime_result an7581_wifi_mt7996_rro_control_platform_initialize(
    struct an7581_wifi_mt7996_rro_control_platform *platform,
    const struct an7581_wifi_mt7996_rro_control_platform_config *config);
enum npu_runtime_result an7581_wifi_mt7996_rro_control_lifecycle_initialize(
    struct an7581_wifi_mt7996_rro_control_lifecycle *lifecycle,
    const struct an7581_wifi_mt7996_rro_control_lifecycle_config *config);
enum npu_runtime_result an7581_wifi_mt7996_rro_control_lifecycle_step(
    struct an7581_wifi_mt7996_rro_control_lifecycle *lifecycle,
    struct an7581_wifi_mt7996_rro_control_lifecycle_result *result);

#endif
