# p0insettia
A tool for [(semi-){un-(tethered jailbreak)}] of iOS 10.3.4 ~~32-bit devices~~ iPhone 5 with checkm8 BootROM exploit.  

## Note
- All at your own risk!  
- The package used for this jailbreak can be obtained via Cydia from the following repository.  
`https://dora2ios.github.io/repo`  

## Supported devices
- iPhone 5 (N41/N42) - iOS 10.3.4  

## Supported environments
- macOS 10.13 (or later?) (intel/x86_64)

## Make
```
./build.sh all
cd build
./dl_files
ramdisk_gen.sh
```

## semi-tethered jailbreak
Please refer to the build/ directory.  

## semi-untethered jailbreak
It uses an IPA App based jailbreak. (ETA SON)   

## untethered jailbreak
It uses an iBoot(7) exploit based jailbreak. (ETA SON)   

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
