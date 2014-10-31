//
//  AppDelegate.h
//  YGOPro
//
//  Created by 马彬 on 14-10-25.
//  Copyright (c) 2014年 garymabin@gmail.com. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "YGOProRenderView.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (strong, nonatomic) YGOProRenderView *view;

@property (readonly, strong, nonatomic) NSManagedObjectContext *managedObjectContext;
@property (readonly, strong, nonatomic) NSManagedObjectModel *managedObjectModel;
@property (readonly, strong, nonatomic) NSPersistentStoreCoordinator *persistentStoreCoordinator;

- (void)saveContext;
- (NSURL *)applicationDocumentsDirectory;

@end
