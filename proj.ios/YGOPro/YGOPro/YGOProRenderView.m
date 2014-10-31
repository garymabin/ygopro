//
//  YGOProRenderView.m
//  YGOPro
//
//  Created by 马彬 on 14-10-25.
//  Copyright (c) 2014年 garymabin@gmail.com. All rights reserved.
//

#import "YGOProRenderView.h"
#import "YGOProRenderEngine.h"

@implementation YGOProRenderView

ygopro::YGOProRenderEngine* mEngine;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        CAEAGLLayer* eaglLayer = (CAEAGLLayer*)super.layer;
        eaglLayer.opaque = YES;
        
        EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
        self.mContext = [[EAGLContext alloc] initWithAPI:api];
        if (!self.mContext || ![EAGLContext setCurrentContext:self.mContext]) {
            return nil;
        } else {
            mEngine = new ygopro::YGOProRenderEngine();
        }
        [self.mContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        
        mEngine->Initialize(CGRectGetWidth(frame), CGRectGetHeight(frame));
        
        [self drawView:nil];
        
        CADisplayLink* displayLink;
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
    return self;
}

- (BOOL) isMultipleTouchEnabled {
    return YES;
}

- (void) drawView:(CADisplayLink *)displayLink {
    mEngine->Render();
    [self.mContext presentRenderbuffer:GL_RENDERBUFFER];
}

+ (Class) layerClass {
    return [CAEAGLLayer class];
}


/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

@end
