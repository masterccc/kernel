#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/errno.h>


#define MAX_NUMBER_SIZE 15 
#define SYS_FOLDER "eudyptula"


static struct kobject *sysfs_root = NULL;
static char data_storage[MAX_NUMBER_SIZE];

/* foo show & store */
static ssize_t foo_show(struct kobject *kobj,
                        struct kobj_attribute *attr,
                        char *buff)
{
        strncpy(buff, data_storage, MAX_NUMBER_SIZE);
        return MAX_NUMBER_SIZE;
}

static ssize_t foo_store(struct kobject *kobj,
                         struct kobj_attribute *attr,
                         const char *buff, size_t count)
{
        int foo_size = min(count, (size_t)MAX_NUMBER_SIZE);
        strncpy(data_storage, buff, foo_size);
        return count;
}

/* bar foo & store */
static ssize_t bar_show(struct kobject *kobj,
                        struct kobj_attribute *attr,
                        char *buff)
{
        strncpy(buff, data_storage, MAX_NUMBER_SIZE);
        return MAX_NUMBER_SIZE;
}

static ssize_t bar_store(struct  kobject *kobj,
                         struct kobj_attribute *attr,
                         const char *buff, size_t count)
{
        int foo_size = min(count, (size_t)MAX_NUMBER_SIZE);
        strncpy(data_storage, buff, foo_size);
        return count;
}

static struct kobj_attribute foo_attr = 
        __ATTR(foo, S_IRUGO | S_IWUSR, foo_show, foo_store);

static struct kobj_attribute bar_attr = 
        __ATTR(bar, S_IRUGO | S_IWUSR, bar_show, bar_store);



static struct attribute *attrs[] = {
        &foo_attr.attr,
        &bar_attr.attr,
        NULL,
};

static struct attribute_group attr_group = {
        .attrs = attrs,
};




static int __init mysysfs_init(void)
{

        int ret ;
        /* Crée le kobject dans /sys/foo */
        /* sysfs_root = kobject_create_and_add("eudyptula", foo_kobj); */

        /* place le kobject dans /sys/eudyptula */
        sysfs_root = kobject_create_and_add("eudyptula", NULL);
        
        if(!sysfs_root)
                goto Fail;
           
        ret = sysfs_create_group(sysfs_root, &attr_group);
        if(ret)
                kobject_put(sysfs_root);

        return ret ;

        Fail:
                printk("Failing creating eudyptula sysfs ...");
                return -ENOENT;

}


static void __exit mysysfs_exit(void)
{

        printk("Removing eudyptula sysfs ...");
        kobject_del(sysfs_root);
        

}

module_init(mysysfs_init)
module_exit(mysysfs_exit)

MODULE_DESCRIPTION("Sysfs module");
MODULE_AUTHOR("masterccc");
MODULE_LICENSE("GPL");
