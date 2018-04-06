/*
 * @file ECalEdepFilter.cxx
 * @class ECalEdepFilter
 * @brief Class defining a UserActionPlugin that allows a user to filter out 
 *        events that exceed an energy deposition threshold in the ECal.
 * @author Josh Hiltbrand, University of Minnesota 
 */

#include "SimPlugins/ECalEdepFilter.h"

SIM_PLUGIN(ldmx, ECalEdepFilter)

namespace ldmx { 

    ECalEdepFilter::ECalEdepFilter() {
        messenger_ = new ECalEdepFilterMessenger(this);
    }

    ECalEdepFilter::~ECalEdepFilter() {
    }

    void ECalEdepFilter::stepping(const G4Step* step) { 

        // Get the track associated with this step.
        G4Track* track = step->GetTrack();

        // Get the volume the particle is in.
        G4VPhysicalVolume* volume = track->GetVolume();
        const G4String& volumeName = volume->GetName();

        // If the particle isn't in the Si of the ECal, don't continue with the processing.
        if (volumeName[0] != 'S' || volumeName[1] != 'i') return;

        // Get the total energy deposited by the step
        double eDep = step->GetTotalEnergyDeposit();
        ecalEdep_ = ecalEdep_ + eDep;

        if (ecalEdep_ > ecalThreshold_) { 

            track->SetTrackStatus(fKillTrackAndSecondaries);
            G4RunManager::GetRunManager()->AbortEvent();
            return;
        }
    }

    void ECalEdepFilter::endEvent(const G4Event*) {

        // Clear running total since event ended
        ecalEdep_ = 0.0;
    }
}
