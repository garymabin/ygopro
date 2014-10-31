//
//  YGOProRenderView.h
//  YGOPro
//
//  Created by 马彬 on 14-10-25.
//  Copyright (c) 2014年 garymabin@gmail.com. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>

@interface YGOProRenderView : UIView

@property (nonatomic, retain)UIViewController* mViewController;

@property (nonatomic, retain)EAGLContext* mContext;

- (void) drawView: (CADisplayLink*) displayLink;

@end
