#include "G4UIExecutive.hh"
#include "G4RunManagerFactory.hh"
#include "FTFP_BERT.hh"
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
#include "G4String.hh"

#include "DetectorConstruction.hpp"
#include "ActionInitialization.hpp"

int main(int argc, char** argv) {

    G4UIExecutive* ui = nullptr;
    if (argc == 1) {
        ui = new G4UIExecutive(argc, argv);
    }

    auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

    runManager->SetUserInitialization(new DetectorConstruction());
    runManager->SetUserInitialization(new FTFP_BERT());
    runManager->SetUserInitialization(new ActionInitialization());

    auto* visManager = new G4VisExecutive();
    visManager->Initialize();
    auto* UImanager = G4UImanager::GetUIpointer();

    if (!ui) {
        G4String cmd = "/control/execute ";
        UImanager->ApplyCommand(cmd + argv[1]);
    }
    else {
        ui->SessionStart();
    }

    delete ui;
    delete visManager;
    delete runManager;

    return 0;
}
