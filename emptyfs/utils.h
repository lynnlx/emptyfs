/*
 * Created 181110  lynnl
 *
 * Help functions used in kernel extension
 */

#ifndef __EMPTYFS_UTILS_H
#define __EMPTYFS_UTILS_H

#include <sys/types.h>
#include <mach/vm_types.h>
#include <sys/malloc.h>
#include <kern/debug.h>

#ifndef __kext_makefile__
#define KEXTNAME_S "emptyfs"
#endif

/*
 * Used to indicate unused function parameters
 * see: <sys/cdefs.h>#__unused
 */
#define UNUSED(arg0, ...)   (void) ((void) arg0, ##__VA_ARGS__)

#define ARRAY_SIZE(x)       (sizeof(x) / sizeof(*(x)))

/*
 * literal-aware alias of extern modifier
 */
#define readonly_extern     extern

/*
 * G for GCC-specific
 * see:
 *  <sys/param.h>#MIN, MAX
 *  <libkern/libkern.h>#min, max
 */
#define GMIN(a, b) ({       \
    typeof(a) _a = (a);     \
    typeof(b) _b = (b);     \
    _a < _b ? _a : _b;      \
})

#define GMAX(a, b) ({       \
    typeof(a) _a = (a);     \
    typeof(b) _b = (b);     \
    _a > _b ? _a : _b;      \
})

#define panicf(fmt, ...)                \
    panic("\n" fmt "\n%s@%s#L%d\n\n",   \
            ##__VA_ARGS__, __BASE_FILE__, __FUNCTION__, __LINE__)

#ifdef DEBUG
/*
 * NOTE: Do NOT use any multi-nary conditional/logical operator inside assertion
 *       like operators && || ?:  it's extremely EVIL
 *       Separate them  each statement per line
 */
#define kassert(ex) (ex) ? (void) 0 : panicf("Assert `%s' failed", #ex)
/**
 * @ex      the expression
 * @fmt     panic message format
 *
 * Example: kassertf(sz > 0, "Why size %zd nonpositive?", sz);
 */
#define kassertf(ex, fmt, ...) \
    (ex) ? (void) 0 : panicf("Assert `%s' failed: " fmt, #ex, ##__VA_ARGS__)
#else
#define kassert(ex)             (void) ((void) (ex))
#define kassertf(ex, fmt, ...)  (void) ((void) (ex), ##__VA_ARGS__)
#endif

#define kassert_nonnull(ptr) kassert(((void *) ptr) != NULL)

/**
 * Branch predictions
 * see: linux/include/linux/compiler.h
 */
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

/**
 * os_log() is only available on macOS 10.12 or newer
 *  thus os_log do have compatibility issue  use printf instead
 *
 * XNU kernel version of printf() don't recognize some rarely used specifiers
 *  like h, i, j, t  use unrecognized spcifier may raise kernel panic
 *
 * Feel free to print NULL as %s  it checked explicitly by kernel-printf
 *
 * see: xnu/osfmk/kern/printf.c#printf
 */
#define LOG(fmt, ...)        printf(KEXTNAME_S ": " fmt "\n", ##__VA_ARGS__)

#define LOG_INF(fmt, ...)    LOG("[INF] " fmt, ##__VA_ARGS__)
#define LOG_WAR(fmt, ...)    LOG("[WAR] " fmt, ##__VA_ARGS__)
#define LOG_ERR(fmt, ...)    LOG("[ERR] " fmt, ##__VA_ARGS__)
#define LOG_BUG(fmt, ...)    LOG("[BUG] " fmt, ##__VA_ARGS__)
#define LOG_OFF(fmt, ...)    (void) ((void) 0, ##__VA_ARGS__)
#ifdef DEBUG
#define LOG_DBG(fmt, ...)    LOG("[DBG] " fmt, ##__VA_ARGS__)
#else
#define LOG_DBG(fmt, ...)    LOG_OFF(fmt, ##__VA_ARGS__)
#endif

void *util_malloc(size_t, int);
void *util_realloc(void *, size_t, size_t, int);
void util_mfree(void *);
void util_massert(void);

int util_get_kcb(void);
int util_put_kcb(void);
int util_read_kcb(void);

char *util_vma_uuid(const vm_address_t);

void format_uuid_string(const uuid_t, uuid_string_t);

/**
 * kern_os_* family provides zero-out memory allocation
 * see: xnu/libkern/c++/OSRuntime.cpp
 */
extern void *kern_os_malloc(size_t);
extern void kern_os_free(void *);
extern void *kern_os_realloc(void *, size_t);

#endif /* __EMPTYFS_UTILS_H */

