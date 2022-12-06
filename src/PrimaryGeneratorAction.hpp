#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();

    void GeneratePrimaries(G4Event*) override;

private:
    G4ParticleGun* fParticleGun;
};
