#ifndef _DRAW_SCENE_H
#define _DRAW_SCENE_H

#include <fbxsdk.h>

void DrawNodeRecursive(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer, 
                       FbxAMatrix& pParentGlobalPosition, FbxPose* pPose);

#endif // #ifndef _DRAW_SCENE_H






