#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#include <linux/random.h>



#define MOD_NAME "mac_changer2"
#define STATUS_LEN 2

#define TXT_READ "Devices list :\n"
#define TXT_READ_LEN (strlen(TXT_READ))

#define MAC_ADDR_SIZE 6

#define PRINT_BUF 256
#define ASCII_CONV 48
#define INPUT_LEN 2

static ssize_t sample_read(struct file *file, char __user *buf,
        size_t count, loff_t *ppos)
{

        struct net_device *dev;
        char buffer[PRINT_BUF], *p;
        size_t current_size, write_size ;
        unsigned int i ;

        p = buffer ;
        memset(buffer, 0, PRINT_BUF);

        current_size = PRINT_BUF - 1 ;
        write_size = snprintf(buffer, current_size, "%s", TXT_READ);
        current_size -= write_size ;
        p+= write_size ;

        printk("Liste et selection du device :\n");

        i = 0 ;
        dev = first_net_device(&init_net);
        while (dev) {
                printk(KERN_INFO "found [%s]\n", dev->name);
                write_size = snprintf(p, current_size, "[%u] %s\n", i, dev->name);
                current_size -= write_size ;
                p+= write_size ;
                i++;
                dev = next_net_device(dev);
        }

        if (*ppos != 0)
                return 0;

        if ((count < STATUS_LEN) ||
                (copy_to_user(buf, buffer, PRINT_BUF - current_size)))
                return -EINVAL;

        *ppos += count;
        return count;
}


static ssize_t sample_write(struct file *file, char const __user *buf,
            size_t count, loff_t *ppos)
{

        char input[INPUT_LEN];
        int changed, i, error;
        struct sockaddr macaddr ;
        struct net_device *dev;

        macaddr.sa_family = AF_UNIX ;
        changed = 0 ;
        i = 0 ;
        error = 0;

        /* count = number of bytes sent */
        if (copy_from_user(input, buf, INPUT_LEN))
                return -EINVAL;

        get_random_bytes(&macaddr.sa_data, MAC_ADDR_SIZE);

        i = 0 ;
        dev = first_net_device(&init_net);
        while (dev) {
                if( (i+ASCII_CONV) == input[0]){
                        changed = 1 ;
                        printk(MOD_NAME ":" "Change on %s\n", dev->name);
                        break;
                }
                dev = next_net_device(dev);
                i++;
        }
        if(!changed)
                return -EINVAL;
        
        rtnl_lock();
        error = dev_set_mac_address(dev, &macaddr,NULL);
        rtnl_unlock();

        if(error){
                printk("Cannot change address - error %d\n",error);
                return -EINVAL ;
        }

        return count; 
}

static const struct file_operations sample_fops = {
        .owner          = THIS_MODULE,
        .write          = sample_write,
        .read           = sample_read,
        .llseek         = no_llseek,
};

struct miscdevice macchanger_dev = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = MOD_NAME,
        .fops = &sample_fops,
};


static int __init misc_init(void)
{
        int error;

        error = misc_register(&macchanger_dev);

        if (error) {
                pr_err("can't misc_register\n");
                return error;
        }
        pr_info(MOD_NAME " enabled.\n");
        return 0 ;
}

static void __exit misc_exit(void)
{
        misc_deregister(&macchanger_dev);
        pr_info(MOD_NAME " disabled\n");
}

module_init(misc_init)
module_exit(misc_exit)

MODULE_DESCRIPTION("Randomize MAC address");
MODULE_AUTHOR("masterccc");
MODULE_LICENSE("GPL");
