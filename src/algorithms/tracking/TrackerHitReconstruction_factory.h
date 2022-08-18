#ifndef EICRECON_TRACKER_HIT_RECONSTRUCTION_FACTORY_H
#define EICRECON_TRACKER_HIT_RECONSTRUCTION_FACTORY_H

#include <random>

#include <spdlog/spdlog.h>

#include <JANA/JFactoryT.h>

#include <edm4hep/MCParticle.h>
#include <edm4hep/SimTrackerHitCollection.h>

#include <eicd/RawTrackerHit.h>
#include <eicd/RawTrackerHitCollection.h>
#include <eicd/MutableRawTrackerHit.h>

#include <TGeoSystemOfUnits.h>
#include <TRandomGen.h>
#include <eicd/MutableTrackerHit.h>

#include "TrackerHitReconstructionConfig.h"
#include "services/geometry/dd4hep/JDD4hep_service.h"
#include "TrackerHitReconstruction.h"

#include <algorithms/interfaces/JChainFactoryT.h>


class TrackerHitReconstruction_factory : public JChainFactoryT<eicd::TrackerHit> {

public:
    TrackerHitReconstruction_factory( std::vector<std::string> default_input_tags ):
            JChainFactoryT<eicd::TrackerHit>( std::move(default_input_tags) ) {
    }

    /** One time initialization **/
    void Init() override;

    /** On run change preparations **/
    void ChangeRun(const std::shared_ptr<const JEvent> &event) override;

    /** Event by event processing **/
    void Process(const std::shared_ptr<const JEvent> &event) override;

private:

    std::shared_ptr<spdlog::logger> m_log;              /// Logger for this factory
    eicrecon::TrackerHitReconstruction m_reco_algo;     /// The reconstruction algorithm
    int m_verbose;                                      /// verbosity 0-none, 1-default, 2-debug, 3-trace
    std::string m_input_tag;                            /// Tag for the input data
};


#endif //EICRECON_TRACKER_HIT_RECONSTRUCTION_FACTORY_H
