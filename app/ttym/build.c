#include "../../include/build.c"
void build_ttym(char *dst)
{
	cc("app/ttym/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
