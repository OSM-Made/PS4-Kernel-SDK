#pragma once

struct au_tid_addr {
    uint32_t at_port;
    uint32_t at_type;
    uint32_t at_addr[4];
};
typedef	struct au_tid_addr	au_tid_addr_t;

struct au_mask {
    unsigned int am_success;
    unsigned int am_failure;
};
typedef	struct au_mask	au_mask_t;

struct auditinfo_addr {
    uint32_t ai_auid;
    au_mask_t ai_mask;
    au_tid_addr_t ai_termid;
    int	ai_asid;
    uint64_t ai_flags;
};

/**
* struct ucred: Holds authentication and security information for a process/thread.
*/
struct ucred {
    uint32_t cr_ref;
    uint32_t cr_uid;
    uint32_t cr_ruid;
    uint32_t cr_svuid;
    int 	 cr_ngroups;
    uint32_t cr_rgid;
    uint32_t cr_svgid;
    void* cr_uidinfo;
    void* cr_ruidinfo;
    void* cr_prison;       // Pointer to the jail/prison structure
    void* cr_loginclass;
    uint32_t cr_flags;
    void* cr_pspare2[2];
    uint64_t cr_sceAuthID;
    uint64_t cr_sceCaps[0x04];
    uint64_t cr_sceAttr[0x04];
    char 	 cr_unk0A0[0x48];
    struct auditinfo_addr cr_audit; // Assuming auditinfo_addr is defined
    uint32_t* cr_groups;
    int 	 cr_agroups;
};

TYPE_BEGIN(struct mount, 0x1000); // XXX: random, don't use directly without fixing it
TYPE_FIELD(void* mnt_data, 0x288);
TYPE_END();

TYPE_BEGIN(struct vnode, 0x1000); // XXX: random, don't use directly without fixing it
TYPE_FIELD(struct mount* v_mount, 0x20);
TYPE_END();

/**
 * struct filedesc: Contains the file-related state for a process.
 */
struct filedesc
{
    void** fd_ofiles; 	        /* file structures for open files */
    char* fd_ofileflags; 	    /* per-process open file flags */
    struct vnode* fd_cdir; 	 	/* current directory (vnode*) */
    struct vnode* fd_rdir; 	    /* root directory (vnode*) */
    struct vnode* fd_jdir; 	    /* jail root directory (vnode*) */
    int fd_nfiles; 	            /* number of open files allocated */
    unsigned long* fd_map; 	    /* bitmap of free fds */
    int fd_lastfile; 	        /* high-water mark of fd_ofiles */
    int fd_freefile; 	        /* approx. next free file */
    unsigned short fd_cmask; 	/* mask for file creation */
    unsigned short fd_refcnt; 	/* thread reference count */
    unsigned short fd_holdcnt; 	/* hold count on structure + mutex */
};

struct prison
{

};

TYPE_BEGIN(struct proc, 0x1000); // XXX: random, don't use directly without fixing it
TYPE_FIELD(LIST_ENTRY(proc) p_list, 0);     // Link to the global process list
TYPE_FIELD(TAILQ_HEAD(, thread) p_threads, 0x10); // List of threads in this process
TYPE_FIELD(struct ucred* p_ucred, 0x40);  // Pointer to the process credentials
TYPE_FIELD(struct filedesc* p_fd, 0x48); // Pointer to the file descriptor table
TYPE_FIELD(int p_flag, 0xA8);
TYPE_FIELD(int p_pid, 0xB0);            // Process ID
TYPE_FIELD(struct mtx p_mtx, 0xF8);           // Process lock
TYPE_FIELD(struct vmspace* p_vmspace, 0x168); // Pointer to the address space
TYPE_FIELD(uint32_t p_lock, 0x2C0);
TYPE_FIELD(struct dynlib* p_dynlib, 0x340); // Pointer to dynamic library info
TYPE_FIELD(char titleId[10], 0x390);    // Application Title ID
TYPE_FIELD(char contentid[64], 0x3D4); // Application Content ID
TYPE_FIELD(char p_comm[32], 0x454);     // Command name
TYPE_FIELD(char p_elfpath[1024], 0x474); // Full path to the executable ELF file
TYPE_FIELD(int p_randomized_path_len, 0x8A8);
TYPE_FIELD(char p_randomized_path[0x100], 0x8AC);
TYPE_END();

TYPE_BEGIN(struct thread, 0x3D8); // XXX: random, don't use directly without fixing it
TYPE_FIELD(struct mtx* volatile td_lock, 0); // Thread lock pointer
TYPE_FIELD(struct proc* td_proc, 8);      // Pointer to the parent process
TYPE_FIELD(TAILQ_ENTRY(thread) td_plist, 0x10); // Link in the process's thread list
TYPE_FIELD(int tid, 0x88);              // Thread ID
TYPE_FIELD(int td_pinned, 0x12C);       // Pinning state (e.g., to a CPU)
TYPE_FIELD(struct ucred* td_ucred, 0x130); // Pointer to the thread credentials
TYPE_FIELD(char td_name[32], 0x284);     // Thread name (for debugging)
TYPE_FIELD(uint64_t td_retval[2], 0x398); // Return values from a syscall/function
TYPE_FIELD(uint16_t td_priority, 0x380); // Scheduling priority
TYPE_END();

LIST_HEAD(proclist, proc);

/**
 * FOREACH_PROC_IN_SYSTEM: Macro to iterate over all active processes.
 * Assumes 'allproc' is the head of the global process list (type 'proclist').
 * @p: The loop variable (struct proc*).
 */
#define	FOREACH_PROC_IN_SYSTEM(p)					\
	LIST_FOREACH((p), allproc, p_list)

/* These flags are kept in p_flag. */
#define	P_ADVLOCK	0x00001	/* Process may hold a POSIX advisory lock. */
#define	P_CONTROLT	0x00002	/* Has a controlling terminal. */
#define	P_KTHREAD	0x00004	/* Kernel thread (*). */
#define	P_FOLLOWFORK	0x00008	/* Attach parent debugger to children. */
#define	P_PPWAIT	0x00010	/* Parent is waiting for child to exec/exit. */
#define	P_PROFIL	0x00020	/* Has started profiling. */
#define	P_STOPPROF	0x00040	/* Has thread requesting to stop profiling. */
#define	P_HADTHREADS	0x00080	/* Has had threads (no cleanup shortcuts) */
#define	P_SUGID		0x00100	/* Had set id privileges since last exec. */
#define	P_SYSTEM	0x00200	/* System proc: no sigs, stats or swapping. */
#define	P_SINGLE_EXIT	0x00400	/* Threads suspending should exit, not wait. */
#define	P_TRACED	0x00800	/* Debugged process being traced. */
#define	P_WAITED	0x01000	/* Someone is waiting for us. */
#define	P_WEXIT		0x02000	/* Working on exiting. */
#define	P_EXEC		0x04000	/* Process called exec. */
#define	P_WKILLED	0x08000	/* Killed, go to kernel/user boundary ASAP. */
#define	P_CONTINUED	0x10000	/* Proc has continued from a stopped state. */
#define	P_STOPPED_SIG	0x20000	/* Stopped due to SIGSTOP/SIGTSTP. */
#define	P_STOPPED_TRACE	0x40000	/* Stopped because of tracing. */
#define	P_STOPPED_SINGLE 0x80000 /* Only 1 thread can continue (not to user). */
#define	P_PROTECTED	0x100000 /* Do not kill on memory overcommit. */
#define	P_SIGEVENT	0x200000 /* Process pending signals changed. */
#define	P_SINGLE_BOUNDARY 0x400000 /* Threads should suspend at user boundary. */
#define	P_HWPMC		0x800000 /* Process is using HWPMCs */

#define	P_JAILED	0x1000000 /* Process is in jail. */
#define	P_INEXEC	0x4000000 /* Process is in execve(). */
#define	P_STATCHILD	0x8000000 /* Child process stopped or exited. */
#define	P_INMEM		0x10000000 /* Loaded into memory. */
#define	P_SWAPPINGOUT	0x20000000 /* Process is being swapped out. */
#define	P_SWAPPINGIN	0x40000000 /* Process is being swapped in. */

#define	P_STOPPED	(P_STOPPED_SIG|P_STOPPED_SINGLE|P_STOPPED_TRACE)
#define	P_SHOULDSTOP(p)	((p)->p_flag & P_STOPPED)
#define	P_KILLED(p)	((p)->p_flag & P_WKILLED)
