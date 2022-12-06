#pragma once

#include "G4VUserDetectorConstruction.hh"


class DetectorConstruction : public G4VUserDetectorConstruction {
public:
    DetectorConstruction() {}
    virtual ~DetectorConstruction() {}

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

private:
    bool fCheckOverlaps = true;
};
