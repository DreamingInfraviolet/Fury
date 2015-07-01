#include "physics.h"

bool Physics::initialise()
{
	//Creating foundation for PhysX
	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);

	//Creating instance of PhysX SDK
	mPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale());

	if (mPhysicsSDK == NULL)
	{
		Error("Could not create PhysX device.");
		return false;
	}

	//Creating scene
	physx::PxSceneDesc sceneDesc(mPhysicsSDK->getTolerancesScale());	//Descriptor class for scenes 

	sceneDesc.gravity = physx::PxVec3(0.0f, -0.0f, 0.0f);		//Setting gravity
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);	//Creates default CPU dispatcher for the scene
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;	//Creates default collision filter shader for the scene

	mScene = mPhysicsSDK->createScene(sceneDesc);				//Creates a scene 


																//Creating PhysX material
	physx::PxMaterial* material = mPhysicsSDK->createMaterial(0.5, 0.5, 0.5); //Creating a PhysX material

	return true;
}

bool Physics::connectToDebugVisualiser()
{
	// check if PvdConnection manager is available on this platform
	if (mPhysicsSDK->getPvdConnectionManager() == NULL)
		return false;

	// setup connection parameters
	const char*     pvd_host_ip = "127.0.0.1";  // IP of the PC which is running PVD
	int             port = 5425;         // TCP port to connect to, where PVD is listening
	unsigned int    timeout = 100;          // timeout in milliseconds to wait for PVD to respond,
											// consoles and remote PCs need a higher timeout.
	physx::PxVisualDebuggerConnectionFlags connectionFlags = physx::PxVisualDebuggerExt::getAllConnectionFlags();

	// and now try to connect
	physx::debugger::comm::PvdConnection* theConnection = physx::PxVisualDebuggerExt::createConnection(mPhysicsSDK->getPvdConnectionManager(),
		pvd_host_ip, port, timeout, connectionFlags);
	return true;
}

void Physics::shutdown()
{
	mPhysicsSDK->release();			//Removes any actors,  particle systems, and constraint shaders from this scene
	mFoundation->release();			//Destroys the instance of foundation SDK
}

void Physics::startComputation()
{
	mScene->simulate(mTimeStep);	//Advances the simulation by 'gTimeStep' time
}

void Physics::joinComputation()
{
	mScene->fetchResults(true);		//Block until the simulation run is completed
}

void Physics::registerObject(TransformNode * transformNode, Mesh * mesh)
{
	physx::PxBoxGeometry geom(physx::PxVec3(0.3f,0.3f,0.3f));
	physx::PxMaterial* mat = mPhysicsSDK->createMaterial(0.5, 0.5, 0.5);
	physx::PxTransform trans = 
		physx::PxTransform(physx::PxVec3(transformNode->getPosX(), transformNode->getPosY(), transformNode->getPosZ()),
			physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f)));

	physx::PxRigidDynamic* rb = physx::PxCreateDynamic(*mPhysicsSDK,
		trans,
		geom, *mat, 1.0f);		//Creating rigid static actor
																					
	mScene->addActor(*rb);														//Adding box actor to PhysX scene
}
