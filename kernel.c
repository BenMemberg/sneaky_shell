#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

//The init function and exit function most likely has to be changed

static int init(void){
  printk(KERN_INFO "My module is loaded."\n);
  return 0;
}

static void uninit(void){

  printk(KERN_INFO "My module is unloaded. \n")
}

module_init(init);
,module_exit(bye);
