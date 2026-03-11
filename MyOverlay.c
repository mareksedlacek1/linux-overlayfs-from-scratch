#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Marek");
MODULE_DESCRIPTION("A Simple Layered Filesystem (Overlay)");

// This function is called when the Kernel tries to mount our FS
static struct dentry *mount(struct file_system_type *fs_type, 
                               int flags, const char *dev_name, void *data) {
    // For now, we'll just print a message. 
    // In a real driver, this is where we'd link the lower and upper folders.
    printk(KERN_INFO "Overlay: Mount attempted on device %s\n", dev_name);
    return mount_nodev(fs_type, flags, data, NULL); 
}

// Defining our Filesystem structure
static struct file_system_type fs_type = {
    .owner = THIS_MODULE,
    .name = "overlay",      // This is the name used in the 'mount -t' command
    .mount = mount,
    .kill_sb = kill_anon_super, // Standard way to 'unmount' a virtual FS
};

// When the module is loaded (insmod)
static int __init my_overlay_init(void) {
    int ret = register_filesystem(&fs_type);
    if (ret == 0)
        printk(KERN_INFO "Overlay: Filesystem registered successfully!\n");
    return ret;
}

// When the module is removed (rmmod)
static void __exit my_overlay_exit(void) {
    unregister_filesystem(&fs_type);
    printk(KERN_INFO "Overlay: Filesystem unregistered.\n");
}

module_init(my_overlay_init);
module_exit(my_overlay_exit);