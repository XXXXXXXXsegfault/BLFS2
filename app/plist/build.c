#include "../../include/build.c"
void build_plist(char *dst)
{
	cc("app/plist/main.c","build/tmp/tmp.asm");
	assemble("build/tmp/tmp.asm",dst);
}
