//  Copyright (c) 2017 Abhimanyu Rawat
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#if defined(HPX_HAVE_STACKOVERFLOW_DETECTION)

// for Visual C++ compiler(NT platform)
#ifndef _MSC_VER
#include <config.h>
#endif

#include <sigsegv.h>
#include <stdint.h> 
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/mman.h>

# define sigemptyset(set)
# define sigprocmask(how,set,oldset)

#include <stddef.h> 
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>

#if HAVE_SETRLIMIT
#include <sys/time.h>
#include <sys/resource.h>
#endif

// stack size value if user has not provided one
#ifndef SIGSTKSZ
#define SIGSTKSZ 16384
#endif

// over allocate the stack to detect the address range
#define MYSTACK_CRUMPLE_ZONE 8192

#pragma startup call
jmp_buf mainloop;
sigset_t mainsigset;

char mystack_storage[SIGSTKSZ + 2 * MYSTACK_CRUMPLE_ZONE + 31];
char *mystack; /* SIGSTKSZ bytes in the middle of storage. */

void
prepare_alternate_stack (void)
{
  memset (mystack_storage, 's', sizeof mystack_storage);
  mystack = (char *) ((uintptr_t) (mystack_storage + MYSTACK_CRUMPLE_ZONE) | 31);
}

// to count the number of stack overflows occured in the implementation
volatile int pass = 0;

static void
stackoverflow_handler_continuation (void *arg1, void *arg2, void *arg3)
{
  int arg = (int) (long) arg1;
  longjmp (mainloop, arg);
}

// handler for stack overflow
static void
stackoverflow_handler (int emergency, stackoverflow_context_t scp)
{
  pass++;
  
  // prints the starting address of the stack
  printf ("Stack overflow caught at %p.\n", mystack);

  sigprocmask (SIG_SETMASK, &mainsigset, NULL);

  // jump to the main and continue with the rest of the program if possible
  sigsegv_leave_handler (stackoverflow_handler_continuation,
                         (void *) (long) (emergency ? -1 : pass), NULL, NULL);
}

// handler for general segmentation fault 
static int
sigsegv_handler (void *address, int emergency)
{

  if (!emergency)
    return 0;

  pass++;

  printf ("Segmentation Fault occured.\n");
  sigprocmask (SIG_SETMASK, &mainsigset, NULL);
  return sigsegv_leave_handler (stackoverflow_handler_continuation,
                                (void *) (long) pass, NULL, NULL);
}

// stack overflow detection API starts from here
static void stack_overflow_detection()
{
  // alternate stack to handle the stack overflow
  prepare_alternate_stack ();
  
  // first check if it's a stackoverflow, if not then segmentation fault handler
  if (stackoverflow_install_handler (&stackoverflow_handler,
                                     mystack, SIGSTKSZ)
      < 0)
    { exit (2);}
  
  if (sigsegv_install_handler (&sigsegv_handler) < 0)
    {exit (2);}
}

#endif  // HPX_HAVE_STACKOVERFLOW_DETECTION code ends here
