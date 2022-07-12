#ifndef _SYSCALL_C_
#define _SYSCALL_C_
#include "error.c"
#define NULL ((void *)0)
#define AT_FDCWD (-100)
#define AT_SYMLINK_NOFOLLOW 0x100
#define AT_REMOVEDIR 0x200
asm ".entry"
asm "lea 8(%rsp),%rax"
asm "push %rax"
asm "pushq 8(%rsp)"
asm "call @main"
asm "mov %rax,%rdi"
asm "mov $231,%eax"
asm "syscall"
long __syscall(long num,long a1,long a2,long a3,long a4,long a5,long a6);
asm "@__syscall"
asm "push %rdi"
asm "push %rsi"
asm "push %rdx"
asm "push %r10"
asm "push %r11"
asm "push %r8"
asm "push %r9"
asm "mov 64(%rsp),%rax"
asm "mov 72(%rsp),%rdi"
asm "mov 80(%rsp),%rsi"
asm "mov 88(%rsp),%rdx"
asm "mov 96(%rsp),%r10"
asm "mov 104(%rsp),%r8"
asm "mov 112(%rsp),%r9"
asm "syscall"
asm "pop %r9"
asm "pop %r8"
asm "pop %r11"
asm "pop %r10"
asm "pop %rdx"
asm "pop %rsi"
asm "pop %rdi"
asm "ret"
#define syscall(num,a1,a2,a3,a4,a5,a6) __syscall((long)num,(long)a1,(long)a2,(long)a3,(long)a4,(long)a5,(long)a6)
#define valid(s) ((unsigned long)((long)(s))<=0xfffffffffffff000)
#define read(fd,buf,size) syscall(0,fd,buf,size,0,0,0)
#define write(fd,buf,size) syscall(1,fd,buf,size,0,0,0)
#define open(name,flags,mode) syscall(2,name,flags,mode,0,0,0)
#define close(fd) syscall(3,fd,0,0,0,0,0)
#define stat(path,st) syscall(4,path,st,0,0,0,0)
#define fstat(fd,st) syscall(5,fd,st,0,0,0,0)
#define lstat(path,st) syscall(6,path,st,0,0,0,0)
#define poll(pfd,nfds,timeout) syscall(7,pfd,nfds,timeout,0,0,0)
#define lseek(fd,off,whence) syscall(8,fd,off,whence,0,0,0)
#define mmap(addr,size,prot,flags,fd,off) ((void *)syscall(9,addr,size,prot,flags,fd,off))
#define munmap(addr,size) syscall(11,addr,size,0,0,0,0)
#define brk(addr) ((void *)syscall(12,addr,0,0,0,0,0))
#define ioctl(fd,cmd,arg) syscall(16,fd,cmd,arg,0,0,0)
#define access(path,mode) syscall(21,path,mode,0,0,0,0)
#define dup(fd) syscall(32,fd,0,0,0,0,0)
#define dup2(fd,newfd) syscall(33,fd,newfd,0,0,0,0)
#define nanosleep(req,rem) syscall(35,req,rem,0,0,0,0)
#define getpid() syscall(39,0,0,0,0,0,0)
#define fork() syscall(57,0,0,0,0,0,0)
#define kill(pid,sig) syscall(62,pid,sig,0,0,0,0)
#define chdir(dir) syscall(80,dir,0,0,0,0,0)
#define rename(name,newname) syscall(82,name,newname,0,0,0,0)
#define mkdir(path,mode) syscall(83,path,mode,0,0,0,0)
#define rmdir(path) syscall(84,path,0,0,0,0,0)
#define link(name,newname) syscall(86,name,newname,0,0,0,0)
#define symlink(target,path) syscall(88,target,path,0,0,0,0)
#define setpgid(pid,pgid) syscall(109,pid,pgid,0,0,0,0)
#define mknod(name,mode,dev) syscall(133,name,mode,dev,0,0,0)
#define setsid() syscall(112,0,0,0,0,0,0)
#define chroot(dir) syscall(161,dir,0,0,0,0,0)
#define sync() syscall(162,0,0,0,0,0,0)
#define mount(dev,mp,type,flags,opt) syscall(165,dev,mp,type,flags,opt,0)
#define umount2(mp,flags) syscall(166,mp,flags,0,0,0,0)
#define reboot(m,m2,cmd,arg) syscall(169,m,m2,cmd,arg,0,0)
#define delete_module(name,flags) syscall(176,name,flags,0,0,0,0)
#define getdents64(fd,buf,size) syscall(217,fd,buf,size,0,0,0)
#define exit(code) syscall(231,code,0,0,0,0,0)
#define openat(dirfd,path,flags,mode) syscall(257,dirfd,path,flags,mode,0,0)
#define mkdirat(dirfd,path,mode) syscall(258,dirfd,path,mode,0,0,0)
#define fstatat(dirfd,path,st,flags) syscall(262,dirfd,path,st,flags,0,0)
#define unlinkat(dirfd,path,flags) syscall(263,dirfd,path,flags,0,0,0)
#define symlinkat(target,dirfd,path) syscall(266,target,dirfd,path,0,0,0)
#define readlinkat(dirfd,path,buf,size) syscall(267,dirfd,path,buf,size,0,0)
#define fchmodat(dirfd,path,mode) syscall(268,dirfd,path,mode,0,0,0)
#define finit_module(fd,params,flags) syscall(313,fd,params,flags,0,0,0)
#define getrandom(buf,size,flags) syscall(318,buf,size,flags,0,0,0)
long int vfork(void);
asm "@vfork"
asm "pop %rdx"
asm "mov $58,%eax"
asm "syscall"
asm "jmp *%rdx"
long int execv(char *path,char **argv)
{
	char *env[1];
	env[0]=0;
	return syscall(59,path,argv,env,0,0,0);
}
long int wait(int *status)
{
	return syscall(61,-1,status,0,0,0,0);
}
#define umount(mpt) umount2(mpt,0)
#define readlink(name,buf,size) readlinkat(AT_FDCWD,name,buf,size)
#define chmod(name,mode) fchmodat(AT_FDCWD,name,mode)
struct timespec
{
	unsigned long sec;
	unsigned long nsec;
};
void sleep(unsigned int sec,unsigned int usec)
{
	struct timespec t;
	t.sec=sec;
	t.nsec=usec*1000;
	nanosleep(&t,0);
}

#endif
