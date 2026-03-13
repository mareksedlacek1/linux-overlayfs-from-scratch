#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/pagemap.h> // for simple_dir_operations/simple_inode_operations


// macros used for metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Marek");
MODULE_DESCRIPTION("A Simple Layered Filesystem (Overlay)");


// Here starts my journey :)

static const struct super_operations my_super_ops = {
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode,
};

static int my_fill_super(struct super_block *sb, void *data, int silent) { 
    struct inode *inode = new_inode(sb);

    inode->i_ino = 1; // Set inode number (1 is standard for root)
    inode->i_mode =  S_IFDIR | 0755; // Set as directory with permissions
    inode->i_op = &simple_dir_inode_operations; // Assign directory operations
    inode->i_fop = &simple_dir_operations;// Assign file operations

    sb->s_op = &my_super_ops; // Required: superblock operations
    sb->s_root = d_make_root(inode); // Required: root dentry
    sb->s_maxbytes = MAX_LFS_FILESIZE; // maximum size allowed by the kernel and architecture
    return 0; 
}

/* mount_nodev is kernel helper function for mounting filesystems that do not use a physical device
it creates a root dentry (the root director entry for my filesystem)
it calls a function to set up the superblock and root inode (NULL is passed no custom setup yet)
In are case it's useful because are goals is not to manage the storage device, instead we want to merge existing directories from other filesystems.
*/
static struct dentry *my_mount_function(struct file_system_type *fs_type, int flags, const char *dev_name, void*data) {
    printk(KERN_INFO "mounted function called \n"); // kernel log level
    return mount_nodev(fs_type, flags, data, my_fill_super); // GOOD: kernel calls function safely
}

static void my_kill_sb(struct super_block *sb) {
    printk(KERN_INFO "OverlayFS: unmounted\n");
    kill_anon_super(sb);
}

/* I am not defining a new struct type but creating a variable of an existing struct type and initializing its field. 
kernel's built-in struct called struct file_system_type
static -> so not accessible out of the module 
*/
static struct file_system_type myfs_type ={
    .name = "overlayfs",
    .owner = THIS_MODULE,
    .mount = my_mount_function,                    
    .kill_sb = my_kill_sb, // the method to call when an instance of this filesystem should be shut down
};

/*creating init and exit function, the kernel calls these when module is loaded or unloaded*/
static int __init myfs_init(void){
    printk(KERN_INFO "OverlayFS: registered\n");
    return register_filesystem(&myfs_type);
}
static void __exit myfs_exit(void){
    unregister_filesystem(&myfs_type);
    printk(KERN_INFO "OverlayFS: unregistered\n");
}

// these are marcros that tell kernel which functions to call when module is loaded and unloaded
module_init(myfs_init); // insmod   
module_exit(myfs_exit); // rmmod


//Add minimal virtual filesystem module with root inode