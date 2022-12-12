#include "DetectorConstruction.hpp"

#include <string>

#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"
#include "G4VPVParameterisation.hh"
#include "G4PVParameterised.hh"

template<typename T> void intentionallyUnused(const T&) { }

class Parameterization2DG4 : public G4VPVParameterisation {
public:
    /**
     * @brief Construct the parameterization for pixels
     * @param div_x Number of divisions in the x-direction (y is automatically inferred)
     * @param size_x Size of single element in x-direction
     * @param size_y Size of single element in y-direction
     * @param offset_x Offset of grid in the x-direction
     * @param offset_y Offset of grid in the y-direction
     * @param pos_z Position of the 2D parameterization in the z-axis
     */
    Parameterization2DG4(int div_x, double size_x, double size_y, double offset_x, double offset_y, double pos_z)
      : div_x_(div_x), size_x_(size_x), size_y_(size_y), offset_x_(offset_x), offset_y_(offset_y), pos_z_(pos_z) {}

    /**
     * @brief Place the physical volume at the correct place with the copy number
     * @param copy_id Id of the volume on the grid
     * @param phys_volume Physical volume to place on the grid
     */
    void ComputeTransformation(const G4int copy_id, G4VPhysicalVolume* phys_volume) const override {
        auto idx_x = copy_id % div_x_;
        auto idx_y = copy_id / div_x_;

        auto pos_x = (idx_x + 0.5) * size_x_ + offset_x_;
        auto pos_y = (idx_y + 0.5) * size_y_ + offset_y_;

        phys_volume->SetTranslation(G4ThreeVector(pos_x, pos_y, pos_z_));
        phys_volume->SetRotation(nullptr);
    }

private:
    int div_x_;
    double size_x_;
    double size_y_;
    double offset_x_;
    double offset_y_;
    double pos_z_;
};

G4VPhysicalVolume* DetectorConstruction::Construct() {
    auto* nist = G4NistManager::Instance();

    double worldHalfSize = 0.1 * m;
    auto* worldMaterial = nist->FindOrBuildMaterial("G4_AIR");

    auto* solidWorld = new G4Box("World", worldHalfSize, worldHalfSize, worldHalfSize);
    auto* logicWorld = new G4LogicalVolume(solidWorld, worldMaterial, "World");
    auto* physiWolrd = new G4PVPlacement(G4Transform3D(), logicWorld, "World", nullptr, false, 0, fCheckOverlaps);

    // Pixel Detector
    double pixelDetectorSizeXY = 2. * cm;
    double pixelDetectorSizeZ = 100. * um;
    auto* pixelDetectorMaterial = nist->FindOrBuildMaterial("G4_Si");
    int pixelsNumXY = 800;
    double pixelsSizeXY = pixelDetectorSizeXY / pixelsNumXY;

    // Single Pixel Cell (for simplicity only Si without inner structure)
    auto* solidSinglePixel = new G4Box("SinglePixel", 0.5 * pixelsSizeXY, 0.5 * pixelsSizeXY, 0.5 * pixelDetectorSizeZ);
    auto* logicSinglePixel = new G4LogicalVolume(solidSinglePixel, pixelDetectorMaterial, "SinglePixel");

    // Pixel Matrix
    auto* paramPixels = new Parameterization2DG4(pixelsNumXY, pixelsSizeXY, pixelsSizeXY, -0.5 * pixelDetectorSizeXY, -0.5 * pixelDetectorSizeXY, 0.);

    for (size_t n = 0; n < 5; ++n) {
        // Place Outer Pixel Detector Volume
        std::string pixelDetectorName = "PixelDetector" + std::to_string(n);
        auto pixelDetectorTransfrom = G4Transform3D({}, {0.5 * pixelsSizeXY, 0.5 * pixelsSizeXY, n * cm - worldHalfSize});
        auto* solidPixelDetector = new G4Box(pixelDetectorName, 0.5 * pixelDetectorSizeXY, 0.5 * pixelDetectorSizeXY, 0.5 * pixelDetectorSizeZ);
        auto* logicPixelDetector = new G4LogicalVolume(solidPixelDetector, pixelDetectorMaterial, pixelDetectorName);
        auto* physiPixelDetector = new G4PVPlacement(pixelDetectorTransfrom, logicPixelDetector, pixelDetectorName, logicWorld, false, 0, fCheckOverlaps);
        intentionallyUnused(physiPixelDetector);

        // Place Pixels
        std::string pixelsName = "Pixels" + std::to_string(n);
        auto* physiPixels = new G4PVParameterised(pixelsName, logicSinglePixel, logicPixelDetector, EAxis::kUndefined, pixelsNumXY * pixelsNumXY, paramPixels, false);
        intentionallyUnused(physiPixels);
    }

    return physiWolrd;
}

void DetectorConstruction::ConstructSDandField()
{ }
