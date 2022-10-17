#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <err.h>
#include <gelf.h>
#include <libelf.h>
#include <sysexits.h>

int main(int argc, char **argv)
{
    Elf *elf;
    Elf_Scn *scn = NULL;
    Elf_Scn *sym_scn = NULL;
    GElf_Shdr symtab_shdr;
    Elf_Data *data;
    int fd, ii, count;

    elf_version(EV_CURRENT);

    fd = open(argv[1], O_RDONLY);
    elf = elf_begin(fd, ELF_C_READ, NULL);

    while ((scn = elf_nextscn(elf, scn)) != NULL)
    {
        gelf_getshdr(scn, &symtab_shdr);
        if (symtab_shdr.sh_type == SHT_SYMTAB)
        {
            /* found a symbol table */
            sym_scn=scn;
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
        const char* sym_name=elf_strptr(elf, symtab_shdr.sh_link, sym.st_name);
        if(1||!strcmp(sym_name,"ptwrite_chunk")||
            !strcmp(sym_name,".text")||
            !strcmp(sym_name,".dyninstInst")){
            printf("%s\n", sym_name);
            printf("GOT IT! 0x%lx 0x%lx\n",sym.st_value,sym.st_size);
            // break;
        }
    }

    

    elf_end(elf);
    close(fd);

    return 0;
}