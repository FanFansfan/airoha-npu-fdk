/* SPDX-License-Identifier: MIT */
#include "an7581/arch/riscv/cache.h"

void an7581_l1_dcache_discard(const void *address) {
#if defined(__riscv)
  __asm__ volatile(".insn r 0x73, 0, 0x7e, x0, %0, x2"
                   :
                   : "r"(address)
                   : "memory");
#else
  (void)address;
#endif
}
