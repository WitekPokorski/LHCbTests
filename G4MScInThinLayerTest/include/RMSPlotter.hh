#ifndef INCLUDE_RMSPLOTTER_HH 
#define INCLUDE_RMSPLOTTER_HH 1
#include<vector>
#include"TGraphErrors.h"
#include"TH1D.h"
#include"TF1.h"
#include<map>
#include"TFitResultPtr.h"
#include<fstream>
struct DataPoint{
  double Energy;
  double rms;
  double rmsUncert;
  TH1D* Distribution;
  TH1D* FittedDist;
  double Fitrms;
  double FitrmsUncert;
};

struct FitEnergy{
  double Energy;
  double rms;
  double rmsUncert;
  TH1D* TheHist;
};

class RMSPlotter
{
private:
  std::vector<DataPoint> DataPoints;
  TGraphErrors* ThePlot=NULL;
  TGraphErrors* FittedPlot=NULL;
  TGraphErrors* PlotFromFits=NULL;
  int counter=0;
  int eventsperenergy=0;
  std::vector<DataPoint>::iterator i;
  std::vector<FitEnergy> Fits;
  std::vector<FitEnergy>::iterator FitIt=Fits.begin();
public:
  RMSPlotter(int n);
  void AddRun(Double_t _Energy,Double_t _rms);
  //int AddForFitting(double _Energy,TH1D* _Hist);
  void MakeAngleHist(double _Energy);
  void FillAngleHist(double _Angle);
  TGraphErrors* MakePlots();
  TGraphErrors* MakeFittedPlots();
  TGraphErrors* MakePlotFromFits();
  std::vector<TH1D*> GetDistributions();
};
#endif
