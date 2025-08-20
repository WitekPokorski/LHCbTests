//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file electromagnetic/TestEm5/TestEm5.cc
/// \brief Main program of the electromagnetic/TestEm5 example
//
//
// $Id$
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4GlobalConfig.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"
#include "SteppingVerbose.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

// LHCb
#include "RMSPlotter.hh"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TFile.h"
#include "TCanvas.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv) {

  //choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  G4VSteppingVerbose::SetInstance(new SteppingVerbose);

  // Construct the default run manager
#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
  // G4int nThreads = std::min(G4Threading::G4GetNumberOfCores(), 4);
  // enforce default one thread
  G4int nThreads = 1;
  if (argc==4) {
    nThreads = G4UIcommand::ConvertToInt(argv[3]);
  }
  runManager->SetNumberOfThreads(nThreads);
  G4cout << "===== TestEm5 is started with "
         <<  runManager->GetNumberOfThreads() << " threads =====" << G4endl;
#else
  G4RunManager* runManager = new G4RunManager;
#endif

  // set mandatory initialization classes
  DetectorConstruction* detector = new DetectorConstruction;
  runManager->SetUserInitialization(detector);
  runManager->SetUserInitialization(new PhysicsList());

  // LHCb
  int n = 1;
  if (argc == 3) {
    n = G4UIcommand::ConvertToInt(argv[2]);
  }
  RMSPlotter* rmsplotter = new RMSPlotter(n);

  // set user action classes
  //
  runManager->SetUserInitialization(new ActionInitialization(detector, rmsplotter));

  // get the pointer to the User Interface manager
  G4UImanager* UI = G4UImanager::GetUIpointer();

  if (argc!=1)   // batch mode
    {
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UI->ApplyCommand(command+fileName);

      // LHCb
      TGraphErrors* rms_plot = rmsplotter->MakePlots();
      std::cout << "Made ToyMC Style Plots" << std::endl;

      TGraphErrors* rms_fitted_plot = rmsplotter->MakeFittedPlots();
      rms_fitted_plot->SetName("GraphFitted");
      std::cout << "Made Fitted Brute Force Plot" << std::endl;
      std::vector<TH1D*> histos = rmsplotter->GetDistributions();

      TFile* output_root_file = new TFile("RMSResults.root", "RECREATE");
      rms_plot->Write();
      rms_fitted_plot->Write();

      TCanvas C;
      rms_plot->Draw("AP");
      C.Print("Results.pdf");

      for (auto const& hist: histos) {
        hist->Write();
      }

      output_root_file->Close();
      delete output_root_file;
      delete rms_plot;
      delete rms_fitted_plot;
    }

  else           //define visualization and UI terminal for interactive mode
    {
#ifdef G4VIS_USE
      G4VisManager* visManager = new G4VisExecutive;
      visManager->Initialize();
#endif

#ifdef G4UI_USE
      G4UIExecutive * ui = new G4UIExecutive(argc,argv);
      ui->SessionStart();
      delete ui;
#endif

#ifdef G4VIS_USE
      delete visManager;
#endif
    }

  // job termination
  //
  delete runManager;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
