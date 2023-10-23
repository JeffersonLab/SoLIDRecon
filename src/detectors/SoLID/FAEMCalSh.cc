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

        app->Add(new JChainMultifactoryGeneratorT<CalorimeterHitDigi_factoryT>(
          "FAEMCalShRawHits", {"FAEMCalShHits"}, {"FAEMCalShRawHits"},
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
          "FAEMCalShRecHits", {"FAEMCalShRawHits"}, {"FAEMCalShRecHits"},
          {
            .capADC = 16384,
            .dyRangeADC = 20. * dd4hep::GeV,
            .pedMeanADC = 100,
            .pedSigmaADC = 1,
            .resolutionTDC = 10 * dd4hep::picosecond,
            .thresholdFactor = 4.0,
            .thresholdValue = 3.0,
            .sampFrac = 0.998,
            .readout = "FAEMCalShHits",
            .sectorField = "sector",
          },
          app   // TODO: Remove me once fixed
        ));
        app->Add(new JChainMultifactoryGeneratorT<CalorimeterTruthClustering_factoryT>(
          "FAEMCalShTruthProtoClusters", {"FAEMCalShRecHits", "FAEMCalShHits"}, {"FAEMCalShTruthProtoClusters"},
          app   // TODO: Remove me once fixed
        ));
        app->Add(new JChainMultifactoryGeneratorT<CalorimeterIslandCluster_factoryT>(
          "FAEMCalShIslandProtoClusters", {"FAEMCalShRecHits"}, {"FAEMCalShIslandProtoClusters"},
          {
            .adjacencyMatrix = "(abs(row_1 - row_2) + abs(column_1 - column_2)) == 1",
            .readout = "FAEMCalShHits",
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
             "FAEMCalShTruthClusters",
            {"FAEMCalShTruthProtoClusters",        // edm4eic::ProtoClusterCollection
             "FAEMCalShHits"},                     // edm4hep::SimCalorimeterHitCollection
            {"FAEMCalShTruthClusters",             // edm4eic::Cluster
             "FAEMCalShTruthClusterAssociations"}, // edm4eic::MCRecoClusterParticleAssociation
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
             "FAEMCalShClusters",
            {"FAEMCalShIslandProtoClusters",  // edm4eic::ProtoClusterCollection
             "FAEMCalShHits"},                // edm4hep::SimCalorimeterHitCollection
            {"FAEMCalShClusters",             // edm4eic::Cluster
             "FAEMCalShClusterAssociations"}, // edm4eic::MCRecoClusterParticleAssociation
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
