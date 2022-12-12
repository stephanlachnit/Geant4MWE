#include "WorkerMTRunManager.hpp"

#include <thread>
#include <future>

#include "WorkerRunManager.hpp"

G4ThreadLocal WorkerRunManager* WorkerMTRunManager::fWorkerRunManager = nullptr;

void WorkerMTRunManager::BeamOn(G4int n_event, const char* macroFile, G4int n_select) {
    // If we don't have a worker yet just call parent BeamOn (e.g. on init fake run)
    fWorkerRunManager == nullptr ? G4MTRunManager::BeamOn(n_event, macroFile, n_select)
                                 : fWorkerRunManager->BeamOn(n_event, macroFile, n_select);
}

void WorkerMTRunManager::Initialize() {
    G4MTRunManager::Initialize();

    // Initialize worker run manager
    if (fWorkerRunManager == nullptr) {
        // Needs to called from different thread, else Run0031 exception
        auto instance = std::async(std::launch::async, WorkerRunManager::GetNewInstanceForThread);
        fWorkerRunManager = instance.get();
    }
}

void WorkerMTRunManager::AbortRun(bool softAbort) {
    fWorkerRunManager == nullptr ? G4MTRunManager::AbortRun(softAbort)
                                 : fWorkerRunManager->AbortRun(softAbort);
}
