/*
 * Print the names of ELF sections .
 */
#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>
// #include <vis.h>

void trevalSections(char *filename)
{
    int fd;
    Elf *e;
    char *name, *p, pc[4 * sizeof(char)];
    Elf_Scn *scn;
    Elf_Data *data;
    GElf_Shdr shdr;
    size_t n, shstrndx, sz;

    if (elf_version(EV_CURRENT) == EV_NONE)
        errx(EX_SOFTWARE, " ELF library initialization "
                          " failed : %s ",
             elf_errmsg(-1));
    if ((fd = open(filename, O_RDONLY, 0)) < 0)
        err(EX_NOINPUT, " open %s\" failed ", filename);
    if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
        errx(EX_SOFTWARE, " elf_begin () failed : %s . ", elf_errmsg(-1));
    if (elf_kind(e) != ELF_K_ELF)
        errx(EX_DATAERR, " %s is not an ELF object . ", filename);
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
        printf(" Section %-4.4jd %s \n ", (uintmax_t)elf_ndxscn(scn), name);
    }

    if ((scn = elf_getscn(e, shstrndx)) == NULL)
        errx(EX_SOFTWARE, " getscn () failed : %s . ",
             elf_errmsg(-1));
    if (gelf_getshdr(scn, &shdr) != &shdr)
        errx(EX_SOFTWARE, " getshdr ( shstrndx ) failed : %s . ",
             elf_errmsg(-1));
    putchar( '\n' );
    elf_end(e);
    close(fd);
}

int main(){
    trevalSections("test");
    return 0;
}
