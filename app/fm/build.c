#include "../../include/build.c"
void build_fm(char *dst)
{
	cc("app/fm/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
