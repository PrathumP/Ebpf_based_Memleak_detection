#ifndef __blazesym_h_
#define __blazesym_h_

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Types of symbol sources and debug information for C API.
 */
typedef enum blazesym_src_type {
  /**
   * Symbols and debug information from an ELF file.
   */
  SRC_T_ELF,
  /**
   * Symbols and debug information from a kernel image and its kallsyms.
   */
  SRC_T_KERNEL,
  /**
   * Symbols and debug information from a process, including loaded object files.
   */
  SRC_T_PROCESS,
  /**
   * Must be last for serialization purposes
   */
  Sentinel,
} blazesym_src_type;

/**
 * BlazeSymbolizer provides an interface to symbolize addresses with
 * a list of symbol sources.
 *
 * Users should present BlazeSymbolizer with a list of symbol sources
 * (`SymbolSrcCfg`); for example, an ELF file and its base address
 * (`SymbolSrcCfg::Elf`), or a Linux kernel image and a copy of its
 * kallsyms (`SymbolSrcCfg::Kernel`).  Additionally, BlazeSymbolizer
 * uses information from these sources to symbolize addresses.
 */
typedef struct BlazeSymbolizer BlazeSymbolizer;

/**
 * The symbol resolver for /proc/kallsyms.
 *
 * The users should provide the path of kallsyms, so you can provide
 * a copy from other devices.
 */
typedef struct KSymResolver KSymResolver;

/**
 * A placeholder symbolizer for C API.
 *
 * It is returned by [`blazesym_new()`] and should be free by
 * [`blazesym_free()`].
 */
typedef struct blazesym {
  struct BlazeSymbolizer *symbolizer;
} blazesym;

/**
 * The result of symbolization of an address for C API.
 *
 * A `blazesym_csym` is the information of a symbol found for an
 * address.  One address may result in several symbols.
 */
typedef struct blazesym_csym {
  /**
   * The symbol name is where the given address should belong to.
   */
  const char *symbol;
  /**
   * The address (i.e.,the first byte) is where the symbol is located.
   *
   * The address is already relocated to the address space of
   * the process.
   */
  uint64_t start_address;
  /**
   * The path of the source code defines the symbol.
   */
  const char *path;
  /**
   * The instruction of the address is in the line number of the source code.
   */
  uintptr_t line_no;
  uintptr_t column;
} blazesym_csym;

/**
 * `blazesym_entry` is the output of symbolization for an address for C API.
 *
 * Every address has an `blazesym_entry` in
 * [`blazesym_result::entries`] to collect symbols found by BlazeSym.
 */
typedef struct blazesym_entry {
  /**
   * The number of symbols found for an address.
   */
  uintptr_t size;
  /**
   * All symbols found.
   *
   * `syms` is an array of blazesym_csym in the size `size`.
   */
  const struct blazesym_csym *syms;
} blazesym_entry;

/**
 * `blazesym_result` is the result of symbolization for C API.
 *
 * The instances of blazesym_result are returned from
 * [`blazesym_symbolize()`].  They should be free by calling
 * [`blazesym_result_free()`].
 */
typedef struct blazesym_result {
  uintptr_t size;
  /**
   * The entries for addresses.
   *
   * Symbolization occurs based on the order of addresses.
   * Therefore, every address must have an entry here on the same
   * order.
   */
  struct blazesym_entry entries[0];
} blazesym_result;

/**
 * The paramters to load symbols and dbug information from an ELF.
 *
 * Describes the path and address of an ELF file loaded in a
 * process.
 */
typedef struct ssc_elf {
  /**
   * The file name of an ELF file.
   *
   * It can be an executable or shared object.
   * For example, passing "/bin/sh" will load symbols and debug information from `sh`.
   * Whereas passing "/lib/libc.so.xxx" will load symbols and debug information from the libc.
   */
  const char *file_name;
  /**
   * The base address is where the file's executable segment(s) is loaded.
   *
   * It should be the address
   * in the process mapping to the executable segment's first byte.
   * For example, in /proc/&lt;pid&gt;/maps
   *
   * ```text
   *     7fe1b2dc4000-7fe1b2f80000 r-xp 00000000 00:1d 71695032                   /usr/lib64/libc-2.28.so
   *     7fe1b2f80000-7fe1b3180000 ---p 001bc000 00:1d 71695032                   /usr/lib64/libc-2.28.so
   *     7fe1b3180000-7fe1b3184000 r--p 001bc000 00:1d 71695032                   /usr/lib64/libc-2.28.so
   *     7fe1b3184000-7fe1b3186000 rw-p 001c0000 00:1d 71695032                   /usr/lib64/libc-2.28.so
   * ```
   *
   * It reveals that the executable segment of libc-2.28.so was
   * loaded at 0x7fe1b2dc4000.  This base address is used to
   * translate an address in the segment to the corresponding
   * address in the ELF file.
   *
   * A loader would load an executable segment with the permission of `x`
   * (executable).  For example, the first block is with the
   * permission of `r-xp`.
   */
  uint64_t base_address;
} ssc_elf;

/**
 * The parameters to load symbols and debug information from a kernel.
 *
 * Use a kernel image and a snapshot of its kallsyms as a source of symbols and
 * debug information.
 */
typedef struct ssc_kernel {
  /**
   * The path of a copy of kallsyms.
   *
   * It can be `"/proc/kallsyms"` for the running kernel on the
   * device.  However, you can make copies for later.  In that situation,
   * you should give the path of a copy.
   * Passing a `NULL`, by default, will result in `"/proc/kallsyms"`.
   */
  const char *kallsyms;
  /**
   * The path of a kernel image.
   *
   * The path of a kernel image should be, for instance,
   * `"/boot/vmlinux-xxxx"`.  For a `NULL` value, it will locate the
   * kernel image of the running kernel in `"/boot/"` or
   * `"/usr/lib/debug/boot/"`.
   */
  const char *kernel_image;
} ssc_kernel;

/**
 * The parameters to load symbols and debug information from a process.
 *
 * Load all ELF files in a process as the sources of symbols and debug
 * information.
 */
typedef struct ssc_process {
  /**
   * It is the PID of a process to symbolize.
   *
   * BlazeSym will parse `/proc/<pid>/maps` and load all the object
   * files.
   */
  uint32_t pid;
} ssc_process;

/**
 * Parameters of a symbol source.
 */
typedef union ssc_params {
  /**
   * The variant for SRC_T_ELF
   */
  struct ssc_elf elf;
  /**
   * The variant for SRC_T_KERNEL
   */
  struct ssc_kernel kernel;
  /**
   * The variant for SRC_T_PROCESS
   */
  struct ssc_process process;
} ssc_params;

/**
 * Description of a source of symbols and debug information for C API.
 */
typedef struct sym_src_cfg {
  /**
   * A type of symbol source.
   */
  enum blazesym_src_type src_type;
  union ssc_params params;
} sym_src_cfg;

typedef uint32_t Elf64_Word;

/**
 * Create a KSymResolver
 *
 * # Safety
 *
 * This function is supposed to be used by C code.  The pointer
 * returned should be free with `sym_resolver_free()`.
 *
 */
struct KSymResolver *sym_resolver_create(void);

/**
 * Free a KsymResolver
 *
 * # Safety
 *
 * The pointer passed in should be the one returned by
 * `sym_resolver_create()`.
 *
 */
void sym_resolver_free(struct KSymResolver *aResolverPtr);

/**
 * Find the symbols of a give address if there is.
 *
 * # Safety
 *
 * The returned string is managed by `resolver_ptr`.  Don't try to
 * free it.
 *
 */
const char *sym_resolver_find_addr(struct KSymResolver *aResolverPtr,
                                   uint64_t aAddr);

/**
 * Create an instance of blazesym a symbolizer for C API.
 *
 * # Safety
 *
 * Free the pointer with [`blazesym_free()`].
 *
 */
struct blazesym *blazesym_new(void);

/**
 * Free an instance of blazesym a symbolizer for C API.
 *
 * # Safety
 *
 * The pointer must be returned by [`blazesym_new()`].
 *
 */
void blazesym_free(struct blazesym *aSymbolizer);

/**
 * Symbolize addresses with the sources of symbols and debug info.
 *
 * Return an array of [`blazesym_result`] with the same size as the
 * number of input addresses.  The caller should free the returned
 * array by calling [`blazesym_result_free()`].
 *
 * # Safety
 *
 * The returned pointer should be freed by [`blazesym_result_free()`].
 *
 */
const struct blazesym_result *blazesym_symbolize(struct blazesym *aSymbolizer,
                                                 const struct sym_src_cfg *aSymSrcs,
                                                 uint32_t aSymSrcsLen,
                                                 const uint64_t *aAddrs,
                                                 uintptr_t aAddrCnt);

/**
 * Free an array returned by blazesym_symbolize.
 *
 * # Safety
 *
 * The pointer must be returned by [`blazesym_symbolize()`].
 *
 */
void blazesym_result_free(const struct blazesym_result *aResults);

#endif /* __blazesym_h_ */
