
#include "JEventProcessorPODIO.h"
#include <services/log/Log_service.h>
#include <JANA/Services/JComponentManager.h>

#include <datamodel_glue.h>
#include <algorithm>


template <typename PodioT, typename PodioCollectionT>
struct TestIfPodioType {
    bool operator() () {
        return true;
    }
};

template <typename PodioT, typename PodioCollectionT>
struct InsertFacIntoStore {
    size_t operator() (JFactory* fac, eic::EventStore* store, bool create) {
        std::string collection_name = fac->GetTag();
        if (create) {
            store->create<PodioCollectionT>(collection_name);
        }
        auto& collection = store->get<PodioCollectionT>(collection_name);
        auto tobjs = fac->GetAs<PodioT>();
        for (auto t : tobjs) {
            collection.push_back( t->clone() );
        }
        return tobjs.size();
    }
};

JEventProcessorPODIO::JEventProcessorPODIO() {
    SetTypeName(NAME_OF_THIS); // Provide JANA with this class's name

    japp->SetDefaultParameter(
            "podio:output_file",
            m_output_file,
            "Name of EDM4hep/podio output file to write to. Setting this will cause the output file to be created and written to."
    );

    // Allow user to set PODIO:OUTPUT_FILE to "1" to specify using the default name.
    if( m_output_file == "1" ){
        auto param = japp->GetJParameterManager()->FindParameter("podio:output_file" );
        if(param) {
            param->SetValue( param->GetDefault() );
            m_output_file = param->GetDefault();
        }
    }

    // Get the output directory path for creating a second copy of the output file at the end of processing.
    // (this is duplicating similar functionality in Juggler/Gaudi so assume it is useful).
    japp->SetDefaultParameter(
            "podio:output_file_copy_dir",
            m_output_file_copy_dir,
            "Directory name to make an additional copy of the output file to. Copy will be done at end of processing. Default is empty string which means do not make a copy. No check is made on path existing."
    );

    // Get the list of output collections to include/exclude
    std::vector<std::string> output_include_collections={
            "MCParticles",
            "GeneratedParticles",
            "ReconstructedParticles",
            "TrackParameters",
            "trackerHits",
            "BarrelTrackerHit",
            "EndcapTrackerHit",
            "EcalEndcapNClusters",
            "EcalEndcapPClusters",
            "EcalBarrelClusters",
            "HcalEndcapNClusters",
            "HcalEndcapPClusters",
            "HcalBarrelClusters",
            "ZDCEcalClusters",
            "EcalEndcapNTruthClusters",
            "EcalEndcapPTruthClusters",
            "EcalBarrelTruthClusters",
            "HcalEndcapNTruthClusters",
//            "HcalEndcapPTruthClusters",  // This gives lots of errors from volume manager on "unknown identifier"
            "HcalBarrelTruthClusters",
            "EcalBarrelTruthClusters",
            "ZDCEcalTruthClusters"
    };
    std::vector<std::string> output_exclude_collections;  // need to get as vector, then convert to set
    japp->SetDefaultParameter(
            "podio:output_include_collections",
            output_include_collections,
            "Comma separated list of collection names to write out. If not set, all collections will be written (including ones from input file). Don't set this and use PODIO:OUTPUT_EXCLUDE_COLLECTIONS to write everything except a selection."
    );
    japp->SetDefaultParameter(
            "podio:output_exclude_collections",
            output_exclude_collections,
            "Comma separated list of collection names to not write out."
    );

    m_output_include_collections = std::set<std::string>(output_include_collections.begin(),
                                                         output_include_collections.end());
    m_output_exclude_collections = std::set<std::string>(output_exclude_collections.begin(),
                                                         output_exclude_collections.end());

}


void JEventProcessorPODIO::Init() {

    auto app = GetApplication();
    m_log = app->GetService<Log_service>()->logger("JEventProcessorPODIO");
    m_log->set_level(spdlog::level::debug);
    m_store = new eic::EventStore();
    m_writer = std::make_shared<eic::ROOTWriter>(m_output_file, m_store, m_log);

}


void JEventProcessorPODIO::FindCollectionsToWrite(const std::vector<JFactory*>& factories) {

    // Set up the set of collections_to_write.
    std::map<std::string, std::string> all_collections_to_types;
    for (auto fac : factories) {
        const auto& colname = fac->GetTag();
        if (colname.empty()) {
            m_log->debug("Factory producing type '{}' has an empty collection name, omitting.", fac->GetObjectName());
        }
        else if (all_collections_to_types.count(colname) != 0) {
            throw JException("Collection name collision: Collection '%s' provided by factories producing types '%s' and '%s'.",
                             colname, all_collections_to_types[colname], fac->GetObjectName());
        }
        else {
            all_collections_to_types[fac->GetTag()] = fac->GetObjectName();
        }
    }
    std::map<std::string, std::string> collections_to_include;

    if (m_output_include_collections.empty()) {
        // User has not specified an include list, so we include _all_ JFactories present in first event.
        // (Non-PODIO types will be ignored later)
        m_log->debug("Persisting full set of valid podio types");
        collections_to_include = all_collections_to_types;
    }
    else {
        m_log->debug("Persisting podio types from includes list");
        m_user_included_collections = true;
        // We match up the include list with what is actually present in the JFactorySet
        for (const auto& col : m_output_include_collections) {
            auto it = all_collections_to_types.find(col);
            if (it != all_collections_to_types.end()) {
                collections_to_include[it->first] = it->second;
            }
            else {
                m_log->warn("Explicitly included collection '{}' not present in JFactorySet, omitting.", col);
            }
        }
    }

    // We remove any collections on the exclude list
    // (whether it be from the include list or the list of all factories)
    for (const auto& col : m_output_exclude_collections) {
        m_log->info("Excluding collection '{}'", col);
        collections_to_include.erase(col);
    }

    // If any of these collections are not PODIO types, remove from collections_to_write and inform the user right away.
    for (const auto& pair : collections_to_include) {
        const auto& col = pair.first;
        const auto& coltype = pair.second;

        if (CallWithPODIOType<TestIfPodioType, bool>(coltype) == std::nullopt) {
            // Severity of the log message depends on whether the user explicitly included the collection or not
            if (m_output_include_collections.empty()) {
                m_log->info("Collection '{}' has non-PODIO type '{}', omitting.", col, coltype);
            }
            else {
                m_log->warn("Explicitly included collection '{}' has non-PODIO type '{}', omitting.", col, coltype);
            }
        }
        else {
            // This IS a PODIO type, and should be included.
            m_collections_to_write[col] = coltype;
            m_log->debug("Writing collection '{}'", pair.first);
        }
    }
}

void JEventProcessorPODIO::Process(const std::shared_ptr<const JEvent> &event) {

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_is_first_event) {
        FindCollectionsToWrite(event->GetAllFactories());
    }

    m_log->trace("==================================");
    m_log->trace("Event #{}", event->GetEventNumber());

    // Loop over all collections/factories to write
    for( const auto& pair : (m_collections_to_write) ){
        JFactory* fac = event->GetFactory(pair.second, pair.first); // Object name, tag name=collection name

        // Attempt to put data from all factories into the store.
        // We need to do this for _all_ factories unless we've constrained it by using includes/excludes.
        // This is because podio::RootWriter constrains us to write one event at a time, so there
        // is no way to add a new branch after the first event.
        // This is called even for ones whose data classes don't inherit from
        // an edm4hep class. Those cases just silently do nothing here and return
        // an empty string. Note that this relies on the JFactory::EnableAs mechanism
        // so that needs to have been called in the factory constructor.
        try {
            if (m_user_included_collections) {
                // If the user specified some collections to include, we make sure that the corresponding factory
                // actually ran. If the user didn't specify any collections in the include list, we don't: For factories
                // that had not already been triggered by an EventProcessor, we simply write out zero objects.
                m_log->trace("Ensuring factory '{}:{}' has been called.", fac->GetObjectName(), fac->GetTag());
                fac->Create(event, mApplication, event->GetRunNumber());
            }
            auto result = CallWithPODIOType<InsertFacIntoStore, size_t, JFactory*, eic::EventStore*, bool>(fac->GetObjectName(), fac, m_store, m_is_first_event);

            if (result == std::nullopt) { 
                m_log->error("Unrecognized PODIO type '{}:{}', ignoring.", fac->GetObjectName(), fac->GetTag());
            }
            else {
                m_log->trace("Added PODIO type '{}:{}' for writing.", fac->GetObjectName(), fac->GetTag());
                if (m_is_first_event) {
                    // We only want to register for write once.
                    // If we register the same collection multiple times, PODIO will segfault.
                    m_writer->registerForWrite(fac->GetTag());
                }
            }
        }
        catch(std::exception &e){
            m_log->error("Exception adding PODIO type '{}:{}': {}.", fac->GetObjectName(), fac->GetTag(), e.what());
        }
    }
    m_writer->writeEvent();
    m_store->clearCollections();
    m_is_first_event = false;
}

void JEventProcessorPODIO::Finish() {
    m_writer->finish();
}
