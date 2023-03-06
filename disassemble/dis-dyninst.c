// 处理perf script --itrace=bw 的输出结果
// 提取跳转指令地址和目标地址以便后期反汇编
// 提取ptwrite的地址和payload

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
#include "dis-inst.h"

#define MAX_CODE_SIZE 20

typedef struct Section_
{
    char *name_ref;
    GElf_Addr start_vaddr;
    GElf_Xword size;
    GElf_Off file_offset;
} Section;

typedef struct Function_
{
    char *name_ref;
    GElf_Addr start_vaddr;
    GElf_Xword size;
    GElf_Off file_offset;
} Function;

// return 0 if can't get the function
int getFunction(Elf *elf, char *func_name, Function *func)
{
    Elf_Scn *scn = NULL;
    Elf_Scn *sym_scn = NULL;
    GElf_Shdr symtab_shdr;
    Elf_Data *data;
    int ii, count;

    // get symbol table
    while ((scn = elf_nextscn(elf, scn)) != NULL)
    {
        gelf_getshdr(scn, &symtab_shdr);
        if (symtab_shdr.sh_type == SHT_SYMTAB)
        {
            /* found a symbol table */
            sym_scn = scn;
            break;
        }
    }

    data = elf_getdata(sym_scn, NULL);
    count = symtab_shdr.sh_size / symtab_shdr.sh_entsize;

    /* print the symbol names */
    for (ii = 0; ii < count; ++ii)
    {
        GElf_Sym sym;
        gelf_getsym(data, ii, &sym);
        char *sym_name = elf_strptr(elf, symtab_shdr.sh_link, sym.st_name);
        if (!strcmp(sym_name, func_name))
        {
#ifdef DEBUG
            printf("%s\n", sym_name);
            printf("GOT IT! 0x%lx 0x%lx\n", sym.st_value, sym.st_size);
#endif
            func->name_ref = func_name;
            func->start_vaddr = sym.st_value;
            func->size = sym.st_size;

            return 1;
        }
    }

    return 0;
}

int getSection(Elf *elf, char *sec_name, Section *sec)
{
    char *name;
    Elf_Scn *scn;
    GElf_Shdr shdr;
    size_t shstrndx;
    Elf *e = elf;

    if (elf_getshdrstrndx(e, &shstrndx) != 0)
        errx(EX_SOFTWARE, " elf_getshdrstrndx () failed : %s . ", elf_errmsg(-1));

    scn = NULL;
    while ((scn = elf_nextscn(e, scn)) != NULL)
    {
        if (gelf_getshdr(scn, &shdr) != &shdr)
            errx(EX_SOFTWARE, " getshdr () failed : %s . ",
                 elf_errmsg(-1));
        if ((name = elf_strptr(e, shstrndx, shdr.sh_name)) == NULL)
            errx(EX_SOFTWARE, " elf_strptr () failed : %s . ",
                 elf_errmsg(-1));

        if (!strcmp(name, sec_name))
        {
#ifdef DEBUG
            printf("%s\n", sec_name);
            printf("GOT IT! 0x%lx 0x%lx 0x%lx\n", shdr.sh_addr, shdr.sh_offset, shdr.sh_size);
#endif
            sec->name_ref = sec_name;
            sec->start_vaddr = shdr.sh_addr;
            sec->size = shdr.sh_size;
            sec->file_offset = shdr.sh_offset;

            return 1;
        }
    }

    return 0;
}

bool inSection(uint64_t target, Section *sec)
{
    return target >= sec->start_vaddr &&
           target <= (sec->start_vaddr + sec->size);
}

Elf *open_elf(char *path, int fd)
{
    Elf *e;
    if (elf_version(EV_CURRENT) == EV_NONE)
        errx(EX_SOFTWARE, " ELF library initialization "
                          " failed : %s ",
             elf_errmsg(-1));
    if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
        errx(EX_SOFTWARE, " elf_begin () failed : %s . ", elf_errmsg(-1));
    if (elf_kind(e) != ELF_K_ELF)
        errx(EX_DATAERR, " %s is not an ELF object . ", path);

    return e;
}

int main(int argc, char **argv)
{
    /* open file and elf */
    int fd;

    if ((fd = open(argv[1], O_RDWR, 0)) < 0)
        errx(EX_NOINPUT, " open %s\" failed ", argv[1]);

    Elf *e = open_elf(argv[1], fd);

    /* read elf , get some sections and function. */
    Section text, dyninstInst;
    Function ptwrite_chunk;
    getSection(e, ".text", &text);
    getSection(e, ".dyninstInst", &dyninstInst);
    // getFunction(e, "ptwrite_chunk", &ptwrite_chunk);

    // map section .dyninstInst
    void *dyninstInst_mem = mmap(NULL, dyninstInst.size,
                                 PROT_READ | PROT_WRITE, MAP_SHARED,
                                 fd, dyninstInst.file_offset);
    if (dyninstInst_mem == MAP_FAILED)
    {
        fprintf(stderr, "Value of errno: %d\n", errno);
        fprintf(stderr, "Error mapping dyninstInst: %s\n", strerror(errno));
    }

    /* read (perf script --itrace=bw) from stdin */
    FILE *out = fopen("out.bw", "w+");
    if (out == NULL)
    {
        fprintf(stderr, "open failed. \n");
        exit(-1);
    }

    char buf[1024];
    char temp_buf[1024];
    char append[1024];
    char *p, *inst_str;
    int token_cnt;
    while (fgets(buf, sizeof(buf), stdin) != NULL)
    {
        strcpy(temp_buf, buf);
        token_cnt = 0;
        p = strtok(buf, " ");
        if (!strcmp(p, "branch:"))
        {
            p = strtok(NULL, " ");

            uint64_t ip = strtoull(p, NULL, 16);
            uint8_t *code_addr;

            if (inSection(ip, &dyninstInst))
            {
                code_addr = (uint8_t *)dyninstInst_mem + (ip - dyninstInst.start_vaddr);
            }
            else
            {
                continue;
            }

            inst_str = dis_inst(code_addr, 20, ip);
            fputs(strcat(temp_buf, inst_str), out);
        }
        else // ptwrite:
        {
            fputs(temp_buf, out);
        }
    }

    fclose(out);

    return 0;
}
