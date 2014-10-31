//
//  YGOProRenderEngine.h
//  YGOPro
//
//  Created by 马彬 on 14-10-25.
//  Copyright (c) 2014年 garymabin@gmail.com. All rights reserved.
//

#ifndef __YGOPro__YGOProRenderEngine__
#define __YGOPro__YGOProRenderEngine__

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
namespace ygopro {

class YGOProRenderEngine {
public:
    YGOProRenderEngine();
    void Initialize(int width, int height);
    void Render() const;
};
    
}

#endif /* defined(__YGOPro__YGOProRenderEngine__) */
