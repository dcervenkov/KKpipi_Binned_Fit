// Martin Duy Tat 25th November 2020

#include<vector>
#include<string>
#include<complex>
#include<fstream>
#include"SophisticatedPhaseSpace.h"
#include"Event.h"
#include"Amplitude.h"
#include"TFile.h"
#include"TTree.h"

SophisticatedPhaseSpace::SophisticatedPhaseSpace(double *masses): RectangularPhaseSpace({12, 1, 1, 1, 1}, masses) {
}

SophisticatedPhaseSpace::~SophisticatedPhaseSpace() {
  if(m_AmplitudeModel != nullptr) {
    delete m_AmplitudeModel;
  }
}

void SophisticatedPhaseSpace::x3x4WhichBin(const Event &event, int &x3bin, int &x4bin) const {
  std::vector<double> X = RectangularPhaseSpace::RectCoordinates(event);
  double x3lower = RectangularPhaseSpace::GetLowerBoundary(2);
  double x3upper = RectangularPhaseSpace::GetUpperBoundary(2);
  double x4lower = RectangularPhaseSpace::GetLowerBoundary(3);
  double x4upper = RectangularPhaseSpace::GetUpperBoundary(3);
  x3bin = int((X[2] - x3lower)*m_x3bins/(x3upper - x3lower));
  x4bin = int((X[3] - x4lower)*m_x4bins/(x4upper - x4lower));
}

void SophisticatedPhaseSpace::LoadAmplitudeModel(const std::string &Damplitude, const std::string &DBARamplitude) {
  m_AmplitudeModel = new Amplitude(Damplitude, DBARamplitude);
}

double SophisticatedPhaseSpace::StrongPhase(const Event &event) const {
  std::vector<double> fourmomenta = event.GetEventVector();
  return std::arg(m_AmplitudeModel->operator()(fourmomenta, +1)) - std::arg(m_AmplitudeModel->operator()(fourmomenta, -1));
}

void SophisticatedPhaseSpace::CalculateStrongPhases(std::string BplusFilename, std::string BminusFilename, std::string MeanFilename, std::string RMSFilename) const {
  std::vector<std::vector<double>> average(m_x3bins, std::vector<double>(m_x4bins));
  std::vector<std::vector<double>> rms(m_x3bins, std::vector<double>(m_x4bins));
  std::vector<std::vector<int>> numberevents(m_x3bins, std::vector<int>(m_x4bins));
  TFile fBplus(BplusFilename.c_str(), "READ");
  TFile fBminus(BminusFilename.c_str(), "READ");
  TTree *treeBplus, *treeBminus;
  fBplus.GetObject("DalitzEventList", treeBplus);
  fBminus.GetObject("DalitzEventList", treeBminus);
  std::vector<double> FourMomentumBplus(16), FourMomentumBminus(16);
  for(int i = 0; i < 16; i++) {
    std::string address = treeBplus->GetListOfBranches()[0][i]->GetName();
    if(i%4 == 0) {
      treeBplus->SetBranchAddress(address.c_str(), FourMomentumBplus.data() + i + 3);
      treeBminus->SetBranchAddress(address.c_str(), FourMomentumBminus.data() + i + 3);
    } else {
      treeBplus->SetBranchAddress(address.c_str(), FourMomentumBplus.data() + i - 1);
      treeBminus->SetBranchAddress(address.c_str(), FourMomentumBminus.data() + i - 1);
    }
  }
  int x3bin, x4bin;
  double BplusStrongPhase, BminusStrongPhase;
  for(int i = 0; i < treeBplus->GetEntries(); i++) {
    treeBplus->GetEvent(i);
    treeBminus->GetEvent(i);
    Event EventBplus(FourMomentumBplus), EventBminus(FourMomentumBminus);
    BplusStrongPhase = StrongPhase(EventBplus);
    BminusStrongPhase = StrongPhase(EventBminus);
    x3x4WhichBin(EventBplus, x3bin, x4bin);
    numberevents[x3bin][x4bin] += 1;
    average[x3bin][x4bin] += BplusStrongPhase;
    rms[x3bin][x4bin] += BplusStrongPhase*BplusStrongPhase;
    x3x4WhichBin(EventBminus, x3bin, x4bin);
    numberevents[x3bin][x4bin] += 1;
    average[x3bin][x4bin] += BminusStrongPhase;
    rms[x3bin][x4bin] += BminusStrongPhase*BminusStrongPhase;
  }
  std::ofstream AverageFile(MeanFilename), RMSFile(RMSFilename);
  for(int i = 0; i < m_x3bins; i++) {
    for(int j = 0; j < m_x4bins; j++) {
      average[i][j] = average[i][j]/numberevents[i][j];
      rms[i][j] = (rms[i][j]/numberevents[i][j]) - average[i][j]*average[i][j];
      AverageFile << average[i][j] << " ";
      RMSFile << rms[i][j] << " ";
    }
    AverageFile << std::endl;
    RMSFile << std::endl;
  }
  AverageFile.close();
  RMSFile.close();
}

int SophisticatedPhaseSpace::WhichBin(const Event &event) const {
  std::vector<double> X = RectangularPhaseSpace::RectCoordinates(event);
  int phi;
  if(X[4] > 0) {
    phi = 6;
  } else {
    phi = 0;
  }
  if(X[2] + X[3] > 1.4) {
    return 0 + phi;
  } else if(10*X[2] - 7*X[3] < -10) {
    return 1 + phi;
  } else if(45*X[2] - 35*X[3] < -17) { 
    return 2 + phi;
  } else if(7*X[2] - 5*X[3] > 5) {
    return 3 + phi;
  } else if(9*X[2] + 4*X[3] > 5) {
    return 4 + phi;
  } else {
    return 5 + phi;
  }
}

int SophisticatedPhaseSpace::NumberOfBins() const {
  return RectangularPhaseSpace::NumberOfBins();
}