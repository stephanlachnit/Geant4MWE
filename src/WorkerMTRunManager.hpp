#pragma once

#include "G4MTRunManager.hh"

class WorkerRunManager;

class WorkerMTRunManager : public G4MTRunManager {
friend WorkerRunManager;
public:
    void BeamOn(G4int n_event, const char* macroFile = nullptr, G4int n_select = -1) override;
    void Initialize() override;
    void AbortRun(bool softAbort) override;

// Overwrite a bunch of now useless functions
protected:
    WorkerActionRequest ThisWorkerWaitForNextAction() override { return WorkerActionRequest::UNDEFINED; }
    void CreateAndStartWorkers() override {}
    void NewActionRequest(WorkerActionRequest) override {}
    void RequestWorkersProcessCommandsStack() override {}
    G4bool SetUpAnEvent(G4Event*, long&, long&, long&, G4bool) override { return false; }
    G4int SetUpNEvents(G4Event*, G4SeedsQueue*, G4bool) override { return 0; }
    void TerminateWorkers() override {}
    void ThisWorkerEndEventLoop() override {}
    void ThisWorkerProcessCommandsStackDone() override {}
    void ThisWorkerReady() override {}
    void WaitForEndEventLoopWorkers() override {}
    void WaitForReadyWorkers() override {}

private:
    // WorkerRunManager worker manager that run on each thread.
    static G4ThreadLocal WorkerRunManager* fWorkerRunManager;
};
