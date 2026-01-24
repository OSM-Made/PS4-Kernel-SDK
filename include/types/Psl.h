#pragma once

/*
 * 386 processor status longword.
 */
#define	PSL_C		0x00000001	/* carry bit */
#define	PSL_PF		0x00000004	/* parity bit */
#define	PSL_AF		0x00000010	/* bcd carry bit */
#define	PSL_Z		0x00000040	/* zero bit */
#define	PSL_N		0x00000080	/* negative bit */
#define	PSL_T		0x00000100	/* trace enable bit */
#define	PSL_I		0x00000200	/* interrupt enable bit */
#define	PSL_D		0x00000400	/* string instruction direction bit */
#define	PSL_V		0x00000800	/* overflow bit */
#define	PSL_IOPL	0x00003000	/* i/o privilege level */
#define	PSL_NT		0x00004000	/* nested task bit */
#define	PSL_RF		0x00010000	/* resume flag bit */
/* #define PSL_VM	0x00020000 */	/* virtual 8086 mode bit */
#define	PSL_AC		0x00040000	/* alignment checking */
/* #define PSL_VIF	0x00080000 */	/* virtual interrupt enable */
/* #define PSL_VIP	0x00100000 */	/* virtual interrupt pending */
#define	PSL_ID		0x00200000	/* identification bit */

/*
 * The i486 manual says that we are not supposed to change reserved flags,
 * but this is too much trouble since the reserved flags depend on the cpu
 * and setting them to their historical values works in practice.
 */
#define	PSL_RESERVED_DEFAULT	0x00000002

/*
 * Initial flags for kernel and user mode.  The kernel later inherits
 * PSL_I and some other flags from user mode.
 */
#define	PSL_KERNEL	PSL_RESERVED_DEFAULT
#define	PSL_USER	(PSL_RESERVED_DEFAULT | PSL_I)

/*
 * Bits that can be changed in user mode on 486's.  We allow these bits
 * to be changed using ptrace(), sigreturn() and procfs.  Setting PS_NT
 * is undesirable but it may as well be allowed since users can inflict
 * it on the kernel directly.  Changes to PSL_AC are silently ignored on
 * 386's.
 */
#define	PSL_USERCHANGE (PSL_C | PSL_PF | PSL_AF | PSL_Z | PSL_N | PSL_T \
			| PSL_D | PSL_V | PSL_NT | PSL_AC | PSL_ID)
