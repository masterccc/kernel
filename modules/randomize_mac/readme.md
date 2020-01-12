# Randomize MAC address kernel module

## Usage

```
# make

# insmod mac_randomize.ko 

# cat /dev/mac_changer2 
Devices list :
[0] lo
[1] enp2s0f0
[2] wlp3s0

# ip a

...
2: enp2s0f0: [...] link/ether a2:3c:13:a9:b4:93 brd ff:ff:ff:ff:ff:ff
...

# echo 1 > /dev/mac_changer2 

# ip a

...
2: enp2s0f0: [...] link/ether 02:2f:03:c2:ff:d2 brd ff:ff:ff:ff:ff:ff
...

```