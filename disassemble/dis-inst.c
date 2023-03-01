#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

// for elf
#include <err.h>
#include <gelf.h>
#include <libelf.h>
#include <sysexits.h>
// for disassemble
//  to use mmap
#include <error.h>
#include <errno.h>
#include <sys/mman.h>
#include <capstone/capstone.h>
#include <capstone/x86.h>
extern int errno;

char *dis_inst(const uint8_t *code, size_t code_size, uint64_t addr)
{
    csh handle;
    cs_insn *insn;
    size_t count;
    if (cs_open(CS_ARCH_X86, CS_MODE_32, &handle) != CS_ERR_OK)
    {
        fprintf(stderr, "Error cs_open. \n");
        exit(-1);
    }
    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON); // turn ON detail feature with CS_OPT_ON
    count = cs_disasm(handle, code,
                      code_size, addr, 1, &insn);
    if (count <= 0)
    {
        fprintf(stderr, "Error cs_disasm. \n");
        exit(-1);
    }
    else
    {
        return insn[0].op_str;
    }
    return NULL;
}
