/*
 * @file Ecal.h
 * @brief Class providing a DetectorElement for the ECal
 * @author JeremyMcCormick, SLAC
 */

#ifndef DETDESCR_ECAL_H_
#define DETDESCR_ECAL_H_

// LDMX
#include "DetDescr/DetectorElementImpl.h"

namespace ldmx {

    /**
     * @class EcalLayer
     * @brief DetectorElement for ECal layers
     */
    class EcalStation : public DetectorElementImpl {

        public:

            EcalStation(DetectorElementImpl* parent, TGeoNode* support);

            /**
             * Get the layer number.
             * @return The layer number.
             */
            int getLayerNumber() {
                return layerNumber_;
            }

        private:

            /** The layer number (0-33 in v3 geometry). */
            int layerNumber_{-1};

            /** The module number (0-7). */
            int module_{-1};

    };

    /**
     * @class Ecal
     * @brief The top-level DetectorElement for the ECal.
     * @note
     * This DetectorElement is compatible with the ECal geometry
     * from the 'v1' and 'v2' detector models.
     */
    class Ecal : public DetectorElementImpl {

        public:

            Ecal();

            ~Ecal();

            /**
             * Get the EcalLayer object by its layer number.
             * @param layerNumber The layer number.
             * @note The layers are numbered from 1, not 0.
             */
            EcalStation* getEcalStation(int layerNumber);

            void initialize();

        private:
            DE_INIT(Ecal)
    };
}

#endif /* DETDESCR_ECALDETECTORELEMENT_H_ */