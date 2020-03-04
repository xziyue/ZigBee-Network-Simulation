modprobe -r fakelb
modprobe fakelb numlbs=2

iwpan dev wpan0 set pan_id 0xbeef
iwpan dev wpan1 set pan_id 0xbeef

iwpan dev wpan0 set short_addr 0x0000
iwpan dev wpan1 set short_addr 0x0001

ip link set wpan0 up
ip link set wpan1 up
