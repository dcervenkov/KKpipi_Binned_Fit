// Martin Duy Tat 30th October 2020
/**
 * Poolstudy is the program for doing a pool study of the binned fitting
 * D meson decay parameters are loaded from file
 */

#include<string>
#include<iostream>
#include<stdlib.h>
#include"PhaseSpaceParameterisation.h"
#include"TFile.h"
#include"TTree.h"
#include"BinList.h"
#include"DDecayParameters.h"
#include"CPParameters.h"
#include"Fitter.h"
#include"TMath.h"
#include"TH1D.h"

void SplitTree(TTree *tree, TTree *treeSmall, const int &StartEvent, const int &SampleSize);

int main(int argc, char *argv[]) {
  if(argc != 6) {
    std::cout << "Incorrect number of inputs\n";
    return 0;
  }
  double r_B = 0.1, delta_B = 130*TMath::Pi()/180, gamma = 75*TMath::Pi()/180;
  double xplus_true = r_B*TMath::Cos(delta_B + gamma);
  double xminus_true = r_B*TMath::Cos(delta_B - gamma);
  double yplus_true = r_B*TMath::Sin(delta_B + gamma);
  double yminus_true = r_B*TMath::Sin(delta_B - gamma);
  std::string BplusFilename = argv[1];
  std::string BminusFilename = argv[2];
  std::string DDecayFilename = argv[3];
  int SampleSize = atoi(argv[4]);
  int Samples = atoi(argv[5]);
  std::cout << "Starting B->DK, D->KKpipi binned fit pool study\n";
  std::cout << "Loaded phase space\n";
  TFile fBplus(BplusFilename.c_str(), "READ");
  TFile fBminus(BminusFilename.c_str(), "READ");
  std::cout << "Opened data files\n";
  TTree *treeBplus, *treeBminus;
  fBplus.GetObject("DalitzEventList", treeBplus);
  fBminus.GetObject("DalitzEventList", treeBminus);
  std::cout << "Loaded trees\n";
  TFile *f = new TFile("PullDistributions.root", "RECREATE");
  TTree *PullTree = new TTree("pull", "Pull distributions of #x_#pm and #y_#pm");
  Double_t xplus_pull, xminus_pull, yplus_pull, yminus_pull;
  PullTree->Branch("xplus", &xplus_pull, "xplus/D");
  PullTree->Branch("xminus", &xminus_pull, "xminus/D");
  PullTree->Branch("yplus", &yplus_pull, "yplus/D");
  PullTree->Branch("yminus", &yminus_pull, "yminus/D");
  for(int i = 0; i < Samples; i++) {
    std::cout << "Starting fitting of sample " << i << std::endl;
    TTree *treeSmallBplus = new TTree("DalitzEventList", "Dbar0 K+ K- pi+ pi-");
    TTree *treeSmallBminus = new TTree("DalitzEventList", "D0 K+ K- pi+ pi-");
    SplitTree(treeBplus, treeSmallBplus, Samples*i, SampleSize);
    SplitTree(treeBminus, treeSmallBminus, Samples*i, SampleSize);
    PhaseSpaceParameterisation psp;
    BinList binlist(psp);
    binlist.LoadTTree(treeSmallBplus, +1);
    binlist.LoadTTree(treeSmallBminus, -1);    
    DDecayParameters ddparameters(DDecayFilename);
    Fitter fit(binlist, ddparameters);
    //CPParameters cpparameters(-0.09, 0.06, -0.04, 0.08);
    CPParameters cpparameters(0.0, 0.0, 0.0, 0.0);
    double xplus, xminus, yplus, yminus, xpluse, xminuse, ypluse, yminuse;
    fit.DoFit(cpparameters);
    cpparameters.GetCPParameters(xplus, xminus, yplus, yminus);
    cpparameters.GetError(xpluse, xminuse, ypluse, yminuse);
    xplus_pull = (xplus - xplus_true)/xpluse;
    xminus_pull = (xminus - xminus_true)/xminuse;
    yplus_pull = (yplus - yplus_true)/ypluse;
    yminus_pull = (yminus - yminus_true)/yminuse;
    PullTree->Fill();
    //delete treeSmallBplus;
    //delete treeSmallBminus;
  }
  std::cout << "Pool study finished\n";
  std::cout << "Analysed " << Samples << " samples, each of size " << SampleSize << std::endl;
  PullTree->Write();
  f->Close();
  return 0;
}

void SplitTree(TTree *tree, TTree *treeSmall, const int &StartEvent, const int &SampleSize) {
  std::vector<Double_t> four_momentum(16);
  Double_t *p = four_momentum.data();
  treeSmall->Branch("_1_K~_Px", p + 0, "_1_K~_Px/D");
  treeSmall->Branch("_1_K~_Py", p + 1, "_1_K~_Py/D");
  treeSmall->Branch("_1_K~_Pz", p + 2, "_1_K~_Pz/D");
  treeSmall->Branch("_1_K~_E", p + 3, "_1_K~_E/D");
  treeSmall->Branch("_2_K#_Px", p + 4, "_2_K#_Px/D");
  treeSmall->Branch("_2_K#_Py", p + 5, "_2_K#_Py/D");
  treeSmall->Branch("_2_K#_Pz", p + 6, "_2_K#_Pz/D");
  treeSmall->Branch("_2_K#_E", p + 7, "_2_K#_E/D");
  treeSmall->Branch("_3_pi~_Px", p + 8, "_3_pi~_Px/D");
  treeSmall->Branch("_3_pi~_Py", p + 9, "_3_pi~_Py/D");
  treeSmall->Branch("_3_pi~_Pz", p + 10, "_3_pi~_Pz/D");
  treeSmall->Branch("_3_pi~_E", p +11, "_3_pi~_E/D");
  treeSmall->Branch("_4_pi#_Px", p + 12, "_4_pi#_Px/D");
  treeSmall->Branch("_4_pi#_Py", p + 13, "_4_pi#_Py/D");
  treeSmall->Branch("_4_pi#_Pz", p + 14, "_4_pi#_Pz/D");
  treeSmall->Branch("_4_pi#_E", p + 15, "_4_pi#_E/D");
  tree->SetBranchAddress("_1_K~_Px", p + 0);
  tree->SetBranchAddress("_1_K~_Py", p + 1);
  tree->SetBranchAddress("_1_K~_Pz", p + 2);
  tree->SetBranchAddress("_1_K~_E", p + 3);
  tree->SetBranchAddress("_2_K#_Px", p + 4);
  tree->SetBranchAddress("_2_K#_Py", p + 5);
  tree->SetBranchAddress("_2_K#_Pz", p + 6);
  tree->SetBranchAddress("_2_K#_E", p + 7);
  tree->SetBranchAddress("_3_pi~_Px", p + 8);
  tree->SetBranchAddress("_3_pi~_Py", p + 9);
  tree->SetBranchAddress("_3_pi~_Pz", p + 10);
  tree->SetBranchAddress("_3_pi~_E", p +11);
  tree->SetBranchAddress("_4_pi#_Px", p + 12);
  tree->SetBranchAddress("_4_pi#_Py", p + 13);
  tree->SetBranchAddress("_4_pi#_Pz", p + 14);
  tree->SetBranchAddress("_4_pi#_E", p + 15);
  for(int i = StartEvent; i < StartEvent + SampleSize; i++) {
    tree->GetEntry(i);
    treeSmall->Fill();
  }
}
