#include "SimApplication/AuxInfoReader.h"

// LDMX
#include "SimApplication/CalorimeterSD.h"
#include "SimApplication/EcalSD.h"
#include "SimApplication/HcalSD.h"
#include "SimApplication/MagneticFieldStore.h"
#include "SimApplication/MagneticFieldMap3D.h"
#include "SimApplication/TargetSD.h"
#include "SimApplication/TrackerSD.h"
#include "SimApplication/UserRegionInformation.h"
#include "SimApplication/VisAttributesStore.h"

// Geant4
#include "G4LogicalVolumeStore.hh"
#include "G4SDManager.hh"
#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include "G4GDMLEvaluator.hh"
#include "G4SystemOfUnits.hh"
#include "G4RegionStore.hh"
#include "G4Region.hh"

// STL
#include <string>
#include <stdlib.h>

using std::string;

namespace ldmx {

    AuxInfoReader::AuxInfoReader(G4GDMLParser* theParser) :
            parser_(theParser), eval_(new G4GDMLEvaluator) {
    }

    AuxInfoReader::~AuxInfoReader() {
        delete eval_;
        delete detectorHeader_;
    }

    void AuxInfoReader::readGlobalAuxInfo() {

        const G4GDMLAuxListType* auxInfoList = parser_->GetAuxList();
        for (std::vector<G4GDMLAuxStructType>::const_iterator iaux = auxInfoList->begin(); iaux != auxInfoList->end(); iaux++) {

            G4String auxType = iaux->type;
            G4String auxVal = iaux->value;
            G4String auxUnit = iaux->unit;

            if (auxType == "SensDet") {
                createSensitiveDetector(auxVal, iaux->auxList);
            } else if (auxType == "MagneticField") {
                createMagneticField(auxVal, iaux->auxList);
            } else if (auxType == "Region") {
                createRegion(auxVal, iaux->auxList);
            } else if (auxType == "VisAttributes") {
                createVisAttributes(auxVal, iaux->auxList);
            } else if (auxType == "DetectorVersion") {
                createDetectorHeader(auxVal, iaux->auxList);
            }
        }
        return;
    }

    void AuxInfoReader::createSensitiveDetector(G4String theSensDetName, const G4GDMLAuxListType* auxInfoList) {

        std::cout << "Creating SensitiveDetector " << theSensDetName << std::endl;

        G4String sdType("");
        G4String hcName("");
        int verbose = 0;
        for (std::vector<G4GDMLAuxStructType>::const_iterator iaux = auxInfoList->begin(); iaux != auxInfoList->end(); iaux++) {

            G4String auxType = iaux->type;
            G4String auxVal = iaux->value;
            G4String auxUnit = iaux->unit;

            std::cout << "auxType: " << auxType << ", auxVal: " << auxVal << ", auxUnit: " << auxUnit << std::endl;

            if (auxType == "SensDetType") {
                sdType = auxVal;
            } else if (auxType == "HitsCollection") {
                hcName = auxVal;
            } else if (auxType == "Verbose") {
                verbose = atoi(auxVal.c_str());
            }
        }

        if (sdType == "") {
            G4Exception("", "", FatalException, "The SensDet is missing the SensDetType.");
        }

        if (hcName == "") {
            G4Exception("", "", FatalException, "The SensDet is missing the HitsCollection.");
        }

        /*
         * Build the Sensitive Detector, and re-assign the detID if applicable
         */
        G4VSensitiveDetector* sd = 0;

        if (sdType == "TrackerSD") {
            sd = new TrackerSD(theSensDetName, hcName);
        } else if (sdType == "EcalSD") {
            sd = new EcalSD(theSensDetName, hcName);
        } else if (sdType == "HcalSD") {
            sd = new HcalSD(theSensDetName, hcName);
        } else if (sdType == "CalorimeterSD") {
            sd = new CalorimeterSD(theSensDetName, hcName);
        } else if (sdType == "TargetSD") {
            sd = new TargetSD(theSensDetName, hcName);
        }

        else {
            std::cerr << "Unknown SensitiveDetector type: " << sdType << std::endl;
            G4Exception("", "", FatalException, "Unknown SensitiveDetector type in aux info.");
        }

        // Set verbose level.
        sd->SetVerboseLevel(verbose);

        std::cout << "Created " << sdType << " " << theSensDetName << " with hits collection " << hcName << " and verbose level " << verbose << std::endl << std::endl;
    }

    void AuxInfoReader::assignAuxInfoToVolumes() {
        const G4LogicalVolumeStore* lvs = G4LogicalVolumeStore::GetInstance();
        std::vector<G4LogicalVolume*>::const_iterator lvciter;
        for (lvciter = lvs->begin(); lvciter != lvs->end(); lvciter++) {
            G4GDMLAuxListType auxInfo = parser_->GetVolumeAuxiliaryInformation(*lvciter);
            if (auxInfo.size() > 0) {

                for (std::vector<G4GDMLAuxStructType>::const_iterator iaux = auxInfo.begin(); iaux != auxInfo.end(); iaux++) {

                    G4String auxType = iaux->type;
                    G4String auxVal = iaux->value;
                    G4String auxUnit = iaux->unit;

                    G4LogicalVolume* lv = (*lvciter);

                    if (auxType == "SensDet") {
                        G4String sdName = auxVal;
                        G4VSensitiveDetector* sd = G4SDManager::GetSDMpointer()->FindSensitiveDetector(sdName);
                        if (sd != NULL) {
                            lv->SetSensitiveDetector(sd);
                            std::cout << "Assigned SD " << sd->GetName() << " to " << lv->GetName() << std::endl;
                        } else {
                            std::cout << "Unknown SensDet in volume's auxiliary info: " << sdName << std::endl;
                            G4Exception("", "", FatalException, "The SensDet was not found.  Is it defined in userinfo?");
                        }
                    } else if (auxType == "MagneticField") {
                        G4String magFieldName = auxVal;
                        G4MagneticField* magField = MagneticFieldStore::getInstance()->getMagneticField(magFieldName);
                        if (magField != NULL) {
                            G4FieldManager* mgr = new G4FieldManager(magField);
                            lv->SetFieldManager(mgr, true /* FIXME: hard-coded to force field manager to daughters */);
                            std::cout << "Assigned magnetic field " << magFieldName << " to volume " << lv->GetName() << std::endl;
                        } else {
                            std::cout << "Unknown MagneticField ref in volume's auxiliary info: " << magFieldName << std::endl;
                            G4Exception("", "", FatalException, "The MagneticField was not found.  Is it defined in userinfo?");
                        }
                    } else if (auxType == "Region") {
                        G4String regionName = auxVal;
                        G4Region* region = G4RegionStore::GetInstance()->GetRegion(regionName);
                        if (region != NULL) {
                            region->AddRootLogicalVolume(lv);
                            std::cout << "Added volume " << lv->GetName() << " to region " << regionName << std::endl;
                        } else {
                            std::cerr << "Referenced region " << regionName << " was not found!" << std::endl;
                            G4Exception("", "", FatalException, "The region was not found.  Is it defined in userinfo?");
                        }
                    } else if (auxType == "VisAttributes") {
                        G4String visName = auxVal;
                        G4VisAttributes* visAttributes = VisAttributesStore::getInstance()->getVisAttributes(visName);
                        if (visAttributes != NULL) {
                            lv->SetVisAttributes(visAttributes);
                            std::cout << "Assigned VisAttributes " << visName << " to volume " << lv->GetName() << std::endl;
                        } else {
                            std::cerr << "Referenced VisAttributes " << visName << " was not found!" << std::endl;
                            G4Exception("", "", FatalException, "The VisAttributes was not found.  Is it defined in userinfo?");
                        }
                    }
                }
            }
        }
    }

    void AuxInfoReader::createMagneticField(G4String magFieldName, const G4GDMLAuxListType* auxInfoList) {

        // Find type of the mag field.
        G4String magFieldType("");
        for (std::vector<G4GDMLAuxStructType>::const_iterator iaux = auxInfoList->begin(); iaux != auxInfoList->end(); iaux++) {

            G4String auxType = iaux->type;
            G4String auxVal = iaux->value;

            if (auxType == "MagneticFieldType") {
                magFieldType = auxVal;
                break;
            }
        }

        if (magFieldType == "") {
            G4Exception("", "", FatalException, "Missing MagFieldType for magnetic field definition.");
        }

        G4MagneticField* magField = NULL;

        // Create a uniform mag field using the built-in Geant4 type.
        if (magFieldType == "G4UniformMagField") {
            string::size_type sz;
            double bx, by, bz;
            bx = by = bz = 0.;
            for (std::vector<G4GDMLAuxStructType>::const_iterator iaux = auxInfoList->begin(); iaux != auxInfoList->end(); iaux++) {

                G4String auxType = iaux->type;
                G4String auxVal = iaux->value;
                G4String auxUnit = iaux->unit;

                G4String expr = auxVal + "*" + auxUnit;
                if (auxType == "bx") {
                    bx = eval_->Evaluate(expr);
                } else if (auxType == "by") {
                    by = eval_->Evaluate(expr);
                } else if (auxType == "bz") {
                    bz = eval_->Evaluate(expr);
                }
            }
            G4ThreeVector fieldComponents(bx, by, bz);
            magField = new G4UniformMagField(fieldComponents);

            std::cout << "Created G4UniformMagField " << magFieldName << " with field components " << fieldComponents << std::endl << std::endl;

            // Create a global 3D field map by reading from a data file.
        } else if (magFieldType == "MagneticFieldMap3D") {

            string fileName;
            double offsetX, offsetY, offsetZ;

            for (std::vector<G4GDMLAuxStructType>::const_iterator iaux = auxInfoList->begin(); iaux != auxInfoList->end(); iaux++) {

                G4String auxType = iaux->type;
                G4String auxVal = iaux->value;
                G4String auxUnit = iaux->unit;

                G4String expr = auxVal + "*" + auxUnit;

                if (auxType == "File") {
                    fileName = auxVal;
                } else if (auxType == "OffsetX") {
                    offsetX = eval_->Evaluate(expr);
                } else if (auxType == "OffsetY") {
                    offsetY = eval_->Evaluate(expr);
                } else if (auxType == "OffsetZ") {
                    offsetZ = eval_->Evaluate(expr);
                }
            }

            if (fileName.size() == 0) {
                G4Exception("", "", FatalException, "File info with field data was not provided.");
            }

            // Create new 3D field map.
            G4MagneticField* fieldMap = new MagneticFieldMap3D(fileName.c_str(), offsetX, offsetY, offsetZ);

            // Assign field map as global field.
            G4FieldManager* fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
            if (fieldMgr->GetDetectorField() != nullptr) {
                G4Exception("", "", FatalException, "Global mag field was already assigned.");
            }
            fieldMgr->SetDetectorField(fieldMap);
            fieldMgr->CreateChordFinder(fieldMap);

        } else {
            std::cerr << "Unknown MagFieldType in auxiliary info: " << magFieldType << std::endl;
            G4Exception("", "", FatalException, "Unknown MagFieldType in auxiliary info.");
        }

        MagneticFieldStore::getInstance()->addMagneticField(magFieldName, magField);
    }

    void AuxInfoReader::createRegion(G4String name, const G4GDMLAuxListType* auxInfoList) {

        bool storeTrajectories = true;
        for (std::vector<G4GDMLAuxStructType>::const_iterator iaux = auxInfoList->begin(); iaux != auxInfoList->end(); iaux++) {

            G4String auxType = iaux->type;
            G4String auxVal = iaux->value;
            G4String auxUnit = iaux->unit;

            if (auxType == "StoreTrajectories") {
                if (auxVal == "false") {
                    storeTrajectories = false;
                } else if (auxVal == "true") {
                    storeTrajectories = true;
                }
            }
        }

        G4VUserRegionInformation* regionInfo = new UserRegionInformation(storeTrajectories);
        G4Region* region = new G4Region(name);
        region->SetUserInformation(regionInfo);

        std::cout << "Created new detector region " << region->GetName() << std::endl << std::endl;
    }

    void AuxInfoReader::createVisAttributes(G4String name, const G4GDMLAuxListType* auxInfoList) {

        G4double rgba[4] = {1., 1., 1., 1.};
        G4bool visible = true;
        G4bool dauInvisible = false;
        G4bool forceWireframe = false;
        G4bool forceSolid = false;
        G4double lineWidth = 1.0;
        G4VisAttributes::LineStyle lineStyle = G4VisAttributes::unbroken;

        for (std::vector<G4GDMLAuxStructType>::const_iterator iaux = auxInfoList->begin(); iaux != auxInfoList->end(); iaux++) {

            G4String auxType = iaux->type;
            G4String auxVal = iaux->value;
            G4String auxUnit = iaux->unit;

            if (auxType == "R") {
                rgba[0] = atof(auxVal.c_str());
            } else if (auxType == "G") {
                rgba[1] = atof(auxVal.c_str());
            } else if (auxType == "B") {
                rgba[2] = atof(auxVal.c_str());
            } else if (auxType == "A") {
                rgba[3] = atof(auxVal.c_str());
            } else if (auxType == "Style") {
                if (auxVal == "wireframe") {
                    forceWireframe = true;
                } else if (auxVal == "solid") {
                    forceSolid = true;
                }
            } else if (auxType == "DaughtersInvisible") {
                if (auxVal == "true") {
                    dauInvisible = true;
                } else if (auxVal == "false") {
                    dauInvisible = false;
                }
            } else if (auxType == "Visible") {
                if (auxVal == "true") {
                    visible = true;
                } else if (auxVal == "false") {
                    visible = false;
                }
            } else if (auxType == "LineStyle") {
                if (auxVal == "unbroken") {
                    lineStyle = G4VisAttributes::unbroken;
                } else if (auxVal == "dashed") {
                    lineStyle = G4VisAttributes::dashed;
                } else if (auxVal == "dotted") {
                    lineStyle = G4VisAttributes::dotted;
                }
            } else if (auxType == "LineWidth") {
                lineWidth = atof(auxVal.c_str());
            }
        }

        G4VisAttributes* visAttributes = new G4VisAttributes();
        visAttributes->SetColor(rgba[0], rgba[1], rgba[2], rgba[3]);
        visAttributes->SetVisibility(visible);
        visAttributes->SetDaughtersInvisible(dauInvisible);
        visAttributes->SetForceWireframe(forceWireframe);
        visAttributes->SetForceSolid(forceSolid);
        visAttributes->SetLineWidth(lineWidth);
        visAttributes->SetLineStyle(lineStyle);
        VisAttributesStore::getInstance()->addVisAttributes(name, visAttributes);

        std::cout << "Created VisAttributes " << name << std::endl << (*visAttributes) << std::endl << std::endl;
    }

    void AuxInfoReader::createDetectorHeader(G4String auxValue, const G4GDMLAuxListType* auxInfoList) {

        int detectorVersion = atoi(auxValue.c_str());

        std::string detectorName("");
        std::string author("");
        std::string description("");

        for (std::vector<G4GDMLAuxStructType>::const_iterator iaux = auxInfoList->begin(); iaux != auxInfoList->end(); iaux++) {

            G4String auxType = iaux->type;
            G4String auxVal = iaux->value;
            G4String auxUnit = iaux->unit;

            if (auxType == "DetectorName") {
                detectorName = auxVal;
            } else if (auxType == "Author") {
                author = auxVal;
            } else if (auxType == "Description") {
                description = auxVal;
            }
        }

        detectorHeader_ = new DetectorHeader(detectorName, detectorVersion, description, author);

        std::cout << std::endl;
        std::cout << "Read detector header from userinfo: " << std::endl;
        std::cout << "  DetectorName: " << detectorHeader_->getName() << std::endl;
        std::cout << "  DetectorVersion: " << detectorHeader_->getVersion() << std::endl;
        std::cout << "  Author: " << detectorHeader_->getAuthor() << std::endl;
        std::cout << "  Description: " << detectorHeader_->getDescription() << std::endl;
        std::cout << std::endl;

    }

}
