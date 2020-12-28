// Martin Duy Tat 2nd November 2020

#include<algorithm>
#include<numeric>
#include<vector>
#include<complex>
#include<string>
#include<fstream>
#include<sstream>
#include"DDecayParameters.h"
#include"Constants.h"
#include"Generator.h"
#include"TLorentzVector.h"
#include"Event.h"
#include"EventList.h"
#include"Bin.h"
#include"TMath.h"
#include"Amplitude.h"
#include"TCanvas.h"
#include"TGraph.h"
#include"TAxis.h"
#include"TLegend.h"

DDecayParameters::DDecayParameters(PhaseSpaceParameterisation *psp, const EventList &eventlist) {
  // Number of bins in this binning scheme
  int NumberBins = psp->NumberOfBins();
  // Reset all bins that store D hadronic parameters
  m_K = std::vector<double>(NumberBins, 0.0);
  m_Kbar = std::vector<double>(NumberBins, 0.0);
  m_c = std::vector<double>(NumberBins, 0.0);
  m_s = std::vector<double>(NumberBins, 0.0);
  // Loop over all unweighted events
  for(int i = 0; i < eventlist.NumberEvents(); i++) {
    // Get generated event
    Event GeneratedEvent = eventlist.GetEvent(i);
    // Check which bin event belongs to
    int BinNumber = psp->WhichBin(GeneratedEvent);
    // Bin index, starting from 0
    int BinIndex = TMath::Abs(BinNumber) - 1;
    // Get the amplitudes
    std::complex<double> amplitude_d;
    std::complex<double> amplitude_dbar;
    // Swap amplitudes if event is in a negative bin
    GeneratedEvent.GetAmplitudes(amplitude_d, amplitude_dbar);
    if(BinNumber < 0) {
      std::swap(amplitude_d, amplitude_dbar);
    }
    // Calculate fractional yield
    m_K[BinIndex] += std::norm(amplitude_d);
    m_Kbar[BinIndex] += std::norm(amplitude_dbar);
    // Calculate strong phase difference
    double phase = std::arg(amplitude_d) - std::arg(amplitude_dbar);
    m_c[BinIndex] += TMath::Sqrt(std::norm(amplitude_d)*std::norm(amplitude_dbar))*TMath::Cos(phase);
    m_s[BinIndex] += TMath::Sqrt(std::norm(amplitude_d)*std::norm(amplitude_dbar))*TMath::Sin(phase);
  }
  double sumK = 0;
  for(int i = 0; i < NumberBins; i++) {
    // Amplitude averaged strong phase variation normalisation
    m_c[i] /= TMath::Sqrt(m_K[i]*m_Kbar[i]);
    m_s[i] /= TMath::Sqrt(m_K[i]*m_Kbar[i]);
    // Sum factional yields for normalisation
    sumK += m_K[i];
    sumK += m_Kbar[i];
  }
  // Divide by total to normalise fractional yields to 1
  std::transform(m_K.begin(), m_K.end(), m_K.begin(), std::bind(std::divides<double>(), std::placeholders::_1, sumK));
  std::transform(m_Kbar.begin(), m_Kbar.end(), m_Kbar.begin(), std::bind(std::divides<double>(), std::placeholders::_1, sumK));
}

DDecayParameters::DDecayParameters(const std::string &filename) {
  std::ifstream DDecayFile(filename);
  std::string line;
  std::getline(DDecayFile, line);
  while(std::getline(DDecayFile, line)) {
    std::stringstream ss(line);
    int i;
    double K, Kbar, c, s;
    ss >> i;
    ss.ignore();
    ss >> K;
    ss.ignore();
    ss >> Kbar;
    ss.ignore();
    ss >> c;
    ss.ignore();
    ss >> s;
    m_K.push_back(K);
    m_Kbar.push_back(Kbar);
    m_c.push_back(c);
    m_s.push_back(s);
  }
  DDecayFile.close();
}

void DDecayParameters::SaveCSV(const std::string &filename) const {
  std::ofstream DDecayFile(filename);
  DDecayFile << "i,K_i,Kbar_i,c_i,s_i\n";
  for(unsigned int i = 0; i < m_K.size(); i++) {
    DDecayFile << i << "," <<  m_K[i] << "," << m_Kbar[i] << "," << m_c[i] << "," << m_s[i] << std::endl;
  }
  DDecayFile.close();
}

void DDecayParameters::PlotParameters(const std::string &filename_cs, const std::string &filename_K) {
  std::vector<Double_t> circle_x(101), circle_y(101);
  for(int i = 0; i < 101; i++) {
    circle_x[i] = TMath::Cos(2*TMath::Pi()*i/100);
    circle_y[i] = TMath::Sin(2*TMath::Pi()*i/100);
  }
  TGraph *gr1 = new TGraph(this->Getc().size(), this->Getc().data(), this->Gets().data());
  TGraph *circle = new TGraph(101, circle_x.data(), circle_y.data());
  TCanvas *c1 = new TCanvas("s_vs_c", "s_i vs c_i", 700, 700);
  circle->Draw("AL");
  gr1->Draw("P");
  circle->GetXaxis()->SetTitle("c_{i}");
  circle->GetYaxis()->SetTitle("s_{i}");
  circle->GetXaxis()->SetRangeUser(-1.0, 1.0);
  circle->GetYaxis()->SetRangeUser(-1.0, 1.0);
  circle->SetTitle("Plot of s_i vs c_i");
  gr1->SetMarkerStyle(kFullDotLarge);
  c1->SetLeftMargin(0.11);
  circle->Draw("AL");
  gr1->Draw("P");
  c1->Update();
  c1->SaveAs(filename_cs.c_str());
  std::vector<double> KKbar = this->GetK();
  int NumberBins = this->GetK().size();
  KKbar.insert(KKbar.end(), this->GetKbar().begin(), this->GetKbar().end());
  double maximum = *std::max_element(KKbar.begin(), KKbar.end());
  std::vector<double> binning(KKbar.size()), binX(KKbar.size() + 1);
  for(int i = 1; i <= NumberBins; i++ ) {
    binning[i - 1] = -i;
    binning[i + NumberBins - 1] = i;
  }
  std::iota(binX.begin(), binX.end(), -NumberBins);
  TGraph *gr2 = new TGraph(binning.size(), binning.data(), KKbar.data());
  TCanvas *c2 = new TCanvas("K", "K_i and Kbar_i");
  gr2->Draw("AP");
  gr2->GetXaxis()->Set(2*NumberBins + 2, -NumberBins - 1, NumberBins + 1);
  gr2->GetYaxis()->SetRangeUser(0.0, maximum + 0.05);
  gr2->SetMarkerStyle(kFullDotLarge);
  gr2->Draw("AP");
  gr2->SetName("gr2");
  gr2->SetTitle("Fractional yields");
  gr2->GetXaxis()->SetTitle("Bin number");
  gr2->GetYaxis()->SetTitle("Fractional yield");
  c2->Update();
  c2->SaveAs(filename_K.c_str());
  delete gr1;
  delete gr2;
  delete circle;
  delete c1;
  delete c2;
}

const std::vector<double>& DDecayParameters::GetK() const {
  return m_K;
}

const std::vector<double>& DDecayParameters::GetKbar() const {
  return m_Kbar;
}

const std::vector<double>& DDecayParameters::Getc() const {
  return m_c;
}

const std::vector<double>& DDecayParameters::Gets() const {
  return m_s;
}
