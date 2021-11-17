//
//  ViewController.m
//  reloader
//
//  Created by sakuRdev on 2021/11/18.
//  Copyright (c) 2021 sakuRdev. All rights reserved.
//

#import "ViewController.h"

#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/utsname.h>
#include <mach-o/dyld.h>
#include <copyfile.h>
#include <spawn.h>

#include "exploit.h"
#include "unjail.h"

void restart(void);

void loader(void)
{
    const char *jl;
    pid_t pd = 0;
    
    char path[256];
    uint32_t size = sizeof(path);
    _NSGetExecutablePath(path, &size);
    char* pt = realpath(path, 0);
    
    NSString* execpath = [[NSString stringWithUTF8String:pt]  stringByDeletingLastPathComponent];
    
    {
        NSString* jlaunchctl = [execpath stringByAppendingPathComponent:@"reload"];
        NSString* jdeamon = [execpath stringByAppendingPathComponent:@"0.reload.plist"];
        
        jl = [jdeamon UTF8String];
        unlink("/tmp/0.reload.plist");
        copyfile(jl, "/tmp/0.reload.plist", 0, COPYFILE_ALL);
        chmod("/tmp/0.reload.plist", 0600);
        chown("/tmp/0.reload.plist", 0, 0);
        
        jl = [jlaunchctl UTF8String];
        unlink("/tmp/reload");
        copyfile(jl, "/tmp/reload", 0, COPYFILE_ALL);
        chmod("/tmp/reload", 0755);
        chown("/tmp/reload", 0, 0);
    }
    
    printf("[*] loading JB\n");
    jl = "/bin/bash";
    posix_spawn(&pd, jl, NULL, NULL, (char **)&(const char*[]){ jl, "-c", "/usr/libexec/dirhelper", NULL }, NULL);
    waitpid(pd, NULL, 0);
    
    usleep(100000);
    
    posix_spawn(&pd, jl, NULL, NULL, (char **)&(const char*[]){ jl, "-c", "launchctl load /tmp/0.reload.plist", NULL }, NULL);
    
}

@interface ViewController ()

@end

@implementation ViewController
@synthesize go_btn;
@synthesize add_btn;
@synthesize box1;
@synthesize box2;
@synthesize box3;
@synthesize box4;
@synthesize box5;

- (void)NotPushed{
    
}

- (void)reloadStart{
    restart();
}

- (void)rebootSystem{
    reboot(0);
    exit(-1);
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    [add_btn setEnabled:NO];
    [add_btn setHidden:YES];
    
    struct stat st;
    struct utsname name;
    uname(&name);
    
    if(!(strstr(name.version, "Darwin Kernel Version 16.7.0: Wed Jul 26 11:08:56 PDT 2017; root:xnu-3789.70.16~21/RELEASE_ARM_S5L8950X"))){ // iPhone 5 - iOS 10.3.4
        [go_btn setTitle:@"Unsupported" forState:UIControlStateDisabled];
        [go_btn setEnabled:NO];
        [box1 setHidden:YES];
        [box2 setHidden:YES];
        [box3 setHidden:YES];
        return;
    }
    
    if(syscall(SYS_stat, "/haxx", &st) != 0){
        // checking haxx dir
        [go_btn setTitle:@"Not jailbroken with p0insettia" forState:UIControlStateNormal];
        [go_btn setEnabled:NO];
        [box2 setHidden:YES];
        [box3 setHidden:YES];
        return;
    }
    
    if(syscall(SYS_stat, "/tmp/.jbd", &st) == 0){
        [go_btn setTitle:@"Already jailbroken?" forState:UIControlStateNormal];
        [go_btn setEnabled:NO];
        [add_btn setEnabled:YES];
        [add_btn setHidden:NO];
        [box1 setHidden:YES];
        [box2 setHidden:YES];
        [box3 setHidden:YES];
        [box4 setHidden:YES];
        return;
    }
    
    if(syscall(SYS_stat, "/.installed_p0insettia", &st) == 0){
        // Normally not accessible.
        [go_btn setTitle:@"Already jailbroken?" forState:UIControlStateNormal];
        [go_btn setEnabled:NO];
        [add_btn setEnabled:YES];
        [add_btn setHidden:NO];
        [box1 setHidden:YES];
        [box2 setHidden:YES];
        [box3 setHidden:YES];
        [box4 setHidden:YES];
        return;
    }
    
    [go_btn setTitle:@"Re-Jailbreak" forState:UIControlStateNormal];
    
    UIAlertController *alertControllerX = [UIAlertController alertControllerWithTitle:waring_title message:waring_txt preferredStyle:UIAlertControllerStyleAlert];
    
    
    [alertControllerX addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        [self NotPushed];
    }]];
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        dispatch_sync( dispatch_get_main_queue(), ^{
            [self presentViewController:alertControllerX animated:YES completion:nil];
        });
    });
    
}


- (IBAction)add_repo:(id)sender {
    UIWebView *webView = [[UIWebView alloc]init];
    webView.delegate = self;
    [self.view addSubview:webView];
    NSURL *url = [NSURL URLWithString:@"cydia://url/https://cydia.saurik.com/api/share#?source=https://dora2ios.github.io/repo/"];
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    [webView loadRequest:request];
    return;
}



- (IBAction)go:(id)sender {
    [go_btn setEnabled:NO];
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0) , ^{
        
        UIAlertController *alertControllerY = [UIAlertController alertControllerWithTitle:reload_title message:reload_txt preferredStyle:UIAlertControllerStyleAlert];
        UIAlertController *alertControllerZ = [UIAlertController alertControllerWithTitle:failed_title message:failed_txt preferredStyle:UIAlertControllerStyleAlert];
        
        dispatch_sync( dispatch_get_main_queue(), ^{
            [self->go_btn setTitle:@"Exploiting sock_port_2" forState:UIControlStateNormal];
        });
        
        // jakejames's sock port
        mach_port_t pt = get_tfp0();
        
        if(pt){
            dispatch_sync( dispatch_get_main_queue(), ^{
                [self->go_btn setTitle:@"Jailbreaking" forState:UIControlStateNormal];
            });
            
            unjail(pt);
            
            dispatch_sync( dispatch_get_main_queue(), ^{
                [self->go_btn setTitle:@"Done!" forState:UIControlStateNormal];
            });
            
            [alertControllerY addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
                [self reloadStart];
            }]];
            dispatch_sync( dispatch_get_main_queue(), ^{
                [self presentViewController:alertControllerY animated:YES completion:nil];
            });
            
            return;
        }
        
        [alertControllerZ addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
            [self rebootSystem];
        }]];
        dispatch_sync( dispatch_get_main_queue(), ^{
            [self presentViewController:alertControllerZ animated:YES completion:nil];
        });
        
        return;
    });
    
    return;
}


@end
