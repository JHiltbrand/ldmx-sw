/**
 * @file DisablePhotoNuclear.cxx
 * @class DisablePhotoNuclear
 * @brief Class defining a UserActionPlugin that turns off the photonNuclear processes completely
 * @author Michael Revering, University of Minnesota
 */

#include "SimPlugins/DisablePhotoNuclear.h"
#include "G4ProcessTable.hh"

namespace ldmx {

    extern "C" DisablePhotoNuclear* createDisablePhotoNuclear() {
        return new DisablePhotoNuclear;
    }

    extern "C" void destroyDisablePhotoNuclear(DisablePhotoNuclear* object) {
        delete object;
    }

    DisablePhotoNuclear::DisablePhotoNuclear() {
    }

    DisablePhotoNuclear::~DisablePhotoNuclear() {
    }

    void DisablePhotoNuclear::beginRun(const G4Run* run) {
      std::cout << "The DisablePhotoNuclear PLUGIN is RUNNING!\n";
      G4ProcessTable* ptable = G4ProcessTable::GetProcessTable();
      ptable->SetProcessActivation("photonNuclear",false);
      ptable->SetProcessActivation("electronNuclear",false);
      ptable->SetProcessActivation("positronNuclear",false);
        return;
    }
}

