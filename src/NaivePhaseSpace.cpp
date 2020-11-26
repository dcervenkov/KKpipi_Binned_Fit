// Martin Duy Tat 2nd November 2020

#include<vector>
#include"NaivePhaseSpace.h"
#include"Event.h"
#include"PhaseSpaceParameterisation.h"

NaivePhaseSpace::NaivePhaseSpace(): PhaseSpaceParameterisation(4) {
}

int NaivePhaseSpace::WhichBin(const Event &event) const {
  std::vector<double> momenta = event.GetEvent();
  if(momenta[3] > momenta[7] && momenta[11] > momenta[15]) {
    return 0;
  } else if(momenta[3] > momenta[7] && momenta[11] < momenta[15]) {
    return 1;
  } else if(momenta[3] < momenta[7] && momenta[11] > momenta[15]) {
    return 2;
  } else {
    return 3;
  }
}

int NaivePhaseSpace::NumberOfBins() const {
  return PhaseSpaceParameterisation::NumberOfBins();
}