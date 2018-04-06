/**
 * @file DisablePhotoNuclear.h
 * @class DisablePhotoNuclear
 * @brief Class defining a UserActionPlugin that turns off the photonNuclear processes completely
 * @author Michael Revering, University of Minnesota
 */

#ifndef SIMPLUGINS_DISABLEPHOTONUCLEAR_H_
#define SIMPLUGINS_DISABLEPHOTONUCLEAR_H_

// Geant4
#include "G4RunManager.hh"

// LDMX
#include "SimPlugins/UserActionPlugin.h"

namespace ldmx {

    class DisablePhotoNuclear : public UserActionPlugin {

        public:

            /**
             * Class constructor.
             */
            DisablePhotoNuclear();

            /**
             * Class destructor.
             */
            ~DisablePhotoNuclear();

            /**
             * Get the name of the plugin.
             * @return The name of the plugin.
             */
            virtual std::string getName() {
                return "DisablePhotoNuclear";
            }

            bool hasRunAction() {
                return true;
            }

            virtual void beginRun(const G4Run*); 
            

    };
// DisablePhotoNuclear
}

#endif // SIMPLUGINS_DISABLEPHOTONUCLEAR_H__
