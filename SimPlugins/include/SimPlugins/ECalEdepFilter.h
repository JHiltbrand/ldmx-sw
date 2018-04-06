/**
 * @file ECalEdepFilter.h
 * @class ECalEdepFilter
 * @brief Class defining a UserActionPlugin that allows a user to filter out 
 *        events that exceed an energy deposition threshold in the ECal.
 * @author Josh Hiltbrand, University of Minnesota 
 */

#ifndef SIMPLUGINS_ECALEDEPFILTER_H_
#define SIMPLUGINS_ECALEDEPFILTER_H_

//----------------//
//   C++ StdLib   //
//----------------//
#include <algorithm>

//------------//
//   Geant4   //
//------------//
#include "G4RunManager.hh"

//----------//
//   LDMX   //
//----------//
#include "SimPlugins/UserActionPlugin.h"
#include "SimPlugins/ECalEdepFilterMessenger.h"

namespace ldmx {

    class ECalEdepFilterMessenger; 

    class ECalEdepFilter : public UserActionPlugin {

        public:

            /**
             * Class constructor.
             */
            ECalEdepFilter();

            /**
             * Class destructor.
             */
            ~ECalEdepFilter();

            /**
             * Get the name of the plugin.
             * @return The name of the plugin.
             */
            virtual std::string getName() {
                return "ECalEdepFilter";
            }

            /**
             * Get whether this plugin implements the event action.
             * @return True if the plugin implements the event action.
             */
            virtual bool hasEventAction() { 
                return true;
            }

            /**
             * Get whether this plugin implements the stepping action.
             * @return True to indicate this plugin implements the stepping action.
             */
            bool hasSteppingAction() {
                return true;
            }

            /**
             * Get whether this plugin implements the stacking aciton.
             * @return True to indicate this plugin implements the stacking action.
             */
            bool hasStackingAction() { 
                return true;
            }

            /**
             * Implement the stepping action.
             * @param step The Geant4 step.
             */
            void stepping(const G4Step* step);

            /**
             * End of event action.
             */
            virtual void endEvent(const G4Event*); 

            /**
             * Set the energy threshold for the ECal.
             */
            void setECalThreshold(double ecalThreshold) { 
                ecalThreshold_ = ecalThreshold; 
            }

        private:
            
            /** Messenger used to pass arguments to this class. */
            ECalEdepFilterMessenger* messenger_{nullptr};

            /** ECal energy deposition threshold. */
            double ecalThreshold_{45.0}; // MeV

            /** ECal energy depostion running total. */
            double ecalEdep_{0.0}; // MeV

    }; // ECalEdepFilter
}

#endif // SIMPLUGINS_ECALEDEPFILTER_H__
