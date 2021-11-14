# p0insettia - ipwnder_lite
*A Tool for utilizing iOS devices using checkm8  BootROM exploit*  

## Features  
This tool is intended to take advantage of the BootROM exploit present on iOS devices.  
It is written in the C and is only available on macosx.  

## Supported environments  
- macOS 10.13 or later (intel/x86_64)  

## Library dependencies  
- [libcurl](https://github.com/curl/curl)  

## Make 
```
./mk_macosx.sh
```

## Note
Please use a regular usb-A to lightning cable. If you go through serial URAT, JTAG/SWD cable, etc., the exploit may fail badly.  

## LICENSE
The license for this entire project is GPLv3, but if the license is listed in the source code header, it will be followed.  
