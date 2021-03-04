#include <stdlib.h>
#include <sys/sysinfo.h>

#include "cpu_x86.h"

int main(int argc, char const *argv[]) {
  int num_threads;
  if (argc != 2) {
    num_threads = get_nprocs();
  } else {
    num_threads = atoi(argv[1]);
  }

  benchmark(num_threads);
  return 0;
}
