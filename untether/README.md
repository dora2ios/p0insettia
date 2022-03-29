# p0insettia - untether
A tool for untethered jailbreak of iOS 10.3.4 ~~32-bit devices~~ iPhone 5 with checkm8 BootROM exploit.  

## Note
All at your own risk!  

## Supported environments
- macOS 10.13 (or later?) (intel/x86_64)

## setup
```
./partitioning_gen.sh
./make_gen_fw.sh
```

## Usage 
### Install old iboot and bootloader  
- gen custom fw  
```
./gen_fw_n42.sh
```
- restore nand_fw  
Please set the device to DFU Mode, connect it, and run the following script.  
```
../build/ipwnder_lite -p
../build/idevicerestore -e -w restorenand.ipsw
```
The device will reboot and enter recovery mode.  

### Install exploit and setup nvram  
Please set the device to DFU Mode, connect it, and run the following script.  
```
./partitioning_boot.sh
```

## LICENSE
GPL-3.0  
