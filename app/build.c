#include "copy/build.c"
#include "cpio/build.c"
#include "mkfs.ext2/build.c"
#include "mkfs.fat/build.c"
#include "remove/build.c"
#include "init/build.c"
#include "ttym/build.c"
#include "shell/build.c"
#include "list/build.c"
#include "pwd/build.c"
#include "create/build.c"
#include "fm/build.c"
#include "edit/build.c"
#include "plist/build.c"
void build_apps(void)
{
	build_copy("build/initramfs/bin/copy");
	build_cpio("build/initramfs/bin/cpio");
	build_mkfs_ext2("build/initramfs/bin/mkfs.ext2");
	build_mkfs_fat("build/initramfs/bin/mkfs.fat");
	build_remove("build/initramfs/bin/remove");
	build_init("build/initramfs/init");
	build_ttym("build/initramfs/bin/ttym");
	build_shell("build/initramfs/bin/shell");
	build_list("build/initramfs/bin/list");
	build_pwd("build/initramfs/bin/pwd");
	build_create("build/initramfs/bin/create");
	build_fm("build/initramfs/bin/fm");
	build_edit("build/initramfs/bin/edit");
	build_plist("build/initramfs/bin/plist");
}
