/**
 * @file ECalEdepFilterMessenger.h
 * @brief Messenger for setting parameters on ECalEdepFilter.
 * @author Josh Hiltbrand, University of Minnesota 
 */

#ifndef SIMPLUGINS_ECALEDEPFILTERMESSENGER_H
#define SIMPLUGINS_ECALEDEPFILTERMESSENGER_H

//------------//
//   Geant4   //
//------------//
#include "G4UIcmdWithAString.hh"

//-------------//
//   ldmx-sw   //
//-------------//
#include "SimPlugins/ECalEdepFilter.h"
#include "SimPlugins/UserActionPluginMessenger.h"

namespace ldmx { 
   
    // Forward declare to avoid circular dependency in headers
    class ECalEdepFilter;

    class ECalEdepFilterMessenger : UserActionPluginMessenger {
        
        public: 

            /** 
             * Constructor
             *
             * @param Filter associated with this messenger.
             */
            ECalEdepFilterMessenger(ECalEdepFilter* filter); 

            /** Destructor */
            ~ECalEdepFilterMessenger();

            /**
             */
            void SetNewValue(G4UIcommand * command, G4String newValue);

        private:

            /** The filter associated with this messenger. */
            ECalEdepFilter* filter_{nullptr};
            
            /**
             * Command allowing a user to specify the total ECal deposition 
             * used to filter out events. 
             */
            G4UIcmdWithAString* thresholdCmd_{nullptr};

    }; // ECalEdepFilterMessenger
}

#endif // SIMPLUGINS_ECALEDEPFILTERMESSENGER_H
