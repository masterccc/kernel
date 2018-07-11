#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/errno.h>

#define MOD_NAME "misc_dri"

#define MWORD "abcdef"
#define MWORD_LEN 6

static int sample_open(struct inode *inode, struct file *file)
{
    pr_info(MOD_NAME " [Ouverture]\n");
    return 0;
}

static int sample_close(struct inode *inodep, struct file *filp)
{
    pr_info(MOD_NAME " [Fermeture]\n");
    return 0;
}

static ssize_t sample_read(struct file *file, char __user *buf,
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


static ssize_t sample_write(struct file *file, char const __user *buf,
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

static const struct file_operations sample_fops = {
    .owner			= THIS_MODULE,
    .write			= sample_write,
    .open			= sample_open,
    .read 			= sample_read,
    .release		= sample_close,
    .llseek 		= no_llseek,
};

struct miscdevice sample_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = MOD_NAME,
    .fops = &sample_fops,
};

static int __init misc_init(void)
{
    int error;

    error = misc_register(&sample_device);
    if (error) {
        pr_err("can't misc_register\n");
        return error;
    }

    pr_info(MOD_NAME " enabled\n");
    printk("hello :)");
    return 0;
}

static void __exit misc_exit(void)
{
    misc_deregister(&sample_device);
    pr_info(MOD_NAME " disabled\n");
}

module_init(misc_init)
module_exit(misc_exit)

MODULE_DESCRIPTION("Misc driver");
MODULE_AUTHOR("masterccc");
MODULE_LICENSE("GPL");
