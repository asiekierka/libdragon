/**
 * @file n64sys.h
 * @brief N64 System Interface
 * @ingroup n64sys
 */
#ifndef __LIBDRAGON_N64SYS_H
#define __LIBDRAGON_N64SYS_H

#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include "cop0.h"
#include "cop1.h"

/**
 * @addtogroup n64sys
 * @{
 */

/**
 * @brief Indicates whether we are running on a vanilla N64 or a iQue player
 */
extern int __bbplayer;

/**
 * @brief Frequency of the RCP
 */
#define RCP_FREQUENCY    (__bbplayer ? 96000000 : 62500000)

/**
 * @brief Frequency of the MIPS R4300 CPU
 */
#define CPU_FREQUENCY    (__bbplayer ? 144000000 : 93750000)

/**
 * @brief void pointer to cached and non-mapped memory start address
 */
#define KSEG0_START_ADDR ((void*)0x80000000)

/**
 * @brief Return the uncached memory address for a given address
 *
 * @param[in] _addr
 *            Address in RAM to convert to an uncached address
 * 
 * @return A void pointer to the uncached memory address in RAM
 */
#define UncachedAddr(_addr) ((void *)(((unsigned long)(_addr))|0x20000000))

/**
 * @brief Return the uncached memory address for a given address
 *
 * @param[in] _addr
 *            Address in RAM to convert to an uncached address
 * 
 * @return A short pointer to the uncached memory address in RAM
 */
#define UncachedShortAddr(_addr) ((short *)(((unsigned long)(_addr))|0x20000000))

/**
 * @brief Return the uncached memory address for a given address
 *
 * @param[in] _addr
 *            Address in RAM to convert to an uncached address
 * 
 * @return An unsigned short pointer to the uncached memory address in RAM
 */
#define UncachedUShortAddr(_addr) ((unsigned short *)(((unsigned long)(_addr))|0x20000000))

/**
 * @brief Return the uncached memory address for a given address
 *
 * @param[in] _addr
 *            Address in RAM to convert to an uncached address
 * 
 * @return A long pointer to the uncached memory address in RAM
 */
#define UncachedLongAddr(_addr) ((long *)(((unsigned long)(_addr))|0x20000000))

/**
 * @brief Return the uncached memory address for a given address
 *
 * @param[in] _addr
 *            Address in RAM to convert to an uncached address
 * 
 * @return An unsigned long pointer to the uncached memory address in RAM
 */
#define UncachedULongAddr(_addr) ((unsigned long *)(((unsigned long)(_addr))|0x20000000))

/**
 * @brief Return the cached memory address for a given address
 *
 * @param[in] _addr
 *            Address in RAM to convert to a cached address
 * 
 * @return A void pointer to the cached memory address in RAM
 */
#define CachedAddr(_addr) ((void *)(((unsigned long)(_addr))&~0x20000000))

/**
 * @brief Return the physical memory address for a given address
 *
 * @param[in] _addr
 *            Address in RAM to convert to a physical address
 * 
 * @return A void pointer to the physical memory address in RAM
 */
#define PhysicalAddr(_addr) ({ \
    const volatile void *_addrp = (_addr); \
    (((unsigned long)(_addrp))&~0xE0000000); \
})

/** @brief Symbol at the start of code (start of ROM contents after header) */
extern char __libdragon_text_start[];

/** @brief Symbol at the end of code, data, and sdata (set by the linker) */
extern char __rom_end[];

/** @brief Symbol at the end of code, data, sdata, and bss (set by the linker) */
extern char __bss_end[];

/**
 * @brief Void pointer to the start of heap memory
 */
#define HEAP_START_ADDR ((void*)__bss_end)

/**
 * @brief Memory barrier to ensure in-order execution
 *
 * Since GCC seems to reorder volatile at -O2, a memory barrier is required
 * to ensure that DMA setup is done in the correct order.  Otherwise, the
 * library is useless at higher optimization levels.
 */
#define MEMORY_BARRIER() asm volatile ("" : : : "memory")

/**
 * @brief Returns the 32-bit hardware tick counter
 *
 * This macro returns the current value of the hardware tick counter,
 * present in the CPU coprocessor 0. The counter increments at half of the
 * processor clock speed (see #TICKS_PER_SECOND), and overflows every
 * 91.625 seconds.
 * 
 * It is fine to use this hardware counter for measuring small time intervals,
 * as long as #TICKS_DISTANCE or #TICKS_BEFORE are used to compare different
 * counter reads, as those macros correctly handle overflows.
 * 
 * Most users might find more convenient to use #get_ticks(), a similar function
 * that returns a 64-bit counter with the same frequency that never overflows.
 * 
 * @see #TICKS_BEFORE
 * @see #TICKS_DISTANCE
 * @see #get_ticks
 */
#define TICKS_READ() C0_COUNT()

/**
 * @brief Number of updates to the count register per second
 *
 * Every second, this many counts will have passed in the count register
 */
#define TICKS_PER_SECOND (CPU_FREQUENCY/2)

/**
 * @brief Calculate the time passed between two ticks
 *
 * If "from" is before "to", the distance in time is positive,
 * otherwise it is negative.
 */
#define TICKS_DISTANCE(from, to) ((int32_t)((uint32_t)(to) - (uint32_t)(from)))

/** @brief Return how much time has passed since the instant t0. */
#define TICKS_SINCE(t0)          TICKS_DISTANCE(t0, TICKS_READ())

/**
 * @brief Returns true if "t1" is before "t2".
 *
 * This is similar to t1 < t2, but it correctly handles timer overflows
 * which are very frequent. Notice that the hardware counter overflows every
 * ~91 seconds, so it's not possible to compare times that are more than
 * ~45 seconds apart.
 * 
 * Use #get_ticks() to get a 64-bit counter that never overflows.
 * 
 * @see #get_ticks
 */
#define TICKS_BEFORE(t1, t2) ({ TICKS_DISTANCE(t1, t2) > 0; })

/**
 * @brief Returns equivalent count ticks for the given milliseconds.
 */
#define TICKS_FROM_MS(val) (((val) * (TICKS_PER_SECOND / 1000)))

/**
 * @brief Returns equivalent count ticks for the given microseconds.
 */
#define TICKS_FROM_US(val) (((val) * (8 * TICKS_PER_SECOND / 1000000) / 8))

/**
 * @brief Returns equivalent count ticks for the given microseconds.
 */
#define TICKS_TO_US(val) (((val) * 8 / (8 * TICKS_PER_SECOND / 1000000)))

/**
 * @brief Returns equivalent count ticks for the given microseconds.
 */
#define TICKS_TO_MS(val) (((val) / (TICKS_PER_SECOND / 1000)))


#ifdef __cplusplus
extern "C" {
#endif

bool sys_bbplayer(void);

int sys_get_boot_cic();
void sys_set_boot_cic(int bc);
/**
 * @brief Read the number of ticks since system startup
 *
 * The frequency of this counter is #TICKS_PER_SECOND. The counter will
 * never overflow, being a 64-bit number.
 *
 * @return The number of ticks since system startup
 */
uint64_t get_ticks(void);

/**
 * @brief Read the number of microseconds since system startup
 *
 * This is similar to #get_ticks, but converts the result in integer
 * microseconds for convenience.
 * 
 * @return The number of microseconds since system startup
 */
uint64_t get_ticks_us(void);

/**
 * @brief Read the number of millisecounds since system startup
 * 
 * This is similar to #get_ticks, but converts the result in integer
 * milliseconds for convenience.
 * 
 * @return The number of millisecounds since system startup
 */
uint64_t get_ticks_ms(void);

/**
 * @brief Spin wait until the number of ticks have elapsed
 *
 * @param[in] wait
 *            Number of ticks to wait
 *            Maximum accepted value is 0xFFFFFFFF ticks
 */
void wait_ticks( unsigned long wait );

/**
 * @brief Spin wait until the number of milliseconds have elapsed
 *
 * @param[in] wait_ms
 *            Number of milliseconds to wait
 *            Maximum accepted value is 91625 ms
 */
void wait_ms( unsigned long wait_ms );

/**
 * @brief Force a data cache invalidate over a memory region
 *
 * Use this to force the N64 to update cache from RDRAM.
 *
 * The cache is made by cachelines of 16 bytes. If a memory region is invalidated
 * and the memory region is not fully aligned to cachelines, a larger area
 * than that requested will be invalidated; depending on the arrangement of
 * the data segments and/or heap, this might make data previously
 * written by the CPU in regular memory locations to be unexpectedly discarded,
 * causing bugs.
 *
 * For this reason, this function must only be called with an address aligned
 * to 16 bytes, and with a length which is an exact multiple of 16 bytes; it
 * will assert otherwise.
 *
 * As an alternative, consider using #data_cache_hit_writeback_invalidate,
 * that first writebacks the affected cachelines to RDRAM, guaranteeing integrity
 * of memory areas that share cachelines with the region that must be invalidated.
 *
 * @param[in] addr_
 *            Pointer to memory in question
 * @param[in] sz_
 *            Length in bytes of the data pointed at by addr
 */
#define data_cache_hit_invalidate(addr_, sz_) ({ \
	void *addr = (addr_); unsigned long sz = (sz_); \
	assert(((uint32_t)addr % 16) == 0 && (sz % 16) == 0); \
	__data_cache_hit_invalidate(addr, sz); \
})

void __data_cache_hit_invalidate(volatile void * addr, unsigned long length);
void data_cache_hit_writeback(volatile const void *, unsigned long);
void data_cache_hit_writeback_invalidate(volatile void *, unsigned long);
void data_cache_index_writeback_invalidate(volatile void *, unsigned long);
void data_cache_writeback_invalidate_all(void);
void inst_cache_hit_writeback(volatile const void *, unsigned long);
void inst_cache_hit_invalidate(volatile void *, unsigned long);
void inst_cache_index_invalidate(volatile void *, unsigned long);
void inst_cache_invalidate_all(void);

int get_memory_size();
bool is_memory_expanded();
void *malloc_uncached(size_t size);
void *malloc_uncached_aligned(int align, size_t size);
void free_uncached(void *buf);

/** @brief Type of TV video output */
typedef enum {
    TV_PAL = 0,      ///< Video output is PAL
    TV_NTSC = 1,     ///< Video output is NTSC
    TV_MPAL = 2      ///< Video output is M-PAL
} tv_type_t;

tv_type_t get_tv_type();


/** @cond */
/* Deprecated version of get_ticks */
__attribute__((deprecated("use get_ticks instead")))
static inline volatile unsigned long read_count(void) {
    return get_ticks();
}
/** @endcond */

#ifdef __cplusplus
}
#endif

/** @} */

#endif
