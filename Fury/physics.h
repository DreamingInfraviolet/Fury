#pragma once

//-------Loading PhysX libraries (32bit only)----------//
#include <iostream> 
#include <conio.h>
#include <PxPhysicsAPI.h> //Single header file to include all features of PhysX API 
#include "mesh.h"
#include "scene.h"



class Physics
{
	physx::PxPhysics*				mPhysicsSDK = nullptr;
	physx::PxFoundation*			mFoundation = nullptr;			//Instance of singleton foundation SDK class
	physx::PxDefaultErrorCallback	mDefaultErrorCallback;		//Instance of default implementation of the error callback
	physx::PxDefaultAllocator		mDefaultAllocatorCallback;	//Instance of default implementation of the allocator interface required by the SDK

	physx::PxScene*				mScene = nullptr;				//Instance of PhysX Scene				
	physx::PxReal					mTimeStep = 1.0f / 60.0f;		//Time-step value for PhysX simulation 



public:
	bool initialise();
	bool connectToDebugVisualiser(); //only call in debug mode
	void shutdown();
	void startComputation();
	void joinComputation();
	void registerObject(TransformNode* transformNode, Mesh* mesh);
};