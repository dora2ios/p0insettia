//
//  ViewController.h
//  reloader
//
//  Created by sakuRdev on 2021/11/18.
//  Copyright (c) 2021 sakuRdev. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController

@property (weak, nonatomic) IBOutlet UIButton *go_btn;
@property (weak, nonatomic) IBOutlet UIButton *add_btn;

@property (weak, nonatomic) IBOutlet UILabel *box1;
@property (weak, nonatomic) IBOutlet UILabel *box2;
@property (weak, nonatomic) IBOutlet UILabel *box3;
@property (weak, nonatomic) IBOutlet UILabel *box4;
@property (weak, nonatomic) IBOutlet UILabel *box5;


@end

NSString* waring_title = @"Warning";
NSString* waring_txt = @"This app will rejailbreak devices that has already been jailbroken with p0insettia using sock_port_2 exploit. All are at your own risk.";

NSString* reload_title = @"Jailbreak Successful?";
NSString* reload_txt = @"Press OK to reload system.";

NSString* failed_title = @"Failed to Jailbreak!";
NSString* failed_txt = @"Press OK to exit system.";
