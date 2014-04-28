#include <stdio.h>
#include <malloc.h>
#include <string.h>

int global_array[100] = {-1};

void
unintialized_use()
{
  // This test provides an example of using uninitialised memory
  int i;
  printf("%d\n", i);               // Error, i hasn't been initialized

  int * num = (int*)malloc(sizeof(int));
  printf("*num = %d \r\n", *num);
  free(num);
}

void
rw_after_free()
{
  // This test provides an example of reading/writing memory after it
  // has been free'd
  int * i = (int *)malloc(sizeof(int));
  free(i);
  *i = 4;                          // Error, i was already freed 
}

void
rw_malloc_overrun()
{
  // This test provides an example of reading/writing off the end of
  // malloc'd blocks
  int * i = (int*)malloc(sizeof(int)*10);
  i[10] = 13;                      // Error, wrote past the end of the block
  printf("i[-1] = %d \r\n", i[-1]);
  free(i);
}

void
rw_bad_stk_location()
{
  // This test provides an example of reading/writing inappropriate
  // areas on the stack.  Note that valgrind only catches errors below
  // the stack (so in this example, we have to pass a negative index
  // to ptr or valgrind won't catch the problem)
  int i;
  int * ptr = &i;
  ptr[-8] = 7;                     // Error, writing to a bad location on stack
  i = ptr[-15];                    // Error, reading from a bad stack location
}

void
mem_leak()
{
  // This test provides an example of memory leaks -- where pointers
  // to malloc'd blocks are not freed
  int    * i = (int *)malloc(sizeof(int));
  double * j = (double *)malloc(sizeof(double));
  i = NULL;
  // Note that neither i or j were freed here, although j being static means
  // that it will be considered still reachable instead of definitely lost
}

/* Not relevant test for C lang */
void
test_6()
{
  // This test provides an example of mismatched use of
  // malloc/new/new [] vs free/delete/delete []
  int *i = (int *)malloc(sizeof(int));
  free(i);                         // Error, new/free mismatch
  double *j = (double *)malloc(50*sizeof(double));
  free(j);                        // Error, new[],delete mismatch 
}

void
memcpy_overlapping_src_dst()
{
  // This test provides an example of overlapping src and dst
  // pointers in memcpy() and related functions
  char big_buf[1000];
  char * ptr_1 = &big_buf[0];
  char * ptr_2 = &big_buf[400];
  memcpy(ptr_1, ptr_2, 500);       // Error, dst region overlaps src region
}

void
double_free()
{
  // This test provides an example of doubly freed memory
  int * i = (int *)sizeof(int);
  free(i);
  free(i);                        // Error, i delete'd twice
}

void
systemcall_unaddressable_bytes()
{
  // This test provides an example of passing unaddressable bytes to a
  // system call.  Note that the file descriptors for standard input
  // (stdin) and standard output (stdout) are 0 and 1 respectively,
  // which is used in the read(2) and write(2) system calls (see the
  // respective man pages for more information).
  char * buf = (char *)sizeof(50*sizeof(char));
  printf("Please type a bunch of characters and hit enter.\n");
  read(0, buf, 1000);              // Error, read data overflows buffer
  write(1, buf, 1000);             // Error, data comes from past end of buffer
  //free(buf);
}

int stk_overrun()
{
  int stack_array[100];
  stack_array[1] = 0;
  return stack_array[200];
}

int glob_buff_overrun()
{
  return global_array[200];
}

int
main(int argc, char**argv)
{
  if (argc<2) {
    printf("Syntax:");
    printf("  %s <test-number> <test-number> <test-number> . . .\r\n", argv[0]);
    return -1;
  }

  if (mallopt(M_CHECK_ACTION, 7) != 1) {
    fprintf(stderr, "mallopt() failed");
    return(-1);
  }
  
  int ctr;
  for( ctr=1; ctr < argc; ctr++ )
  {
    int test_number = atoi(argv[ctr]);

    switch (test_number) {
      case 1: unintialized_use(); break;
      case 2: rw_after_free(); break;
      case 3: rw_malloc_overrun(); break;
      case 4: rw_bad_stk_location(); break;
      case 5: mem_leak(); break;
      case 6: test_6(); break;
      case 7: memcpy_overlapping_src_dst(); break;
      case 8: double_free(); break;
      case 9: systemcall_unaddressable_bytes(); break;
      case 10: stk_overrun(); break;
      case 11: glob_buff_overrun(); break;
      default: printf("No test or invalid test specified (only 1--9 are valid).");
               return -1;
    }
  }

  return 0;
}
