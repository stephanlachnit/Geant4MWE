#include <chrono>
#include <iostream>

#include "G4UIExecutive.hh"
#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "FTFP_BERT.hh"
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
#include "G4String.hh"

#include "DetectorConstruction.hpp"
#include "ActionInitialization.hpp"

G4RunManager* prepRunManager(bool MT) {
    G4RunManager* runManager = nullptr;
    if (MT) {
        runManager = new G4MTRunManager();
        runManager->SetNumberOfThreads(1);
    }
    else {
        runManager = new G4RunManager();
    }

    runManager->SetUserInitialization(new DetectorConstruction());
    runManager->SetUserInitialization(new FTFP_BERT());
    runManager->SetUserInitialization(new ActionInitialization());

    // Ensure proper initialization
    runManager->Initialize();
    runManager->BeamOn(0);

    return runManager;
}

std::chrono::milliseconds bench(G4RunManager* runManager, size_t runs) {
    auto start = std::chrono::steady_clock::now();
    for (size_t n = 0; n < runs; ++n) {
        runManager->BeamOn(1);
    }
    auto stop = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
}

int vis(int argc, char** argv) {
    auto* ui = new G4UIExecutive(argc, argv);
    auto* runManager = prepRunManager(true);

    auto* visManager = new G4VisExecutive();
    visManager->Initialize();
    auto* UImanager = G4UImanager::GetUIpointer();

    G4String vis_backend = (argc == 3) ? argv[2] : "OGL";
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
    std::cerr << "Usage: [st, mt, vis [vis_backend]]" << std::endl;
    return 1;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        return errorMessage();
    }
    if (strcmp(argv[1], "vis") == 0) {
        return vis(argc, argv);
    }

    if (strcmp(argv[1], "mt") == 0) {
        std::cout << "Preparing MT Benchmark..." << std::endl;
        auto* runManagerMT = prepRunManager(true);
        std::cout << "Starting MT benchmark..." << std::endl;
        auto result_mt = bench(runManagerMT, 1000);
        std::cout << "Result MT " << result_mt.count() << " ms" << std::endl;
        delete runManagerMT;
        return 0;
    }

    if (strcmp(argv[1], "st") == 0) {
        std::cout << "Preparing ST Benchmark..." << std::endl;
        auto* runManagerST = prepRunManager(false);
        std::cout << "Starting ST benchmark..." << std::endl;
        auto result_st = bench(runManagerST, 1000);
        std::cout << "Result ST " << result_st.count() << " ms" << std::endl;
        delete runManagerST;
        return 0;
    }

    return errorMessage();
}
