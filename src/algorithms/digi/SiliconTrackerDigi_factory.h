#ifndef EICRECON_JFACTORY_SILICON_TRACKER_DIGI_H
#define EICRECON_JFACTORY_SILICON_TRACKER_DIGI_H

#include <random>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <algorithms/interfaces/JChainFactoryT.h>
#include <JANA/JEvent.h>

#include <edm4hep/SimTrackerHit.h>
#include <edm4hep/MCParticle.h>
#include <edm4hep/SimTrackerHitCollection.h>

#include <eicd/RawTrackerHit.h>
#include <eicd/RawTrackerHitCollection.h>
#include <eicd/MutableRawTrackerHit.h>

#include <TGeoSystemOfUnits.h>
#include <TRandomGen.h>

#include "SiliconTrackerDigiConfig.h"
#include "SiliconTrackerDigi.h"

namespace eicrecon {

    class SiliconTrackerDigi;

    class SiliconTrackerDigi_factory : public  JChainFactoryT<eicd::RawTrackerHit> {

    public:

        explicit SiliconTrackerDigi_factory(const std::vector<std::string> &default_input_tags);

        /** One time initialization **/
        void Init() override;

        /** On run change preparations **/
        void ChangeRun(const std::shared_ptr<const JEvent> &event) override;

        /** Event by event processing **/
        void Process(const std::shared_ptr<const JEvent> &event) override;

    private:

        std::shared_ptr<spdlog::logger> m_log;          /// Logger
        std::vector<std::string> m_input_tags;          /// Input base tags
        eicrecon::SiliconTrackerDigi m_digi_algo;       /// Actual digitisation algorithm
        int m_verbose = 0;                              /// 0 - none, 1 - default, 2 - debug, 3 - trace
    };

}

#endif //EICRECON_JFACTORY_SILICON_TRACKER_DIGI_H
