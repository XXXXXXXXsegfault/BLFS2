asm "@entry"
asm "lea @entry-@__X(%rip),%rax"
asm "@__X"
asm "cmp $@entry,%rax"
asm "jne @__Y"
asm "push %rdx"
asm "push %rcx"
asm "pushq $0x100000"
asm "pushq $0"
asm "pushq $0x100000"
asm "call @memset"
asm "add $24,%rsp"
asm "call @main"
asm "add $16,%rsp"
asm "@__Y"
asm "ret"
#include "../include/mem.c"
#include "efi.c"
struct EFI_system_table *efitab;
void *efihandle;
void bootid(void); // filled by build.c
#include "palloc.c"
#include "graphics.c"
#include "block.c"
#include "../include/ext2.c"
#include "ext2_load.c"
#include "do_boot.c"
int main(void *handle,struct EFI_system_table *tab)
{
	efihandle=handle;
	efitab=tab;
	if(graphics_init())
	{
		return -1;
	}
	if(block_init())
	{
		return -2;
	}
	if(ext2_init())
	{
		return -3;
	}
	return boot_init();
}
