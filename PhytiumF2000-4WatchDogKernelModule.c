#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<asm/io.h>
#include<asm/uaccess.h>
#include<asm/sizes.h>


// define registers
#define WDT_BASE 0x0002800A000 // base addr
#define WDT_WRR WDT_BASE+0x0000 // updating register addr (feeding dog)
#define WDT_WCS WDT_BASE+0x1000 // controlling/status register addr
#define WDT_WOR WDT_BASE+0x1008 // counting register addr
// define operations
#define WDT_ENABLE 1
#define WDT_UPDATE 2

void *wdt_base,*wdt_wrr,*wdt_wcs,*wdt_wor;
static int wdt_devno=0,wdt_major=0,wdt_minor=0;
struct cdev* wdt_cdev;


void wdt_update(void){
    writel(0x1,wdt_wrr);
}



void wdt_enable(void){
    unsigned int reg=readl(wdt_wcs);
    reg|=(0x1<<0);
    writel(reg,wdt_wcs);
}



long wdt_ioctl(struct  file *flip,unsigned int cmd,unsigned long args){
    switch(cmd){
        case WDT_ENABLE:
            wdt_enable();
            break;
        case WDT_UPDATE:
            wdt_update();
            break;
        default:
            return -EINVAL;
    }
    return 0;
}



struct file_operations wdt_fops={
    .unlocked_ioctl=wdt_ioctl,
};



static int init(void){
	printk("[wdt] module loaded\n");

    printk("vaddrs mapped\n");
    wdt_wrr=ioremap(WDT_WRR,SZ_4K);
    wdt_wcs=ioremap(WDT_WCS,SZ_4K);

    int ret=0;
    if(ret=alloc_chrdev_region(&wdt_devno,0,1,"wdt")){
		printk("[wdt] driver registeration failed\n");
        return ret;
	}
    wdt_major=MAJOR(wdt_devno);
    wdt_minor=MINOR(wdt_devno);
	printk("[wdt] driver registered with major %d, minor %d\n",wdt_major,wdt_minor);

    wdt_cdev=cdev_alloc();
    cdev_init(wdt_cdev,&wdt_fops);
    if(ret=cdev_add(wdt_cdev,wdt_devno,1)){
        printk("[wdt] device adding failed\n");
        unregister_chrdev_region(wdt_devno,1);
        return ret;
    }
    printk("[wdt] device added\n");

	return 0;
}



static void exit(void){
    cdev_del(wdt_cdev);
    printk("[wdt] device removed\n");
	unregister_chrdev_region(wdt_devno,1);
	printk("[wdt] driver unregistered\n");
	printk("[wdt] module unloaded\n");
    iounmap(wdt_wcs);
    iounmap(wdt_wrr);
    printk("vaddrs unmapped\n");
}



module_init(init);
module_exit(exit);



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pairman");
