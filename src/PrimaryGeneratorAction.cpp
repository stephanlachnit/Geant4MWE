#include "PrimaryGeneratorAction.hpp"

#include "G4ParticleTable.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction() {
    auto* particleTable = G4ParticleTable::GetParticleTable();
    auto* particle = particleTable->FindParticle("e-");

    fParticleGun = new G4ParticleGun(particle);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -0.5 * m));
    fParticleGun->SetParticleEnergy(4. * GeV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
    fParticleGun->GeneratePrimaryVertex(event);
}
