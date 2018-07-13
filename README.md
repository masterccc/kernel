# kernel

## Building the Kernel (for a minimal working with Qemu)

```kernel_build/config_kernel``` is a minimal config file, (```make tinyconfig``` + minimal stuff to run in qemu (debug, printk,uncompress gzip initram disk))

```getandbuild.sh``` : download, decompress and build the kernel (not yet tested)

```mkinitcpio.conf``` : default mkinitcpio, create an initramfs with busybox
