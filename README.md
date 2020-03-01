# ZigBee Network Simulation

![zigbee](./doc/zigbee.png)

## Development stages
- [x] Port ZBOSS to CMake
- [x] Test `zdo_startup` and fix Wireshark compatibility
- [x] Migrate the project to 64 bit
- [ ] Provide Linux WPAN support
- [ ] Inter-machine WPAN backend
- [ ] Write ZigBee device
- [ ] More realistic simulation using docker
- [ ] Device UI
- [ ] Demonstrate fake install code attack
- [ ] Demonstrate replay attack

## Platform requirements
- Ubuntu, Mac OS
- Little-endian machine

## Package dependency on Ubuntu x64
- libxml++2.6-dev
- libpcap-dev

## Build

- Define `ZB_TRANSPORT_USE_LINUX_PIPES` to simulate network w/ Linux pipes.
Define `ZB_TRANSPORT_USE_LINUX_WPAN` to simulate network w/ Linux WPAN.

## Create virtual interface on Ubuntu
- Install [wpan-tools](https://packages.debian.org/sid/wpan-tools)
```shell script
modprobe -r fakelb
modprobe fakelb numlbs=1
iwpan dev wpan0 set pan_id 0xbeef
ip link set wpan0 up
```

