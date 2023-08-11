// Saved registers for kernel context switches.
struct context {
  uint64 ra;
  uint64 sp;

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

// Per-CPU state.
struct cpu {
  struct proc *proc;          // The process running on this cpu, or null.
  struct context context;     // swtch() here to enter scheduler().
  int noff;                   // Depth of push_off() nesting.
  int intena;                 // Were interrupts enabled before push_off()?
};

extern struct cpu cpus[NCPU];

// per-process data for the trap handling code in trampoline.S.
// sits in a page by itself just under the trampoline page in the
// user page table. not specially mapped in the kernel page table.
// the sscratch register points here.
// uservec in trampoline.S saves user registers in the trapframe,
// then initializes registers from the trapframe's
// kernel_sp, kernel_hartid, kernel_satp, and jumps to kernel_trap.
// usertrapret() and userret in trampoline.S set up
// the trapframe's kernel_*, restore user registers from the
// trapframe, switch to the user page table, and enter user space.
// the trapframe includes callee-saved user registers like s0-s11 because the
// return-to-user path via usertrapret() doesn't return through
// the entire kernel call stack.
struct trapframe {
  /*   0 */ uint64 kernel_satp;   // kernel page table
  /*   8 */ uint64 kernel_sp;     // top of process's kernel stack
  /*  16 */ uint64 kernel_trap;   // usertrap()
  /*  24 */ uint64 epc;           // saved user program counter
  /*  32 */ uint64 kernel_hartid; // saved kernel tp
  /*  40 */ uint64 ra;
  /*  48 */ uint64 sp;
  /*  56 */ uint64 gp;
  /*  64 */ uint64 tp;
  /*  72 */ uint64 t0;
  /*  80 */ uint64 t1;
  /*  88 */ uint64 t2;
  /*  96 */ uint64 s0;
  /* 104 */ uint64 s1;
  /* 112 */ uint64 a0;
  /* 120 */ uint64 a1;
  /* 128 */ uint64 a2;
  /* 136 */ uint64 a3;
  /* 144 */ uint64 a4;
  /* 152 */ uint64 a5;
  /* 160 */ uint64 a6;
  /* 168 */ uint64 a7;
  /* 176 */ uint64 s2;
  /* 184 */ uint64 s3;
  /* 192 */ uint64 s4;
  /* 200 */ uint64 s5;
  /* 208 */ uint64 s6;
  /* 216 */ uint64 s7;
  /* 224 */ uint64 s8;
  /* 232 */ uint64 s9;
  /* 240 */ uint64 s10;
  /* 248 */ uint64 s11;
  /* 256 */ uint64 t3;
  /* 264 */ uint64 t4;
  /* 272 */ uint64 t5;
  /* 280 */ uint64 t6;
};

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
// Per-process state
struct proc {
  struct spinlock lock;

  // p->lock must be held when using these:
  enum procstate state;        // Process state
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  int xstate;                  // Exit status to be returned to parent's wait
  int pid;                     // Process ID

  // wait_lock must be held when using this:
  struct proc *parent;         // Parent process

  // these are private to the process, so p->lock need not be held.
  uint64 kstack;               // Virtual address of kernel stack
  uint64 sz;                   // Size of process memory (bytes)
  pagetable_t pagetable;       // User page table
  struct trapframe *trapframe; // data page for trampoline.S
  struct context context;      // swtch() here to run process
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
  /*** for lab ***/
  int alarm_interval;       // every alarm_interval call alarm handler
  void (*alarm_handler)();     // alarm handler
  int ticks;                   // timer ticks
  struct trapframe *saved_trapframe; // save trapframe when exec alarm handler
  int alarm_handling;          // if non-zero, alarm handler is being exec
};

static inline void 
cpytrapframe(struct trapframe* dst, struct trapframe* src) {
  dst->kernel_hartid = src->kernel_hartid;
  dst->kernel_satp = src->kernel_satp;
  dst->kernel_sp = src->kernel_sp;
  dst->kernel_trap = src->kernel_trap;
  dst->a0 = src->a0;
  dst->a1 = src->a1;
  dst->a2= src->a2;
  dst->a3 = src->a3;
  dst->a4 = src->a4;
  dst->a5 = src->a5;
  dst->a6 = src->a6;
  dst->a7 = src->a7;
  dst->epc = src->epc;
  dst->ra = src->ra;
  dst->sp = src->sp;
  dst->gp = src->gp;
  dst->tp = src->tp;
  dst->t0 = src->t0;
  dst->t1 = src->t1;
  dst->t2 = src->t2;
  dst->s0 = src->s0;
  dst->s1 = src->s1;
  dst->s2 = src->s2;
  dst->s3 = src->s3;
  dst->s4 = src->s4;
  dst->s5 = src->s5;
  dst->s6 = src->s6;
  dst->s7 = src->s7;
  dst->s8 = src->s8;
  dst->s9 = src->s9;
  dst->s10 = src->s10;
  dst->s11 = src->s11;
  dst->t3 = src->t3;
  dst->t4 = src->t4;
  dst->t5 = src->t5;
  dst->t6 = src->t6;
}
