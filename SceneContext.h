/****************************************************************************************

Copyright (C) 2014 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

#ifndef _SCENE_CONTEXT_H
#define _SCENE_CONTEXT_H


#if defined(__APPLE__) || defined(MACOSX)
# include <OpenGL/gl.h> 
#else
#define  GL_GLEXT_PROTOTYPES
# include <GL/gl.h>
#endif

#include <fbxsdk.h>


// This class is responsive for loading files and recording current status as
// a bridge between window system such as GLUT or Qt and a specific FBX scene.
class SceneContext
{
public:
    enum Status
    {
        UNLOADED,               // Unload file or load failure;
        MUST_BE_LOADED,         // Ready for loading file;
        MUST_BE_REFRESHED,      // Something changed and redraw needed;
        REFRESHED               // No redraw needed.
    };
    Status GetStatus() const { return mStatus; }

    // Initialize with a .FBX, .DAE or .OBJ file name and current window size.
    SceneContext(const char * pFileName);
    ~SceneContext();

    // Return the FBX scene for more informations.
    const FbxScene * GetScene() const { return mScene; }
    // Load the FBX or COLLADA file into memory.
    bool LoadFile();

    // The time period for one frame.
    const FbxTime GetFrameTime() const { return mFrameTime; }

    // Call this method when redraw is needed.
    bool OnDisplay();
    void OnTimerClick() const;

    // Get all the animation stack names in current scene.
    const FbxArray<FbxString *> & GetAnimStackNameArray() const { return mAnimStackNameArray; }
    // Get all the pose in current scene.
    const FbxArray<FbxPose *> & GetPoseArray() const { return mPoseArray; }

    // The input index is corresponding to the array returned from GetAnimStackNameArray.
    bool SetCurrentAnimStack(int pIndex);
    // The input index is corresponding to the array returned from GetPoseArray.
    bool SetCurrentPoseIndex(int pPoseIndex);

private:

    const char * mFileName;
    mutable Status mStatus;
    FbxManager * mSdkManager;
    FbxScene * mScene;
    FbxImporter * mImporter;
    FbxAnimLayer * mCurrentAnimLayer;

    int mPoseIndex;
    FbxArray<FbxString*> mAnimStackNameArray;
    FbxArray<FbxPose*> mPoseArray;

    mutable FbxTime mFrameTime, mStart, mStop, mCurrentTime;
    mutable FbxTime mCache_Start, mCache_Stop;

};


#endif // _SCENE_CONTEXT_H

