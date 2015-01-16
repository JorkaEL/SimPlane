/****************************************************************************************

Copyright (C) 2014 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

#include "SceneContext.h"

#include "SceneCache.h"
#include "DrawScene.h"
#include "glimage.h"
#include "Common/Common.h"

namespace
{

    // Find all poses in this scene.
    void FillPoseArray(FbxScene* pScene, FbxArray<FbxPose*>& pPoseArray)
    {
        const int lPoseCount = pScene->GetPoseCount();

        for (int i=0; i < lPoseCount; ++i)
        {
            pPoseArray.Add(pScene->GetPose(i));
        }
    }

    void PreparePointCacheData(FbxScene* pScene, FbxTime &pCache_Start, FbxTime &pCache_Stop)
    {
        // This function show how to cycle through scene elements in a linear way.
		const int lNodeCount = pScene->GetSrcObjectCount<FbxNode>();
        FbxStatus lStatus;

        for (int lIndex=0; lIndex<lNodeCount; lIndex++)
        {
            FbxNode* lNode = pScene->GetSrcObject<FbxNode>(lIndex);

            if (lNode->GetGeometry()) 
            {
                int i, lVertexCacheDeformerCount = lNode->GetGeometry()->GetDeformerCount(FbxDeformer::eVertexCache);

                // There should be a maximum of 1 Vertex Cache Deformer for the moment
                lVertexCacheDeformerCount = lVertexCacheDeformerCount > 0 ? 1 : 0;

                for (i=0; i<lVertexCacheDeformerCount; ++i )
                {
                    // Get the Point Cache object
                    FbxVertexCacheDeformer* lDeformer = static_cast<FbxVertexCacheDeformer*>(lNode->GetGeometry()->GetDeformer(i, FbxDeformer::eVertexCache));
                    if( !lDeformer ) continue;
                    FbxCache* lCache = lDeformer->GetCache();
                    if( !lCache ) continue;

                    // Process the point cache data only if the constraint is active
                    if (lDeformer->Active.Get())
                    {
                        if (lCache->GetCacheFileFormat() == FbxCache::eMaxPointCacheV2)
                        {
                            // This code show how to convert from PC2 to MC point cache format
                            // turn it on if you need it.
#if 0 
                            if (!lCache->ConvertFromPC2ToMC(FbxCache::eMCOneFile, 
                                FbxTime::GetFrameRate(pScene->GetGlobalTimeSettings().GetTimeMode())))
                            {
                                // Conversion failed, retrieve the error here
                                FbxString lTheErrorIs = lCache->GetStaus().GetErrorString();
                            }
#endif
                        }
                        else if (lCache->GetCacheFileFormat() == FbxCache::eMayaCache)
                        {
                            // This code show how to convert from MC to PC2 point cache format
                            // turn it on if you need it.
                            //#if 0 
                            if (!lCache->ConvertFromMCToPC2(FbxTime::GetFrameRate(pScene->GetGlobalSettings().GetTimeMode()), 0, &lStatus))
                            {
                                // Conversion failed, retrieve the error here
                                FbxString lTheErrorIs = lStatus.GetErrorString();
                            }
                            //#endif
                        }
						

                        // Now open the cache file to read from it
                        if (!lCache->OpenFileForRead(&lStatus))
                        {
                            // Cannot open file 
                            FbxString lTheErrorIs = lStatus.GetErrorString();

                            // Set the deformer inactive so we don't play it back
                            lDeformer->Active = false;
                        }
						else
						{
							// get the start and stop time of the cache
							FbxTime lChannel_Start;
							FbxTime lChannel_Stop;
							int lChannelIndex = lCache->GetChannelIndex(lDeformer->Channel.Get());	
							if(lCache->GetAnimationRange(lChannelIndex, lChannel_Start, lChannel_Stop))
							{
								// get the smallest start time
								if(lChannel_Start < pCache_Start) pCache_Start = lChannel_Start;

								// get the biggest stop time
								if(lChannel_Stop  > pCache_Stop)  pCache_Stop  = lChannel_Stop;
							}
						}
                    }
                }
            }
        }
    }

    // Load a texture file (TGA only now) into GPU and return the texture object name
    bool LoadTextureFromFile(const FbxString & pFilePath, unsigned int & pTextureObject)
    {

      if ((pTextureObject = glimageLoadAndBind(pFilePath.Buffer()))){
	glBindTexture(GL_TEXTURE_2D, pTextureObject);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
      }
      return false;
    }

    // Bake node attributes and materials under this node recursively.
    // Currently only mesh, light and material.
    void LoadCacheRecursive(FbxNode * pNode, FbxAnimLayer * pAnimLayer)
    {
        // Bake material and hook as user data.
        const int lMaterialCount = pNode->GetMaterialCount();
        for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex)
        {
            FbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lMaterialIndex);
            if (lMaterial && !lMaterial->GetUserDataPtr())
            {
                FbxAutoPtr<MaterialCache> lMaterialCache(new MaterialCache);
                if (lMaterialCache->Initialize(lMaterial))
                {
                    lMaterial->SetUserDataPtr(lMaterialCache.Release());
                }
            }
        }

        FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
        if (lNodeAttribute)
        {
            // Bake mesh as VBO(vertex buffer object) into GPU.
            if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
            {
                FbxMesh * lMesh = pNode->GetMesh();
                if (lMesh && !lMesh->GetUserDataPtr())
                {
                    FbxAutoPtr<VBOMesh> lMeshCache(new VBOMesh);
                    if (lMeshCache->Initialize(lMesh))
                    {
                        lMesh->SetUserDataPtr(lMeshCache.Release());
                    }
                }
            }
        }

        const int lChildCount = pNode->GetChildCount();
        for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
        {
            LoadCacheRecursive(pNode->GetChild(lChildIndex), pAnimLayer);
        }
    }

    // Unload the cache and release the memory under this node recursively.
    void UnloadCacheRecursive(FbxNode * pNode)
    {
        // Unload the material cache
        const int lMaterialCount = pNode->GetMaterialCount();
        for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex)
        {
            FbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lMaterialIndex);
            if (lMaterial && lMaterial->GetUserDataPtr())
            {
                MaterialCache * lMaterialCache = static_cast<MaterialCache *>(lMaterial->GetUserDataPtr());
                lMaterial->SetUserDataPtr(NULL);
                delete lMaterialCache;
            }
        }

        FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
        if (lNodeAttribute)
        {
            // Unload the mesh cache
            if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
            {
                FbxMesh * lMesh = pNode->GetMesh();
                if (lMesh && lMesh->GetUserDataPtr())
                {
                    VBOMesh * lMeshCache = static_cast<VBOMesh *>(lMesh->GetUserDataPtr());
                    lMesh->SetUserDataPtr(NULL);
                    delete lMeshCache;
                }
            }
        }

        const int lChildCount = pNode->GetChildCount();
        for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
        {
            UnloadCacheRecursive(pNode->GetChild(lChildIndex));
        }
    }

    // Bake node attributes and materials for this scene and load the textures.
    void LoadCacheRecursive(FbxScene * pScene, FbxAnimLayer * pAnimLayer, const char * pFbxFileName)
    {
        // Load the textures into GPU, only for file texture now
        const int lTextureCount = pScene->GetTextureCount();
        for (int lTextureIndex = 0; lTextureIndex < lTextureCount; ++lTextureIndex)
        {
            FbxTexture * lTexture = pScene->GetTexture(lTextureIndex);
            FbxFileTexture * lFileTexture = FbxCast<FbxFileTexture>(lTexture);
            if (lFileTexture && !lFileTexture->GetUserDataPtr())
            {
                // Try to load the texture from absolute path
                const FbxString lFileName = lFileTexture->GetFileName();

                GLuint lTextureObject = 0;
                bool lStatus = LoadTextureFromFile(lFileName, lTextureObject);

                const FbxString lAbsFbxFileName = FbxPathUtils::Resolve(pFbxFileName);
                const FbxString lAbsFolderName = FbxPathUtils::GetFolderName(lAbsFbxFileName);
                if (!lStatus)
                {
                    // Load texture from relative file name (relative to FBX file)
                    const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, lFileTexture->GetRelativeFileName());
                    lStatus = LoadTextureFromFile(lResolvedFileName, lTextureObject);
                }

                if (!lStatus)
                {
                    // Load texture from file name only (relative to FBX file)
                    const FbxString lTextureFileName = FbxPathUtils::GetFileName(lFileName);
                    const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, lTextureFileName);
                    lStatus = LoadTextureFromFile(lResolvedFileName, lTextureObject);
                }

                if (lStatus)
                {
                    GLuint * lTextureName = new GLuint(lTextureObject);
                    lFileTexture->SetUserDataPtr(lTextureName);
                }
            }
        }

        LoadCacheRecursive(pScene->GetRootNode(), pAnimLayer);
    }

    // Unload the cache and release the memory fro this scene and release the textures in GPU
    void UnloadCacheRecursive(FbxScene * pScene)
    {
        const int lTextureCount = pScene->GetTextureCount();
        for (int lTextureIndex = 0; lTextureIndex < lTextureCount; ++lTextureIndex)
        {
            FbxTexture * lTexture = pScene->GetTexture(lTextureIndex);
            FbxFileTexture * lFileTexture = FbxCast<FbxFileTexture>(lTexture);
            if (lFileTexture && lFileTexture->GetUserDataPtr())
            {
                GLuint * lTextureName = static_cast<GLuint *>(lFileTexture->GetUserDataPtr());
                lFileTexture->SetUserDataPtr(NULL);
                glDeleteTextures(1, lTextureName);
                delete lTextureName;
            }
        }

        UnloadCacheRecursive(pScene->GetRootNode());
    }
}

SceneContext::SceneContext(const char * pFileName)
  : mFileName(pFileName), mStatus(UNLOADED), mSdkManager(NULL), mScene(NULL), mImporter(NULL), mCurrentAnimLayer(NULL), mPoseIndex(-1)
{

   // Create the FBX SDK manager which is the object allocator for almost 
   // all the classes in the SDK and create the scene.
   InitializeSdkObjects(mSdkManager, mScene);

   if (mSdkManager) {
       // Create the importer.
     int lFileFormat = -1;
     mImporter = FbxImporter::Create(mSdkManager,"");
     if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(mFileName, lFileFormat)) {
       // Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
       lFileFormat = mSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription( "FBX binary (*.fbx)" );;
       }

     // Initialize the importer by providing a filename.
     if(mImporter->Initialize(mFileName, lFileFormat) == true) {
       mStatus = MUST_BE_LOADED;
     }
     else 
       fprintf(stderr, "Pb %s\n", mImporter->GetStatus().GetErrorString());
   }
}

SceneContext::~SceneContext() {
    FbxArrayDelete(mAnimStackNameArray);

    // Unload the cache and free the memory
    if (mScene) {
      UnloadCacheRecursive(mScene);
    }

    // Delete the FBX SDK manager. All the objects that have been allocated 
    // using the FBX SDK manager and that haven't been explicitly destroyed 
    // are automatically destroyed at the same time.
    DestroySdkObjects(mSdkManager, true);
}

bool SceneContext::LoadFile() {
    bool lResult = false;
    // Make sure that the scene is ready to load.
    if (mStatus == MUST_BE_LOADED) {
      if (mImporter->Import(mScene) == true) {
	// Set the scene status flag to refresh 
	// the scene in the first timer callback.
	mStatus = MUST_BE_REFRESHED;
	
	// Convert Axis System to what is used in this example, if needed
	FbxAxisSystem SceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
	if( SceneAxisSystem != OurAxisSystem ) {
	  OurAxisSystem.ConvertScene(mScene);
	}

	// Convert Unit System to what is used in this example, if needed
	FbxSystemUnit SceneSystemUnit = mScene->GetGlobalSettings().GetSystemUnit();
	if( SceneSystemUnit.GetScaleFactor() != 1.0 ) {
	  //The unit in this example is centimeter.
	  FbxSystemUnit::cm.ConvertScene( mScene);
	}

	// Get the list of all the animation stack.
	mScene->FillAnimStackNameArray(mAnimStackNameArray);

	// Convert mesh, NURBS and patch into triangle mesh
	FbxGeometryConverter lGeomConverter(mSdkManager);
	lGeomConverter.Triangulate(mScene, true);
	
	// Split meshes per material, so that we only have one material per mesh (for VBO support)
	lGeomConverter.SplitMeshesPerMaterial(mScene, true);

	// Bake the scene for one frame
	LoadCacheRecursive(mScene, mCurrentAnimLayer, mFileName);
	
	// Convert any .PC2 point cache data into the .MC format for 
	// vertex cache deformer playback.
	PreparePointCacheData(mScene, mCache_Start, mCache_Stop);
	
	// Get the list of pose in the scene
	FillPoseArray(mScene, mPoseArray);
	// Initialize the frame period.
	mFrameTime.SetTime(0, 0, 0, 1, 0, mScene->GetGlobalSettings().GetTimeMode());
	
	lResult = true;
      }
      else {
	// Import failed, set the scene status flag accordingly.
	mStatus = UNLOADED;
	fprintf(stderr, "Pb %s\n", mImporter->GetStatus().GetErrorString());
      }
      
      // Destroy the importer to release the file.
      mImporter->Destroy();
      mImporter = NULL;
    }
    return lResult;
}

bool SceneContext::SetCurrentAnimStack(int pIndex)
{
    const int lAnimStackCount = mAnimStackNameArray.GetCount();
    if (!lAnimStackCount || pIndex >= lAnimStackCount) {
      return false;
    }

    // select the base layer from the animation stack
   FbxAnimStack * lCurrentAnimationStack = mScene->FindMember<FbxAnimStack>(mAnimStackNameArray[pIndex]->Buffer());
   if (lCurrentAnimationStack == NULL) {
     // this is a problem. The anim stack should be found in the scene!
     return false;
   }

   // we assume that the first animation layer connected to the animation stack is the base layer
   // (this is the assumption made in the FBXSDK)
   mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
   mScene->SetCurrentAnimationStack(lCurrentAnimationStack);

   FbxTakeInfo* lCurrentTakeInfo = mScene->GetTakeInfo(*(mAnimStackNameArray[pIndex]));
   if (lCurrentTakeInfo)  {
     mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
     mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
   }
   else  {
     // Take the time line value
     FbxTimeSpan lTimeLineTimeSpan;
     mScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);
     
     mStart = lTimeLineTimeSpan.GetStart();
     mStop  = lTimeLineTimeSpan.GetStop();
   }

   // check for smallest start with cache start
   if(mCache_Start < mStart)
	   mStart = mCache_Start;

   // check for biggest stop with cache stop
   if(mCache_Stop  > mStop) mStop  = mCache_Stop;

   // move to beginning
   mCurrentTime = mStart;

   // Set the scene status flag to refresh 
   // the scene in the next timer callback.
   mStatus = MUST_BE_REFRESHED;
   
   return true;
}

void SceneContext::OnTimerClick() const
{
    // Loop in the animation stack if not paused.
  if (mStop > mStart) {
    // Set the scene status flag to refresh 
    // the scene in the next timer callback.
    mStatus = MUST_BE_REFRESHED;
    
    mCurrentTime += mFrameTime;
    
    if (mCurrentTime > mStop) {
      mCurrentTime = mStart;
    }
  }
  // Avoid displaying the same frame on 
  // and on if the animation stack has no length.
  else {
    // Set the scene status flag to avoid refreshing 
    // the scene in the next timer callback.
    mStatus = REFRESHED;
  }
}

// Redraw the scene
bool SceneContext::OnDisplay() {
  // Test if the scene has been loaded yet.
  if (mStatus != UNLOADED && mStatus != MUST_BE_LOADED) {
    glPushAttrib(GL_ENABLE_BIT);
    glPushAttrib(GL_LIGHTING_BIT);
    glEnable(GL_DEPTH_TEST);
    // Draw the front face only, except for the texts and lights.
    glEnable(GL_CULL_FACE);
    
    FbxPose * lPose = NULL;
    if (mPoseIndex != -1) {
      lPose = mScene->GetPose(mPoseIndex);
    }

    // If one node is selected, draw it and its children.
    FbxAMatrix lDummyGlobalPosition;
        
    DrawNodeRecursive(mScene->GetRootNode(), mCurrentTime, mCurrentAnimLayer, 
		      lDummyGlobalPosition, lPose);

    glPopAttrib();
    glPopAttrib();
  }

  
  return true;
}
