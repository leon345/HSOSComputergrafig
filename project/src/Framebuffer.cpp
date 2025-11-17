//
//  Framebuffer.cpp
//  RealtimeRending
//
//  Created by Philipp Lensing on 03.12.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#include "Framebuffer.h"
#include <assert.h>
#include "texture.h"
#include "rgbimage.h"


Framebuffer::Framebuffer() : m_FramebufferID(0), m_DepthTargetID(0), m_pTexture(NULL), m_Width(0), m_Height(0)
{
    
}
Framebuffer::~Framebuffer()
{

}

bool Framebuffer::create(bool CreateDepthTarget, unsigned int Width, unsigned int Height, bool useTextureForDepth)
{
    glGenFramebuffers(1, &m_FramebufferID);
    
    assert(m_FramebufferID>0);
    
	m_Width = Width;
	m_Height = Height;
    
    if( !CreateDepthTarget)
        return true;

    if (useTextureForDepth)
    {
        // Depth-Texture
        glGenTextures(1, &m_DepthTargetID);
        glBindTexture(GL_TEXTURE_2D, m_DepthTargetID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTargetID, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    } else
    {
        // create and setup depth-renderbufer
        glGenRenderbuffers(1, &m_DepthTargetID);
        glBindRenderbuffer( GL_RENDERBUFFER, m_DepthTargetID );
        glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, Width, Height );
        glBindRenderbuffer( GL_RENDERBUFFER, 0);

        // add depth-renderbuffer to framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthTargetID);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(Status == GL_FRAMEBUFFER_COMPLETE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}


bool Framebuffer::attachColorTarget( const Texture& Tex)
{
    assert(Tex.width() == width() && Tex.height() == height() );
    glBindFramebuffer( GL_FRAMEBUFFER, m_FramebufferID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Tex.ID(), 0);
    glBindFramebuffer( GL_FRAMEBUFFER, 0);
    m_pTexture = &Tex;
    return true;
}

bool Framebuffer::detachColorTarget()
{
    glBindFramebuffer( GL_FRAMEBUFFER, m_FramebufferID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
    glBindFramebuffer( GL_FRAMEBUFFER, 0);
    return true;
}

void Framebuffer::activate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(Status==GL_FRAMEBUFFER_COMPLETE);

    
}
void Framebuffer::deactivate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
