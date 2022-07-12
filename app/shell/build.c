#include "../../include/build.c"
void build_shell(char *dst)
{
	cc("app/shell/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
