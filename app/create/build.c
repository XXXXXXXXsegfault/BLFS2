#include "../../include/build.c"
void build_create(char *dst)
{
	cc("app/create/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
