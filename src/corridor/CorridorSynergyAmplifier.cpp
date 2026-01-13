#include "CorridorSynergyAmplifier.h"

#include <stdexcept>
#include <cmath>

namespace econet {

CorridorSynergyAmplifier::CorridorSynergyAmplifier(double maxAmplifier)
    : maxAmplifier_(maxAmplifier)
{
    if (maxAmplifier_ < 1.0) {
        throw std::invalid_argument("maxAmplifier must be >= 1.0");
    }
}

SynergyOutputRow CorridorSynergyAmplifier::compute(const CorridorInputRow& row) const {
    SynergyOutputRow out{};
    out.corridorId = row.corridorId;
    out.region = row.region;

    const double baseScore = computeBaseEcoImpactScore(row);
    out.baseEcoImpactScore = baseScore;

    SynergyCoefficients s = computeSynergyCoefficients(row);
    out.coeffs = s;

    const double synergyRaw = aggregateSynergy(s);
    const double amplifier = clampAmplifier(1.0 + synergyRaw);
    out.synergyAmplifier = amplifier;
    out.ecoImpactScoreSynergy = clamp01(baseScore * amplifier);

    return out;
}

double CorridorSynergyAmplifier::computeBaseEcoImpactScore(const CorridorInputRow& row) {
    const double weights[6] = {1,1,1,1,1,1};
    const double wSum = 6.0;

    double num =
        row.smartGridScore         * weights[0] +
        row.netZeroScore           * weights[1] +
        row.greenInfraScore        * weights[2] +
        row.airSensingScore        * weights[3] +
        row.lowCarbonMobilityScore * weights[4] +
        row.circularMaterialsScore * weights[5];

    return clamp01(num / wSum);
}

SynergyCoefficients CorridorSynergyAmplifier::computeSynergyCoefficients(const CorridorInputRow& row) {
    SynergyCoefficients c{};

    // 1) SmartGrid <-> Mobility (EV charging + DR)
    const double gridC = -row.smartGrid.deltaC_tco2;
    const double mobC  = -row.mobility.deltaC_tco2;
    c.gridEv = normalizedCoupling(gridC, mobC);

    // 2) SmartGrid <-> Buildings (DR + building automation)
    const double bldC = -row.buildings.deltaC_tco2;
    c.gridBuildings = normalizedCoupling(gridC, bldC);

    // 3) GreenInfra <-> Buildings (UHI cooling -> HVAC savings)
    const double greenC = -row.greenInfra.deltaC_tco2;
    c.greenBuildings = normalizedCoupling(greenC, bldC);

    // 4) AirSensing <-> Mobility (signal timing / routing by PM2.5)
    const double airP = -row.airSensing.deltaP_ugm3;
    const double mobP = -row.mobility.deltaP_ugm3;
    c.airMobility = normalizedCoupling(airP, mobP);

    // 5) CircularMaterials <-> Buildings (embodied + operational)
    const double matC = -row.materials.deltaC_tco2;
    c.materialsBuildings = normalizedCoupling(matC, bldC);

    return c;
}

double CorridorSynergyAmplifier::normalizedCoupling(double a, double b) {
    if (a <= 0.0 || b <= 0.0) return 0.0;

    const double a2 = a * a;
    const double b2 = b * b;
    const double denom = a2 + b2;
    if (denom == 0.0) return 0.0;
    return 2.0 * a * b / denom;
}

double CorridorSynergyAmplifier::aggregateSynergy(const SynergyCoefficients& s) {
    const double wGridEv          = 0.30;
    const double wGridBuildings   = 0.25;
    const double wGreenBuildings  = 0.20;
    const double wAirMobility     = 0.15;
    const double wMaterialsBld    = 0.10;

    const double total =
        s.gridEv             * wGridEv +
        s.gridBuildings      * wGridBuildings +
        s.greenBuildings     * wGreenBuildings +
        s.airMobility        * wAirMobility +
        s.materialsBuildings * wMaterialsBld;

    const double maxIncrement = 0.20;
    return maxIncrement * clamp01(total);
}

double CorridorSynergyAmplifier::clampAmplifier(double a) const {
    if (a < 1.0) return 1.0;
    if (a > maxAmplifier_) return maxAmplifier_;
    return a;
}

double CorridorSynergyAmplifier::clamp01(double x) {
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 1.0;
    return x;
}

} // namespace econet
