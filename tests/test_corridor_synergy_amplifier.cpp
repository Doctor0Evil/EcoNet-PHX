#include "corridor/CorridorSynergyAmplifier.h"

#include <cassert>
#include <cmath>

using namespace econet;

int main() {
    CorridorSynergyAmplifier amp(1.20);

    CorridorInputRow row{};
    row.corridorId = "PHX-TEST";
    row.region = "Phoenix-AZ-US";
    row.smartGridScore = 0.8;
    row.netZeroScore = 0.7;
    row.greenInfraScore = 0.6;
    row.airSensingScore = 0.5;
    row.lowCarbonMobilityScore = 0.9;
    row.circularMaterialsScore = 0.7;

    row.smartGrid =   {0.0, -10.0, 0.0, 0.0};
    row.buildings =   {0.0, -12.0, 0.0, 0.0};
    row.mobility =    {0.0, -8.0,  0.0, -5.0};
    row.greenInfra =  {0.0, -6.0,  0.0, 0.0};
    row.airSensing =  {0.0, 0.0,   0.0, -3.0};
    row.materials =   {0.0, -4.0,  0.0, 0.0};

    SynergyOutputRow out = amp.compute(row);

    assert(out.baseEcoImpactScore >= 0.0 && out.baseEcoImpactScore <= 1.0);
    assert(out.synergyAmplifier >= 1.0 && out.synergyAmplifier <= 1.20);
    assert(out.ecoImpactScoreSynergy >= out.baseEcoImpactScore - 1e-9);

    assert(out.coeffs.gridEv > 0.0);
    assert(out.coeffs.gridBuildings > 0.0);
    assert(out.coeffs.greenBuildings > 0.0);
    assert(out.coeffs.airMobility > 0.0);
    assert(out.coeffs.materialsBuildings > 0.0);

    return 0;
}
