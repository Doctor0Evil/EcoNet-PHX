#pragma once

#include <string>

namespace econet {

struct ClusterDeltas {
    double deltaE_kwh;      // energy change (negative = savings)
    double deltaC_tco2;     // CO2 change (negative = reduction)
    double deltaW_m3;       // water change
    double deltaP_ugm3;     // pollution change (e.g., PM2.5)
};

struct CorridorInputRow {
    std::string corridorId;
    std::string region;

    // normalized 0–1 base scores per cluster
    double smartGridScore;
    double netZeroScore;
    double greenInfraScore;
    double airSensingScore;
    double lowCarbonMobilityScore;
    double circularMaterialsScore;

    // physical deltas per cluster, corridor-aggregated
    ClusterDeltas smartGrid;
    ClusterDeltas buildings;
    ClusterDeltas mobility;
    ClusterDeltas greenInfra;
    ClusterDeltas airSensing;
    ClusterDeltas materials;
};

struct SynergyCoefficients {
    double gridEv;             // SmartGrid <-> Mobility
    double gridBuildings;      // SmartGrid <-> Buildings
    double greenBuildings;     // GreenInfra <-> Buildings
    double airMobility;        // AirSensing <-> Mobility
    double materialsBuildings; // Materials <-> Buildings
};

struct SynergyOutputRow {
    std::string corridorId;
    std::string region;
    double baseEcoImpactScore;   // pre-synergy composite 0–1
    double synergyAmplifier;     // multiplicative factor >= 1.0
    double ecoImpactScoreSynergy;// final score, capped at 1.0
    SynergyCoefficients coeffs;
};

class CorridorSynergyAmplifier {
public:
    explicit CorridorSynergyAmplifier(double maxAmplifier = 1.20);

    SynergyOutputRow compute(const CorridorInputRow& row) const;

private:
    double maxAmplifier_;

    static double computeBaseEcoImpactScore(const CorridorInputRow& row);
    static SynergyCoefficients computeSynergyCoefficients(const CorridorInputRow& row);
    static double normalizedCoupling(double a, double b);
    static double aggregateSynergy(const SynergyCoefficients& s);
    double clampAmplifier(double a) const;
    static double clamp01(double x);
};

} // namespace econet
