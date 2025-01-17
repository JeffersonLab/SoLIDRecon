// Copyright 2022, David Lawrence
// Subject to the terms in the LICENSE file found in the top-level directory.
//
//

#include "extensions/jana/JChainMultifactoryGeneratorT.h"

#include "factories/calorimetry/CalorimeterClusterRecoCoG_factoryT.h"
#include "factories/calorimetry/CalorimeterHitDigi_factoryT.h"
#include "factories/calorimetry/CalorimeterHitReco_factoryT.h"
#include "factories/calorimetry/CalorimeterTruthClustering_factoryT.h"
#include "factories/calorimetry/CalorimeterIslandCluster_factoryT.h"

extern "C" {
    void InitPlugin(JApplication *app) {

        using namespace eicrecon;

        InitJANAPlugin(app);

	// preshower
        app->Add(new JChainMultifactoryGeneratorT<CalorimeterHitDigi_factoryT>(
          "LAEMCalPrShRawHits", {"LAEMCalPrShHits"}, {"LAEMCalPrShRawHits"},
          {
            .eRes = {0.0 * sqrt(dd4hep::GeV), 0.02, 0.0 * dd4hep::GeV},
            .tRes = 0.0 * dd4hep::ns,
            .capADC = 16384,
            .dyRangeADC = 20 * dd4hep::GeV,
            .pedMeanADC = 100,
            .pedSigmaADC = 1,
            .resolutionTDC = 10 * dd4hep::picosecond,
            .corrMeanScale = 1.0,
          },
          app   // TODO: Remove me once fixed
        ));
        app->Add(new JChainMultifactoryGeneratorT<CalorimeterHitReco_factoryT>(
          "LAEMCalPrShRecHits", {"LAEMCalPrShRawHits"}, {"LAEMCalPrShRecHits"},
          {
            .capADC = 16384,
            .dyRangeADC = 20. * dd4hep::GeV,
            .pedMeanADC = 100,
            .pedSigmaADC = 1,
            .resolutionTDC = 10 * dd4hep::picosecond,
            .thresholdFactor = 4.0,
            .thresholdValue = 3.0,
            .sampFrac = 0.998,
            .readout = "LAEMCalPrShHits",
            .sectorField = "sector",
          },
          app   // TODO: Remove me once fixed
        ));
        app->Add(new JChainMultifactoryGeneratorT<CalorimeterTruthClustering_factoryT>(
          "LAEMCalPrShTruthProtoClusters", {"LAEMCalPrShRecHits", "LAEMCalPrShHits"}, {"LAEMCalPrShTruthProtoClusters"},
          app   // TODO: Remove me once fixed
        ));
        app->Add(new JChainMultifactoryGeneratorT<CalorimeterIslandCluster_factoryT>(
          "LAEMCalPrShIslandProtoClusters", {"LAEMCalPrShRecHits"}, {"LAEMCalPrShIslandProtoClusters"},
          {
            .adjacencyMatrix = "(abs(row_1 - row_2) + abs(column_1 - column_2)) == 1",
            .readout = "LAEMCalPrShHits",
            .sectorDist = 5.0 * dd4hep::cm,
            .splitCluster = true,
            .minClusterHitEdep = 1.0 * dd4hep::MeV,
            .minClusterCenterEdep = 30.0 * dd4hep::MeV,
            .transverseEnergyProfileMetric = "globalDistEtaPhi",
            .transverseEnergyProfileScale = 0.08,
          },
          app   // TODO: Remove me once fixed
        ));

        app->Add(
          new JChainMultifactoryGeneratorT<CalorimeterClusterRecoCoG_factoryT>(
             "LAEMCalPrShTruthClusters",
            {"LAEMCalPrShTruthProtoClusters",        // edm4eic::ProtoClusterCollection
             "LAEMCalPrShHits"},                     // edm4hep::SimCalorimeterHitCollection
            {"LAEMCalPrShTruthClusters",             // edm4eic::Cluster
             "LAEMCalPrShTruthClusterAssociations"}, // edm4eic::MCRecoClusterParticleAssociation
            {
              .energyWeight = "log",
              .moduleDimZName = "",
              .sampFrac = 1.0,
              .logWeightBase = 4.6,
              .depthCorrection = 0.0,
              .enableEtaBounds = false
            },
            app   // TODO: Remove me once fixed
          )
        );

        app->Add(
          new JChainMultifactoryGeneratorT<CalorimeterClusterRecoCoG_factoryT>(
             "LAEMCalPrShClusters",
            {"LAEMCalPrShIslandProtoClusters",  // edm4eic::ProtoClusterCollection
             "LAEMCalPrShHits"},                // edm4hep::SimCalorimeterHitCollection
            {"LAEMCalPrShClusters",             // edm4eic::Cluster
             "LAEMCalPrShClusterAssociations"}, // edm4eic::MCRecoClusterParticleAssociation
            {
              .energyWeight = "log",
              .moduleDimZName = "",
              .sampFrac = 1.0,
              .logWeightBase = 3.6,
              .depthCorrection = 0.0,
              .enableEtaBounds = false,
            },
            app   // TODO: Remove me once fixed
          )
        );

	// shower
        app->Add(new JChainMultifactoryGeneratorT<CalorimeterHitDigi_factoryT>(
          "LAEMCalShRawHits", {"LAEMCalShHits"}, {"LAEMCalShRawHits"},
          {
            .eRes = {0.0 * sqrt(dd4hep::GeV), 0.02, 0.0 * dd4hep::GeV},
            .tRes = 0.0 * dd4hep::ns,
            .capADC = 16384,
            .dyRangeADC = 20 * dd4hep::GeV,
            .pedMeanADC = 100,
            .pedSigmaADC = 1,
            .resolutionTDC = 10 * dd4hep::picosecond,
            .corrMeanScale = 1.0,
          },
          app   // TODO: Remove me once fixed
        ));
        app->Add(new JChainMultifactoryGeneratorT<CalorimeterHitReco_factoryT>(
          "LAEMCalShRecHits", {"LAEMCalShRawHits"}, {"LAEMCalShRecHits"},
          {
            .capADC = 16384,
            .dyRangeADC = 20. * dd4hep::GeV,
            .pedMeanADC = 100,
            .pedSigmaADC = 1,
            .resolutionTDC = 10 * dd4hep::picosecond,
            .thresholdFactor = 4.0,
            .thresholdValue = 3.0,
            .sampFrac = 0.998,
            .readout = "LAEMCalShHits",
            .sectorField = "sector",
          },
          app   // TODO: Remove me once fixed
        ));
        app->Add(new JChainMultifactoryGeneratorT<CalorimeterTruthClustering_factoryT>(
          "LAEMCalShTruthProtoClusters", {"LAEMCalShRecHits", "LAEMCalShHits"}, {"LAEMCalShTruthProtoClusters"},
          app   // TODO: Remove me once fixed
        ));
        app->Add(new JChainMultifactoryGeneratorT<CalorimeterIslandCluster_factoryT>(
          "LAEMCalShIslandProtoClusters", {"LAEMCalShRecHits"}, {"LAEMCalShIslandProtoClusters"},
          {
            .adjacencyMatrix = "(abs(row_1 - row_2) + abs(column_1 - column_2)) == 1",
            .readout = "LAEMCalShHits",
            .sectorDist = 5.0 * dd4hep::cm,
            .splitCluster = true,
            .minClusterHitEdep = 1.0 * dd4hep::MeV,
            .minClusterCenterEdep = 30.0 * dd4hep::MeV,
            .transverseEnergyProfileMetric = "globalDistEtaPhi",
            .transverseEnergyProfileScale = 0.08,
          },
          app   // TODO: Remove me once fixed
        ));

        app->Add(
          new JChainMultifactoryGeneratorT<CalorimeterClusterRecoCoG_factoryT>(
             "LAEMCalShTruthClusters",
            {"LAEMCalShTruthProtoClusters",        // edm4eic::ProtoClusterCollection
             "LAEMCalShHits"},                     // edm4hep::SimCalorimeterHitCollection
            {"LAEMCalShTruthClusters",             // edm4eic::Cluster
             "LAEMCalShTruthClusterAssociations"}, // edm4eic::MCRecoClusterParticleAssociation
            {
              .energyWeight = "log",
              .moduleDimZName = "",
              .sampFrac = 1.0,
              .logWeightBase = 4.6,
              .depthCorrection = 0.0,
              .enableEtaBounds = false
            },
            app   // TODO: Remove me once fixed
          )
        );

        app->Add(
          new JChainMultifactoryGeneratorT<CalorimeterClusterRecoCoG_factoryT>(
             "LAEMCalShClusters",
            {"LAEMCalShIslandProtoClusters",  // edm4eic::ProtoClusterCollection
             "LAEMCalShHits"},                // edm4hep::SimCalorimeterHitCollection
            {"LAEMCalShClusters",             // edm4eic::Cluster
             "LAEMCalShClusterAssociations"}, // edm4eic::MCRecoClusterParticleAssociation
            {
              .energyWeight = "log",
              .moduleDimZName = "",
              .sampFrac = 1.0,
              .logWeightBase = 3.6,
              .depthCorrection = 0.0,
              .enableEtaBounds = false,
            },
            app   // TODO: Remove me once fixed
          )
        );
    }
}
