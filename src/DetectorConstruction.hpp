#pragma once

#include "G4VUserDetectorConstruction.hh"


class DetectorConstruction : public G4VUserDetectorConstruction {
public:
    DetectorConstruction(bool checkOverlaps = false) : fCheckOverlaps(checkOverlaps) {}
    virtual ~DetectorConstruction() {}

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

private:
    bool fCheckOverlaps;
};
