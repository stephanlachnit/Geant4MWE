#include <chrono>
#include <iostream>

#include "G4UIExecutive.hh"
#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "FTFP_BERT.hh"
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
#include "G4String.hh"
#include "G4ProcessTable.hh"
#include "G4EmParameters.hh"
#include "G4HadronicProcessStore.hh"
#include "G4HadronicParameters.hh"
#include "G4NuclearLevelData.hh"

#include "DetectorConstruction.hpp"
#include "ActionInitialization.hpp"

constexpr size_t runs = 100;

template<class RunManager>
std::chrono::milliseconds prepRunManager(RunManager*& runManager, bool checkOverlaps = false) {
    auto start = std::chrono::steady_clock::now();
    runManager = new RunManager();
    runManager->SetNumberOfThreads(1);

    auto* physicsList = new FTFP_BERT();
    runManager->SetUserInitialization(new DetectorConstruction(checkOverlaps));
    runManager->SetUserInitialization(physicsList);
    runManager->SetUserInitialization(new ActionInitialization());

    physicsList->SetVerboseLevel(0);
    G4ProcessTable::GetProcessTable()->SetVerboseLevel(0);
    G4EmParameters::Instance()->SetVerbose(0);
    G4HadronicProcessStore::Instance()->SetVerbose(0);
    G4HadronicParameters::Instance()->SetVerboseLevel(0);
    G4NuclearLevelData::GetInstance()->GetParameters()->SetVerbose(0);

    // Ensure proper initialization
    runManager->Initialize();
    runManager->BeamOn(0);

    auto stop = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
}

template<class RunManager>
std::chrono::milliseconds bench(RunManager* runManager, size_t loops, size_t events_per_loop) {
    auto start = std::chrono::steady_clock::now();
    for (size_t n = 0; n < loops; ++n) {
        runManager->BeamOn(events_per_loop);
    }
    auto stop = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
}

template<class RunManager>
void run_full_bench(size_t events_per_loop) {
    std::cout << "Preparing Benchmark..." << std::endl;
    size_t loops = runs / events_per_loop;
    RunManager* runManager = nullptr;
    auto duration_prep = prepRunManager(runManager);

    std::cout << "Starting benchmark with " << events_per_loop << " per loop (total: " << runs << ")" << std::endl;
    auto duration_bench = bench(runManager, loops, events_per_loop);
    std::cout << "Result Prep:  " << duration_prep.count() << " ms" << "\n"
              << "Result Bench: " << duration_bench.count() << " ms" << std::endl;

    delete runManager;
}

void run_full_bench(bool mt, size_t splits) {
    if (mt) {
        run_full_bench<G4MTRunManager>(splits);
    }
    else {
        run_full_bench<G4RunManager>(splits);
    }
}

int vis(int argc, char** argv) {
    auto* ui = new G4UIExecutive(argc, argv);
    G4MTRunManager* runManager = nullptr;
    prepRunManager(runManager);

    auto* visManager = new G4VisExecutive();
    visManager->Initialize();
    auto* UImanager = G4UImanager::GetUIpointer();

    G4String vis_backend = argv[2];
    UImanager->ApplyCommand("/run/initialize");
    UImanager->ApplyCommand("/vis/open " + vis_backend);
    UImanager->ApplyCommand("/vis/drawVolume");
    UImanager->ApplyCommand("/vis/scene/add/trajectories smooth");
    UImanager->ApplyCommand("/vis/modeling/trajectories/create/drawByCharge");
    ui->SessionStart();

    delete ui;
    delete visManager;
    delete runManager;

    return 0;
}

int errorMessage() {
    std::cerr << "Usage: Benchmark: [st, mt] [splits]" << "\n"
              << "       Visualization: [vis] [vis_backend]" << "\n"
              << " Note: splits needs to divide " << runs << " without reminder." << std::endl;
    return 1;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        return errorMessage();
    }
    if (strcmp(argv[1], "vis") == 0) {
        return vis(argc, argv);
    }

    bool mt;
    if (strcmp(argv[1], "mt") == 0) {
        mt = true;
        std::cout << "Using multi-threaded RunManager" << std::endl;
    }
    else if (strcmp(argv[1], "st") == 0) {
        mt = false;
        std::cout << "Using normal RunManager" << std::endl;
    }
    else {
        return errorMessage();
    }

    int splits = std::atoi(argv[2]);
    if (splits <= 0 || runs % splits != 0) {
        return errorMessage();
    }

    run_full_bench(mt, static_cast<size_t>(splits));

    return 0;
}
