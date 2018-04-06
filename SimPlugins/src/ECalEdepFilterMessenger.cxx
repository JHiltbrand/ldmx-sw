/**
 * @file ECalEdepFilterMessenger.cxx
 * @brief Messenger for setting parameters on ECalEdepFilter.
 * @author Josh Hiltbrand, University of Minnesota 
 */

#include "SimPlugins/ECalEdepFilterMessenger.h"
#include <iostream>

namespace ldmx { 
    
    ECalEdepFilterMessenger::ECalEdepFilterMessenger(ECalEdepFilter* filter) :
        UserActionPluginMessenger(filter), filter_(filter) {

            thresholdCmd_ = new G4UIcmdWithAString{std::string(getPath() + "threshold").c_str(), this};
            thresholdCmd_->AvailableForStates(G4ApplicationState::G4State_PreInit,
                                         G4ApplicationState::G4State_Idle);
            thresholdCmd_->SetGuidance("ECal Energy deposition threshold to filter events on."); 

    }

    ECalEdepFilterMessenger::~ECalEdepFilterMessenger() {
        delete thresholdCmd_;
    }

    void ECalEdepFilterMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
        
        // Handles verbose command.
        UserActionPluginMessenger::SetNewValue(command, newValue);

        if (command == thresholdCmd_) {
            filter_->setECalThreshold(G4UIcommand::ConvertToDouble(newValue));
        }
    }
}
