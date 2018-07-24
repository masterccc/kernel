/*
TODO
  
  

DONE
- Take the kernel module you wrote for task 01, and modify it to be
    create a debugfs subdirectory called "eudyptula".  In that
    directory, create 3 virtual files called "id", "jiffies", and "foo".
- The file "id" operates just like it did for example 06, use the same
    logic there
- The file "jiffies" is to be read only by any user, and when read,
    should return the current value of the jiffies kernel timer.
- When the module is unloaded, all of the debugfs files are cleaned
    up, and any memory allocated is freed.
- The file "foo" needs to be writable only by root, but readable by
    anyone.  When writing to it, the value must be stored, up to one
    page of data.  When read, which can be done by any user, the value
    must be returned that is stored it it.  Properly handle the fact
    that someone could be reading from the file while someone else is
    writing to it (oh, a locking hint!)


http://blog.techveda.org/debugfs/

*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/poll.h>
#include <linux/timer.h>


#define MWORD "abcdef"
#define MWORD_LEN 6

#define MAX_NUMBER_SIZE 15 
#define DBG_FOLDER "eudyptula"


static struct timer_list my_timer;
static struct dentry *my_debug_fs_root;

/* Equivalent mutex http://tuxthink.blogspot.com/2011/05/using-semaphores-in-linux.html */
static DEFINE_SEMAPHORE(sem);

static char data[PAGE_SIZE];
/* begin id debugfs */

static ssize_t id_read(struct file *file, char __user *buf,
                        size_t count, loff_t *ppos)
{
        char *hello_str = MWORD;

        if (*ppos != 0)
                return 0;

        if ((count < MWORD_LEN) ||
                (copy_to_user(buf, hello_str, MWORD_LEN)))
                return -EINVAL;

        *ppos += count;
        return count;
}


static ssize_t id_write(struct file *file, char const __user *buf,
                        size_t count, loff_t *ppos)
{
        char *hello_str = MWORD;
        char input[MWORD_LEN];

        if ((count != MWORD_LEN) ||
                (copy_from_user(input, buf, MWORD_LEN)) ||
                (strncmp(hello_str, input, MWORD_LEN - 1)))
                return -EINVAL;
        else
                return count;
}

static const struct file_operations id_fops = {
        .owner        = THIS_MODULE,
        .write        = id_write,
        .read         = id_read
};
/* end id debugfs */

/* begin foo debugfs */

static ssize_t foo_read(struct file *file, char __user *buf,
                        size_t count, loff_t *ppos)
{

        if (*ppos != 0)
                return 0;
        down(&sem); /* Place le verrou */
        copy_to_user(buf, data, PAGE_SIZE);
        up(&sem); /* Relache le verrou */

		*ppos += count;
		return count;
}


static ssize_t foo_write(struct file *file, char const __user *buf,
                        size_t count, loff_t *ppos)
{

		down(&sem);
		memset(data, 0, PAGE_SIZE);
        copy_from_user(data, buf, (count > PAGE_SIZE ? PAGE_SIZE : count));
        printk("Valeur stockée : %s (%ld)\n", data, count);
        up(&sem);

        return count; /* Nb d'octets lu */
}

static const struct file_operations foo_fops = {
        .owner        = THIS_MODULE,
        .write        = foo_write,
        .read         = foo_read
};

/* end foo debugfs */

/* begin jiffies debugfs */
static ssize_t jiffies_read(struct file *file, char __user *buf,
                        size_t count, loff_t *ppos)
{
        char time[30];

        if (*ppos != 0)
                return 0;

        memset(time,0,30);
        snprintf(time, 30, "%ld", jiffies / HZ );
        time[29] = 0 ;
        copy_to_user(buf, time, 30);
        *ppos += count;
        return count;
}       


static const struct file_operations jiffies_fops = {
        .owner        = THIS_MODULE,
        .read        = jiffies_read
};

/* end jiffies debugfs */

void my_timer_callback( unsigned long data )
{
        printk( "my_timer_callback called (%ld).\n", jiffies );
}

static int __init mydbgfs_init(void)
{

        /*
                Seems obsolete :
                setup_timer(&my_timer, my_timer_callback, jiffies);
        */
        struct dentry *debugfs_id, *debugfs_jiffies, *debugfs_foo ;

        my_debug_fs_root = debugfs_create_dir("eudyptula", NULL);

        if(!my_debug_fs_root)
                goto Fail;
        
        debugfs_jiffies = debugfs_create_file("jiffies", 0444, my_debug_fs_root, NULL, &jiffies_fops);
        if(!debugfs_jiffies)
                goto Fail;

        debugfs_id = debugfs_create_file("id", 0666, my_debug_fs_root, NULL, &id_fops);
        if(!debugfs_id)
                goto Fail;

        debugfs_foo = debugfs_create_file("foo", 0644, my_debug_fs_root, NULL, &foo_fops);
        if(!debugfs_foo)
                goto Fail;
		
        memset(data, 0, PAGE_SIZE);

        return 0 ;

        Fail:
                printk("Failing creating eudyptula debugfs ...");
                debugfs_remove_recursive(my_debug_fs_root);
                return -ENOENT;

}


static void __exit mydbgfs_exit(void)
{
        int ret;

        printk("Removing eudyptula debugfs ...");

        debugfs_remove_recursive(my_debug_fs_root);
        ret = del_timer(&my_timer);
        if(ret)
                printk("Erreur, timer in use");
        return ;
}

module_init(mydbgfs_init)
module_exit(mydbgfs_exit)

MODULE_DESCRIPTION("Debugfs module");
MODULE_AUTHOR("masterccc");
MODULE_LICENSE("GPL");
