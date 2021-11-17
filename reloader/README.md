# sock_port + p0insettia reloader
A tool for semi-untethered jailbreak of iOS 10.3.4 ~~32-bit devices~~ iPhone 5 with socket exploit.  

## Note
- All at your own risk!  
- The package used for this jailbreak can be obtained via Cydia from the following repository.  
`https://dora2ios.github.io/repo`  

## Supported devices
- iPhone 5 (N41/N42) - iOS 10.3.4  

## Note for this jailbreak environment (iOS 10.3 or higher)  
This jailbreak will not apply the nuke sandbox patch used by h3lix.  
In iOS 10.3 and later, apps under /Applications will also be sandboxed. so, Apps such as Cydia will be sandboxed and will not work. For this reason, this jailbreak adds a key to Cydia's entitlements to disable sandbox.  

- Key `com.apple.private.security.no-container`  

Some other jailbreak apps may require this entitlement.  

- entitlement key
```
<key>com.apple.private.security.no-container</key>
<true/>
```

## credits
- Ned Williamson for the socket bug  
- jakeajames for the [sock_port_2](https://github.com/jakeajames/sock_port/tree/sock_port_2)  
- SongXiaoXi for the sock_port_2 support for armv7 devices  
- qwertyoruiopz for the yalu102  
- planetbeing for [ios-jailbreak-patchfinder](https://github.com/planetbeing/ios-jailbreak-patchfinder)  
- xerub for the iloader  
- checkra1n team for the kernel patche methods  
