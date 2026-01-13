#include "CorridorSynergyAmplifier.h"

#include <vector>
#include <string>

namespace econet {

struct CorridorRecordPersisted {
    std::string corridorId;
    std::string region;
    double baseEcoImpactScore;
    double synergyAmplifier;
    double ecoImpactScoreSynergy;
    SynergyCoefficients coeffs;
};

class CorridorAnalyticsService {
public:
    explicit CorridorAnalyticsService(double maxAmplifier = 1.20)
        : amplifier_(maxAmplifier) {}

    CorridorRecordPersisted evaluateCorridor(const CorridorInputRow& row) const {
        SynergyOutputRow out = amplifier_.compute(row);
        CorridorRecordPersisted stored{};
        stored.corridorId = out.corridorId;
        stored.region = out.region;
        stored.baseEcoImpactScore = out.baseEcoImpactScore;
        stored.synergyAmplifier = out.synergyAmplifier;
        stored.ecoImpactScoreSynergy = out.ecoImpactScoreSynergy;
        stored.coeffs = out.coeffs;
        return stored;
    }

private:
    CorridorSynergyAmplifier amplifier_;
};

} // namespace econet
