#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#define PAGE_SIZE 0x4000
#define PAGE_MASK (PAGE_SIZE-1)
#define round_page(x) ((((unsigned long)(x)) + PAGE_MASK) & ~(PAGE_MASK))

#undef NULL
#define NULL 0

#define M_NOWAIT        0x0001      /* do not block */
#define M_WAITOK        0x0002      /* ok to block */
#define M_ZERO          0x0100      /* bzero the allocation */
#define M_NOVM          0x0200      /* don't ask VM for pages */
#define M_EXEC          0x4000      /* allocate executable space */
#define M_USE_RESERVE   0x0400      /* can alloc out of reserve memory */
#define M_NODUMP        0x0800  
#define M_MAGIC         877983977   /* time when first defined :-) */

/*
 * Basic types upon which most other types are built.
 */
typedef int8_t          __int8_t;
typedef uint8_t         __uint8_t;
typedef int16_t         __int16_t;
typedef uint16_t        __uint16_t;
typedef int32_t         __int32_t;
typedef uint32_t        __uint32_t;

/*
 * Standard type definitions.
 */
typedef int64_t         __critical_t;
typedef double          __double_t;
typedef float           __float_t;
typedef int64_t         __intfptr_t;
typedef int64_t         __intmax_t;
typedef int64_t         __intptr_t;
typedef int32_t         __int_fast8_t;
typedef int32_t         __int_fast16_t;
typedef int32_t         __int_fast32_t;
typedef int64_t         __int_fast64_t;
typedef int8_t          __int_least8_t;
typedef int16_t         __int_least16_t;
typedef int32_t         __int_least32_t;
typedef int64_t         __int_least64_t;
typedef int64_t         __ptrdiff_t;        /* ptr1 - ptr2 */
typedef int64_t         __register_t;
typedef int64_t         __segsz_t;          /* segment size (in pages) */
typedef int64_t         __time_t;           /* time()... */
typedef uint64_t        __uintfptr_t;
typedef uint64_t        __uintmax_t;
typedef uint64_t        __uintptr_t;
typedef uint32_t        __uint_fast8_t;
typedef uint32_t        __uint_fast16_t;
typedef uint32_t        __uint_fast32_t;
typedef uint64_t        __uint_fast64_t;
typedef uint8_t         __uint_least8_t;
typedef uint16_t        __uint_least16_t;
typedef uint32_t        __uint_least32_t;
typedef uint64_t        __uint_least64_t;
typedef uint64_t        __u_register_t;
typedef uint64_t        __vm_offset_t;
typedef int64_t         __vm_ooffset_t;
typedef uint64_t        __vm_paddr_t;
typedef uint64_t        __vm_pindex_t;
typedef uint64_t        __vm_size_t;
typedef int64_t         __blkcnt_t;         /* file block count */
typedef int32_t         __clockid_t;        /* clock_gettime()... */
typedef uint64_t        __cap_rights_t;     /* capability rights */
typedef uint32_t        __fflags_t;         /* file flags */
typedef uint64_t        __fsblkcnt_t;
typedef uint64_t        __fsfilcnt_t;
typedef int32_t         __lwpid_t;          /* Thread ID (a.k.a. LWP) */
typedef int             __accmode_t;        /* access permissions */
typedef int             __nl_item;
typedef int64_t         __off_t;            /* file offset */
typedef int32_t         __pid_t;            /* process [group] */
/* signed, because of legacy code */
/* that uses -1 for RLIM_INFINITY */
typedef uint8_t         __sa_family_t;
typedef uint32_t        __socklen_t;
typedef long            __suseconds_t;      /* microseconds (signed) */
typedef uint32_t        __uid_t;
typedef unsigned int    __useconds_t;       /* microseconds (unsigned) */
typedef int             __cpuwhich_t;       /* which parameter for cpuset. */
typedef int             __cpulevel_t;       /* level parameter for cpuset. */
typedef int             __cpusetid_t;       /* cpuset identifier. */
typedef int             __ct_rune_t;        /* arg type for ctype funcs */
typedef __ct_rune_t     __rune_t;           /* rune_t (see above) */
typedef __ct_rune_t     __wchar_t;          /* wchar_t (see above) */
typedef __ct_rune_t     __wint_t;           /* wint_t (see above) */
typedef uint32_t        __fixpt_t;          /* fixed point number */

typedef long            __ssize_t;
typedef __ssize_t       ssize_t;
typedef int64_t         off_t;              /* file offset */
typedef unsigned long   size_t;
typedef char*           caddr_t;            /* core address */
typedef uint16_t        au_event_t;
typedef uint32_t        gid_t;
typedef uint32_t        uid_t;
typedef uint16_t        mode_t;
typedef int64_t         time_t;
typedef long            suseconds_t;
typedef __register_t    register_t;

struct iovec {
    void*   iov_base;  // Pointer to the data
    size_t  iov_len;   // Length of the data
};

struct fpu_kern_ctx {
    // Omitted
};

struct timespec {
    time_t  tv_sec;
    long    tv_nsec;
};

struct timeval {
    time_t      tv_sec;
    suseconds_t tv_usec;
};