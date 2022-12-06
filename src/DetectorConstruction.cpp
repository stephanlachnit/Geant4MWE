#include "DetectorConstruction.hpp"

#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"

G4VPhysicalVolume* DetectorConstruction::Construct() {
    auto* nist = G4NistManager::Instance();

    double worldHalfSize = 0.5 * m;
    auto* worldMaterial = nist->FindOrBuildMaterial("G4_AIR");

    auto* solidWorld = new G4Box("World", worldHalfSize, worldHalfSize, worldHalfSize);
    auto* logicWorld = new G4LogicalVolume(solidWorld, worldMaterial, "World");
    auto* physiWolrd = new G4PVPlacement(G4Transform3D(), logicWorld, "World", nullptr, false, 0, fCheckOverlaps);

    return physiWolrd;
}

void DetectorConstruction::ConstructSDandField()
{ }
