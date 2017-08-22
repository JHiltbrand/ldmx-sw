/*
 * @file TargetRecoilFilter.cxx
 * @class TargetRecoilFilter
 * @brief Class defining a UserActionPlugin that allows a user to filter out 
 *        events that don't result in a brem within the target.
 * @author Omar Moreno, SLAC National Accelerator Laboratory
 */

#include "Biasing/TargetRecoilFilter.h"

namespace ldmx { 

    extern "C" TargetRecoilFilter* createTargetRecoilFilter() {
        return new TargetRecoilFilter;
    }

    extern "C" void destroyTargetRecoilFilter(TargetRecoilFilter* object) {
        delete object;
    }

    TargetRecoilFilter::TargetRecoilFilter() {
        //messenger_ = new TargetRecoilFilterMessenger(this);
    }

    TargetRecoilFilter::~TargetRecoilFilter() {
    }

    G4ClassificationOfNewTrack TargetRecoilFilter::stackingClassifyNewTrack(
            const G4Track* track, 
            const G4ClassificationOfNewTrack& currentTrackClass) {

        /*std::cout << "********************************" << std::endl;*/ 
        /*std::cout << "*   Track pushed to the stack  *" << std::endl;*/
        /*std::cout << "********************************" << std::endl;*/

        // get the PDGID of the track.
        G4int pdgID = track->GetParticleDefinition()->GetPDGEncoding();

        // Get the particle type.
        G4String particleName = track->GetParticleDefinition()->GetParticleName();

        /*std::cout << "[ TargetRecoilFilter ]: " << "\n" 
                    << "\tParticle " << particleName << " ( PDG ID: " << pdgID << " ) : " << "\n"
                    << "\tTrack ID: " << track->GetTrackID() << "\n" 
                    << std::endl;*/


        // Use current classification by default so values from other plugins are not overridden.
        G4ClassificationOfNewTrack classification = currentTrackClass;

        if (track->GetTrackID() == 1 && pdgID == 11) {
            ///*std::cout << "[ TargetRecoilFilter ]: Pushing track to waiting stack." << std::endl;*/
            return fWaiting; 
        }

        return classification;
    }

    void TargetRecoilFilter::stepping(const G4Step* step) { 

        // Get the track associated with this step.
        G4Track* track = step->GetTrack();

        // Only process the primary electron track
        if (track->GetParentID() != 0) return;

        // get the PDGID of the track.
        G4int pdgID = track->GetParticleDefinition()->GetPDGEncoding();
        
        // Make sure that the particle being processed is an electron.
        if (pdgID != 11) return; // Throw an exception

        // Get the volume the particle is in.
        G4VPhysicalVolume* volume = track->GetVolume();
        G4String volumeName = volume->GetName();

        // Get the particle type.
        G4String particleName = track->GetParticleDefinition()->GetParticleName();

        // Get the kinetic energy of the particle.
        //double incidentParticleEnergy = step->GetPostStepPoint()->GetTotalEnergy();
        /*std::cout << "*******************************" << std::endl;*/ 
        /*std::cout << "*   Step " << track->GetCurrentStepNumber() << std::endl;*/
        /*std::cout << "********************************" << std::endl;*/

        /*std::cout << "[ TargetRecoilFilter ]: " << "\n" 
                    << "\tTotal energy of " << particleName      << " ( PDG ID: " << pdgID
                    << " ) : " << incidentParticleEnergy       << "\n"
                    << "\tTrack ID: " << track->GetTrackID()     << "\n" 
                    << "\tStep #: " << track->GetCurrentStepNumber() << "\n"
                    << "\tParticle currently in " << volumeName  
                    << "\tPost step process: " << step->GetPostStepPoint()->GetStepStatus() 
                    << std::endl;*/
 
        // If the particle isn't in the target, don't continue with the processing.
        if (volumeName.compareTo(volumeName_) != 0) return;

        // Check if the particle is exiting the volume.
        if (step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) { 
           
            std::cout << "[ TargetRecoilFilter ]: "
                        << "Particle " << particleName << "is leaving the "
                        << volumeName << " volume with momentum "
                        << track->GetMomentum().mag() << std::endl;
            
            if (track->GetMomentum().mag() >= recoilEnergyThreshold_) { 
                std::cout << "[ TargetRecoilFilter ]: "
                            << "Electron energy is above threshold --> Aborting event."
                            << std::endl;
                
                track->SetTrackStatus(fKillTrackAndSecondaries);
                G4RunManager::GetRunManager()->AbortEvent();
                return;
            
            }

            // Get the particles daughters.
            const G4TrackVector* secondaries = step->GetSecondary();
           
            /*std::cout << "[ TargetRecoilFilter ]: "
                        << "Incident " << particleName  << " produced " << secondaries->size() 
                        << " secondaries." << std::endl;*/
           
            // If the particle didn't produce any secondaries, stop processing
            // the event.
            if (secondaries->size() == 0) { 
                /*std::cout << "[ TargetRecoilFilter ]: "
                            << "Primary did not produce secondaries --> Killing primary track!" 
                            << std::endl;*/
                
                track->SetTrackStatus(fKillTrackAndSecondaries);
                G4RunManager::GetRunManager()->AbortEvent();
                return;
            }
                
            track->SetTrackStatus(fSuspend);  
       
        } else if (step->GetPostStepPoint()->GetKineticEnergy() == 0) { 
            /*std::cout << "[ TargetRecoilFilter ]: "
                        << "Electron never made it out of the target --> Killing all tracks!"
                        << std::endl;*/

            track->SetTrackStatus(fKillTrackAndSecondaries);
            G4RunManager::GetRunManager()->AbortEvent();
            return;
        }
    }
}

