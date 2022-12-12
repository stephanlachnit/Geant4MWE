#pragma once

#include "G4WorkerRunManager.hh"

class WorkerMTRunManager;

class WorkerRunManager : public G4WorkerRunManager {
friend WorkerMTRunManager;
public:
    virtual ~WorkerRunManager() {}
    static WorkerRunManager* GetNewInstanceForThread();
protected:
    WorkerRunManager() = default;
// Override now useless functions
    void DoWork() override {}
    void MergePartialResults() override {}
};
