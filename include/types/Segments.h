#pragma once

#define	SEL_RPL_MASK	3	/* requester priv level */
#define	ISPL(s)	((s)&3)		/* what is the priority level of a selector */
#define	SEL_KPL	0		/* kernel priority level */
#define	SEL_UPL	3		/* user priority level */
#define	ISLDT(s)	((s)&SEL_LDT)	/* is it local or global */
#define	SEL_LDT	4		/* local descriptor table */
#define	IDXSEL(s)	(((s)>>3) & 0x1fff)		/* index of selector */
#define	LSEL(s,r)	(((s)<<3) | SEL_LDT | r)	/* a local selector */
#define	GSEL(s,r)	(((s)<<3) | r)			/* a global selector */

/*
 * User segment descriptors (%cs, %ds etc for compatability apps. 64 bit wide)
 * For long-mode apps, %cs only has the conforming bit in sd_type, the sd_dpl,
 * sd_p, sd_l and sd_def32 which must be zero).  %ds only has sd_p.
 */
struct	user_segment_descriptor {
	uint64_t sd_lolimit:16;	/* segment extent (lsb) */
	uint64_t sd_lobase:24;		/* segment base address (lsb) */
	uint64_t sd_type:5;		/* segment type */
	uint64_t sd_dpl:2;		/* segment descriptor priority level */
	uint64_t sd_p:1;		/* segment descriptor present */
	uint64_t sd_hilimit:4;		/* segment extent (msb) */
	uint64_t sd_xx:1;		/* unused */
	uint64_t sd_long:1;		/* long mode (cs only) */
	uint64_t sd_def32:1;		/* default 32 vs 16 bit size */
	uint64_t sd_gran:1;		/* limit granularity (byte/page units)*/
	uint64_t sd_hibase:8;		/* segment base address  (msb) */
};

#define	USD_GETBASE(sd)		(((sd)->sd_lobase) | (sd)->sd_hibase << 24) 
#define	USD_SETBASE(sd, b)	(sd)->sd_lobase = (b); 	\
				(sd)->sd_hibase = ((b) >> 24);
#define	USD_GETLIMIT(sd)	(((sd)->sd_lolimit) | (sd)->sd_hilimit << 16)
#define	USD_SETLIMIT(sd, l)	(sd)->sd_lolimit = (l);	\
				(sd)->sd_hilimit = ((l) >> 16);

/*
 * System segment descriptors (128 bit wide)
 */
struct	system_segment_descriptor {
	uint64_t sd_lolimit:16;	/* segment extent (lsb) */
	uint64_t sd_lobase:24;		/* segment base address (lsb) */
	uint64_t sd_type:5;		/* segment type */
	uint64_t sd_dpl:2;		/* segment descriptor priority level */
	uint64_t sd_p:1;		/* segment descriptor present */
	uint64_t sd_hilimit:4;		/* segment extent (msb) */
	uint64_t sd_xx0:3;		/* unused */
	uint64_t sd_gran:1;		/* limit granularity (byte/page units)*/
	uint64_t sd_hibase:40;/* segment base address  (msb) */
	uint64_t sd_xx1:8;
	uint64_t sd_mbz:5;		/* MUST be zero */
	uint64_t sd_xx2:19;
};

/*
 * Gate descriptors (e.g. indirect descriptors, trap, interrupt etc. 128 bit)
 * Only interrupt and trap gates have gd_ist.
 */
struct	gate_descriptor {
	uint64_t gd_looffset:16;	/* gate offset (lsb) */
	uint64_t gd_selector:16;	/* gate segment selector */
	uint64_t gd_ist:3;		/* IST table index */
	uint64_t gd_xx:5;		/* unused */
	uint64_t gd_type:5;		/* segment type */
	uint64_t gd_dpl:2;		/* segment descriptor priority level */
	uint64_t gd_p:1;		/* segment descriptor present */
	uint64_t gd_hioffset:48;	/* gate offset (msb) */
	uint64_t sd_xx1:32;
};

/*
 * Generic descriptor
 */
union	descriptor	{
	struct	user_segment_descriptor sd;
	struct	gate_descriptor gd;
};

	/* system segments and gate types */
#define	SDT_SYSNULL	 0	/* system null */
#define	SDT_SYS286TSS	 1	/* system 286 TSS available */
#define	SDT_SYSLDT	 2	/* system 64 bit local descriptor table */
#define	SDT_SYS286BSY	 3	/* system 286 TSS busy */
#define	SDT_SYS286CGT	 4	/* system 286 call gate */
#define	SDT_SYSTASKGT	 5	/* system task gate */
#define	SDT_SYS286IGT	 6	/* system 286 interrupt gate */
#define	SDT_SYS286TGT	 7	/* system 286 trap gate */
#define	SDT_SYSNULL2	 8	/* system null again */
#define	SDT_SYSTSS	 9	/* system available 64 bit TSS */
#define	SDT_SYSNULL3	10	/* system null again */
#define	SDT_SYSBSY	11	/* system busy 64 bit TSS */
#define	SDT_SYSCGT	12	/* system 64 bit call gate */
#define	SDT_SYSNULL4	13	/* system null again */
#define	SDT_SYSIGT	14	/* system 64 bit interrupt gate */
#define	SDT_SYSTGT	15	/* system 64 bit trap gate */

	/* memory segment types */
#define	SDT_MEMRO	16	/* memory read only */
#define	SDT_MEMROA	17	/* memory read only accessed */
#define	SDT_MEMRW	18	/* memory read write */
#define	SDT_MEMRWA	19	/* memory read write accessed */
#define	SDT_MEMROD	20	/* memory read only expand dwn limit */
#define	SDT_MEMRODA	21	/* memory read only expand dwn limit accessed */
#define	SDT_MEMRWD	22	/* memory read write expand dwn limit */
#define	SDT_MEMRWDA	23	/* memory read write expand dwn limit accessed */
#define	SDT_MEME	24	/* memory execute only */
#define	SDT_MEMEA	25	/* memory execute only accessed */
#define	SDT_MEMER	26	/* memory execute read */
#define	SDT_MEMERA	27	/* memory execute read accessed */
#define	SDT_MEMEC	28	/* memory execute only conforming */
#define	SDT_MEMEAC	29	/* memory execute only accessed conforming */
#define	SDT_MEMERC	30	/* memory execute read conforming */
#define	SDT_MEMERAC	31	/* memory execute read accessed conforming */

/*
 * Software definitions are in this convenient format,
 * which are translated into inconvenient segment descriptors
 * when needed to be used by the 386 hardware
 */

struct	soft_segment_descriptor {
	unsigned long ssd_base;		/* segment base address  */
	unsigned long ssd_limit;	/* segment extent */
	unsigned long ssd_type:5;	/* segment type */
	unsigned long ssd_dpl:2;	/* segment descriptor priority level */
	unsigned long ssd_p:1;		/* segment descriptor present */
	unsigned long ssd_long:1;	/* long mode (for %cs) */
	unsigned long ssd_def32:1;	/* default 32 vs 16 bit size */
	unsigned long ssd_gran:1;	/* limit granularity (byte/page units)*/
};

/*
 * region descriptors, used to load gdt/idt tables before segments yet exist.
 */
struct region_descriptor {
	unsigned long rd_limit:16;		/* segment extent */
	unsigned long rd_base:64;	/* base address  */
};

/*
 * Size of IDT table
 */
#define	NIDT	256		/* 32 reserved, 16 h/w, 0 s/w, linux's 0x80 */
#define	NRSVIDT	32		/* reserved entries for cpu exceptions */

/*
 * Entries in the Interrupt Descriptor Table (IDT)
 */
#define	IDT_DE		0	/* #DE: Divide Error */
#define	IDT_DB		1	/* #DB: Debug */
#define	IDT_NMI		2	/* Nonmaskable External Interrupt */
#define	IDT_BP		3	/* #BP: Breakpoint */
#define	IDT_OF		4	/* #OF: Overflow */
#define	IDT_BR		5	/* #BR: Bound Range Exceeded */
#define	IDT_UD		6	/* #UD: Undefined/Invalid Opcode */
#define	IDT_NM		7	/* #NM: No Math Coprocessor */
#define	IDT_DF		8	/* #DF: Double Fault */
#define	IDT_FPUGP	9	/* Coprocessor Segment Overrun */
#define	IDT_TS		10	/* #TS: Invalid TSS */
#define	IDT_NP		11	/* #NP: Segment Not Present */
#define	IDT_SS		12	/* #SS: Stack Segment Fault */
#define	IDT_GP		13	/* #GP: General Protection Fault */
#define	IDT_PF		14	/* #PF: Page Fault */
#define	IDT_MF		16	/* #MF: FPU Floating-Point Error */
#define	IDT_AC		17	/* #AC: Alignment Check */
#define	IDT_MC		18	/* #MC: Machine Check */
#define	IDT_XF		19	/* #XF: SIMD Floating-Point Exception */
#define	IDT_IO_INTS	NRSVIDT	/* Base of IDT entries for I/O interrupts. */
#define	IDT_SYSCALL	0x80	/* System Call Interrupt Vector */
#define	IDT_DTRACE_RET	0x20	/* DTrace pid provider Interrupt Vector */

/*
 * Entries in the Global Descriptor Table (GDT)
 */
#define	GNULL_SEL	0	/* Null Descriptor */
#define	GNULL2_SEL	1	/* Null Descriptor */
#define	GUFS32_SEL	2	/* User 32 bit %fs Descriptor */
#define	GUGS32_SEL	3	/* User 32 bit %gs Descriptor */
#define	GCODE_SEL	4	/* Kernel Code Descriptor */
#define	GDATA_SEL	5	/* Kernel Data Descriptor */
#define	GUCODE32_SEL	6	/* User 32 bit code Descriptor */
#define	GUDATA_SEL	7	/* User 32/64 bit Data Descriptor */
#define	GUCODE_SEL	8	/* User 64 bit Code Descriptor */
#define	GPROC0_SEL	9	/* TSS for entering kernel etc */
/* slot 10 is second half of GPROC0_SEL */
#define	GUSERLDT_SEL	11	/* LDT */
/* slot 11 is second half of GUSERLDT_SEL */
#define	NGDT 		13