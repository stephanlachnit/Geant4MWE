#include "WorkerRunManager.hpp"

#include <atomic>

#include "G4MTRunManager.hh"
#include "G4UImanager.hh"
#include "G4UserWorkerInitialization.hh"
#include "G4UserWorkerThreadInitialization.hh"
#include "G4WorkerThread.hh"
#include "G4VUserActionInitialization.hh"

std::atomic_int counter; // counter to give each worker a unique id

WorkerRunManager* WorkerRunManager::GetNewInstanceForThread() {
    // from https://gitlab.cern.ch/allpix-squared/allpix-squared/-/blob/b0d5ec6056bf230691ca27e0f8969cd41bc8f22f/src/tools/geant4/WorkerRunManager.cpp

    WorkerRunManager* thread_run_manager = nullptr;
    G4MTRunManager* master_run_manager = G4MTRunManager::GetMasterRunManager();

    // Step-0: Thread ID
    // Initliazie per-thread stream-output
    // The following line is needed before we actually do I/O initialization
    // because the constructor of UI manager resets the I/O destination.
    G4int this_id = counter.fetch_add(1);
    G4Threading::G4SetThreadId(this_id);
    G4UImanager::GetUIpointer()->SetUpForAThread(this_id);

    // Step-1: Random number engine
    // RNG Engine needs to be initialized by "cloning" the master one.
    const CLHEP::HepRandomEngine* master_engine = master_run_manager->getMasterRandomEngine();
    master_run_manager->GetUserWorkerThreadInitialization()->SetupRNGEngine(master_engine);

    // Step-2: Initialize worker thread
    if(master_run_manager->GetUserWorkerInitialization() != nullptr) {
        master_run_manager->GetUserWorkerInitialization()->WorkerInitialize();
    }

    // Now initialize worker part of shared objects (geometry/physics)
    G4WorkerThread::BuildGeometryAndPhysicsVector();

    // create the new instance
    thread_run_manager = new WorkerRunManager();

    // Step-3: Setup worker run manager
    // Set the detector and physics list to the worker thread. Share with master
    const G4VUserDetectorConstruction* detector = master_run_manager->GetUserDetectorConstruction();

    thread_run_manager->G4RunManager::SetUserInitialization(const_cast<G4VUserDetectorConstruction*>(detector)); // NOLINT

    const G4VUserPhysicsList* physicslist = master_run_manager->GetUserPhysicsList();
    thread_run_manager->SetUserInitialization(const_cast<G4VUserPhysicsList*>(physicslist)); // NOLINT

    // Step-4: Initialize worker run manager
    if(master_run_manager->GetUserActionInitialization() != nullptr) {
        master_run_manager->GetNonConstUserActionInitialization()->Build();
    }

    if(master_run_manager->GetUserWorkerInitialization() != nullptr) {
        master_run_manager->GetUserWorkerInitialization()->WorkerStart();
    }

    thread_run_manager->Initialize();

    // Execute UI commands stored in the masther UI manager
    std::vector<G4String> cmds = master_run_manager->GetCommandStack();
    G4UImanager* uimgr = G4UImanager::GetUIpointer();        // TLS instance
    std::vector<G4String>::const_iterator it = cmds.begin(); // NOLINT
    for(; it != cmds.end(); it++) {
        uimgr->ApplyCommand(*it);
    }

    return thread_run_manager;
}
