#include <libgen.h>
#include <stdio.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

using std::cout;
using std::string;

#define BIT_TEST(bit_map, pos) (((bit_map) & (0x1 << (pos))) ? 1 : 0)
#define SET_FEAT(feat_mask) \
  { feat |= (feat_mask); }

typedef enum {
  _CPUID_X86_SSE_ = 0x1,
  _CPUID_X86_AVX_ = 0x2,
  _CPUID_X86_FMA_ = 0x4,
  _CPUID_X86_AVX512F_ = 0x8,
  _CPUID_X86_AVX512_VNNI_ = 0x10,
} cpuid_x86_feature_t;

struct cpuid_t {
  unsigned int ieax;
  unsigned int iecx;
  unsigned int eax;
  unsigned int ebx;
  unsigned int ecx;
  unsigned int edx;
};

static unsigned int feat;

static void cpuid_x86_exec(struct cpuid_t *cpuid) {
  asm volatile(
      "pushq %%rbx\n"
      "cpuid\n"
      "movl %%ebx, %1\n"
      "popq %%rbx\n"
      : "=a"(cpuid->eax), "=r"(cpuid->ebx), "=c"(cpuid->ecx), "=d"(cpuid->edx)
      : "a"(cpuid->ieax), "c"(cpuid->iecx)
      : "cc");
}

static void cpuid_x86_init() {
  struct cpuid_t cpuid;

  feat = 0;

  cpuid.ieax = 0x1;
  cpuid.iecx = 0x0;
  cpuid_x86_exec(&cpuid);

  if (BIT_TEST(cpuid.edx, 25)) {
    SET_FEAT(_CPUID_X86_SSE_);
  }
  if (BIT_TEST(cpuid.ecx, 28)) {
    SET_FEAT(_CPUID_X86_AVX_);
  }
  if (BIT_TEST(cpuid.ecx, 12)) {
    SET_FEAT(_CPUID_X86_FMA_);
  }

  cpuid.ieax = 0x7;
  cpuid.iecx = 0x0;
  cpuid_x86_exec(&cpuid);

  if (BIT_TEST(cpuid.ebx, 16)) {
    SET_FEAT(_CPUID_X86_AVX512F_);
  }
  if (BIT_TEST(cpuid.ecx, 11)) {
    SET_FEAT(_CPUID_X86_AVX512_VNNI_);
  }
}

unsigned int cpuid_x86_support(cpuid_x86_feature_t feature) {
  return feat & feature;
}

int main(int argc, char *argv[]) {
  cpuid_x86_init();
  char *pwd = dirname(argv[0]);
  std::string string_pwd(pwd);
  chdir(pwd);

  bool compile_all = false;

  if (argc == 2 && strncmp(argv[1], "--compile-all", 14) == 0) {
    compile_all = true;
  }
  // flags
  string cFlags = getenv("CFLAGS") == NULL ? "" : getenv("CFLAGS");
  string asFlags = getenv("ASFLAGS") == NULL ? "" : getenv("ASFLAGS");

  // assembly files
  string avx512f = string_pwd + "/binary/cpu_kernel_x86_avx512f.asm";
  string avx512_vnni = string_pwd + "/binary/cpu_kernel_x86_avx512_vnni.asm";
  string fma = string_pwd + "/binary/cpu_kernel_x86_fma.asm";
  string avx = string_pwd + "/binary/cpu_kernel_x86_avx.asm";
  string sse = string_pwd + "/binary/cpu_kernel_x86_sse.asm";

  // libs
  string libc = string_pwd + "/lib/*.c";
  string libo = string_pwd + "/*.o";
  string headers = "-lm -I " + string_pwd + "/include";

  string smtl_cmd =
      "gcc " + cFlags + " -pthread -O3 -c " + libc + " " + headers + "\n";
  string asm_cmd = "";
  string c_cmd = "gcc " + cFlags + " -pthread " + headers + " ";
  string lnk_cmd =
      "gcc " + cFlags + " -pthread -lrt " + headers + " " + libo + " -o $1";

  string isa_macro = "";

  if (cpuid_x86_support(_CPUID_X86_AVX512F_) || compile_all) {
    string out = "cpu_kernel_x86_avx512f.o";
    isa_macro += "-D_AVX512F_ ";

    asm_cmd += "as " + asFlags + " -o " + out + " " + avx512f + "\n";
  }
  if (cpuid_x86_support(_CPUID_X86_AVX512_VNNI_) || compile_all) {
    string out = "cpu_kernel_x86_avx512_vnni.o";
    isa_macro += "-D_AVX512_VNNI_ ";

    asm_cmd += "as " + asFlags + " -o " + out + " " + avx512_vnni + "\n";
  }
  if (cpuid_x86_support(_CPUID_X86_FMA_) || compile_all) {
    string out = "cpu_kernel_x86_fma.o";
    isa_macro += "-D_FMA_ ";

    asm_cmd += "as " + asFlags + " -o " + out + " " + fma + "\n";
  }
  if (cpuid_x86_support(_CPUID_X86_AVX_) || compile_all) {
    string out = "cpu_kernel_x86_avx.o";
    isa_macro += "-D_AVX_ ";

    asm_cmd += "as " + asFlags + " -o " + out + " " + avx + "\n";
  }
  if (cpuid_x86_support(_CPUID_X86_SSE_) || compile_all) {
    string out = "cpu_kernel_x86_sse.o";
    isa_macro += "-D_SSE_ ";

    asm_cmd += "as " + asFlags + " -o " + out + " " + sse + "\n";
  }

  c_cmd += (isa_macro + "-c lib/cpu_x86.c\n");
  lnk_cmd += "\n";

  cout << smtl_cmd << asm_cmd << c_cmd << lnk_cmd;

  return 0;
}
