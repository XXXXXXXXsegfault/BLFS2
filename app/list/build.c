#include "../../include/build.c"
void build_list(char *dst)
{
	cc("app/list/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
