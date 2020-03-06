# ZigBee Network Simulation

![zigbee](./doc/zigbee.png)

## Development stages
- [x] Port ZBOSS to CMake
- [x] Test `zdo_startup` and fix Wireshark compatibility
- [x] Migrate the project to 64 bit
- [x] Provide Linux WPAN support
- [ ] Inter-machine WPAN backend
- [x] Inter-machine UDP backend
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

It is most convenient to develop with [CLion](https://www.jetbrains.com/clion/).

### Swithcing between Linux PIPE and WPAN

There are two options in `CMakeLists.txt` at the root folder. Uncomment one of them
to configure the right build option.

```cmake
# use linux pipes
#add_definitions(-DZB_TRANSPORT_USE_LINUX_PIPES)

# use linux wpan
#add_definitions(-DZB_TRANSPORT_USE_LINUX_WPAN)
```

Once the config is changed, rerun cmake.

### Manual build

To build with cmake manually:
1. Create a build folder for cmake under the root directory and cd into
the folder:
    ```shell script
    mkdir build
    cd build
    ```
2. Run cmake to generate cmake cache (now only debug build is maintained):
    ```shell script
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    ```
3. Build the entire project with cmake:
    ```
   cmake --build .
   ```
   If you wish to build a specific target:
   ```shell script
   cmake --build . --target zdo_startup
   ```
    *The Linux PIPE/wpan option will automatically add/remove some test cases.
    If cmake cannot find a target, please make sure the transport type is set correctly.*
    
    
## Create virtual interface on Ubuntu
- Install [wpan-tools](https://packages.debian.org/sid/wpan-tools)
```shell script
modprobe -r fakelb
modprobe fakelb numlbs=1
iwpan dev wpan0 set pan_id 0xbeef
ip link set wpan0 up
```

