#include"RMSPlotter.hh"
#include<iostream>
RMSPlotter::RMSPlotter(int n){
  //ThePlot=NULL;
  eventsperenergy=n;
}

void RMSPlotter::AddRun(Double_t _Energy, Double_t _rms){
  if((counter==0)||(counter%eventsperenergy==0)){
    DataPoint tmpDP;
    tmpDP.Energy=_Energy;
    double Lower=_rms-(0.03*_rms);
    double Higher=_rms+(0.03*_rms);
    Char_t histname[50];
    std::sprintf(histname,"Distribution for Energy %5.0f",_Energy);
    tmpDP.Distribution=new TH1D(histname,histname,100,Lower,Higher);
    tmpDP.Distribution->Fill(_rms);
    Char_t fittedhistname[50];
    std::sprintf(fittedhistname,"Fitted Distribution for Energy %5.0f",_Energy);
    tmpDP.FittedDist=new TH1D(fittedhistname,fittedhistname,100,Lower,Higher);
    tmpDP.FittedDist->Fill(_rms);
    DataPoints.push_back(tmpDP);
    
    if(counter==0){
      i=DataPoints.begin();
      std::cout<<"counter = 0"<<std::endl;
    }
    else{
      ++i;
      std::cout<<"iterator iterated"<<std::endl;
    }
  }
  else{
    std::cout<<"Filling with rms= "<<_rms<<std::endl;
    DataPoint TheStruct=DataPoints.back();
    std::cout<<TheStruct.Energy<<std::endl;
    TheStruct.Distribution->Fill(_rms);
    TheStruct.FittedDist->Fill(_rms);
    DataPoints.back()=TheStruct;
    std::cout<<"done second else fine"<<std::endl;
  }
  counter++;
}

TGraphErrors* RMSPlotter::MakePlots(){
  std::ofstream outt("Results_Table.txt");
  outt<<"Energy/MeV  "<<"RMS of Scattering Angle/mRad "<<" Uncertainty/mRad" <<std::endl;
  int D= DataPoints.size();
  double dD=D;
  Double_t* E=new Double_t[D];
  Double_t* R=new Double_t[D];
  Double_t* RU=new Double_t[D];
  Double_t* EU=new Double_t[D];
  std::vector<DataPoint>::iterator Di=DataPoints.begin();
  int a=0;
  for(;Di!=DataPoints.end();++Di,++a){
    E[a]=(*Di).Energy;
    R[a]=(*Di).rms=(*Di).Distribution->GetMean();
    RU[a]=(*Di).rmsUncert=((*Di).Distribution->GetRMS())/dD;
    EU[a]=0;
    outt<<E[a]<<" , "<<R[a]<<" , "<<RU[a]<<std::endl;
  }
  ThePlot= new TGraphErrors(D,E,R,EU,RU);
  ThePlot->SetTitle("RMS of Scattering Angle Against Energy;Energy/MeV;RMS/mrad");
  delete[] E;
  delete[] R;
  delete[] RU;
  delete[] EU;
  outt.close();
  std::cout<<"MADE A PLOT "<<std::endl;
  return ThePlot;
}

TGraphErrors* RMSPlotter::MakeFittedPlots(){
  int D= DataPoints.size();
  Double_t* E=new Double_t[D];
  Double_t* R=new Double_t[D];
  Double_t* RU=new Double_t[D];
  Double_t* EU=new Double_t[D];
  std::vector<DataPoint>::iterator Di=DataPoints.begin();
  int a=0;
  for(;Di!=DataPoints.end();++Di,++a){
    (*Di).FittedDist->Fit("gaus");
    TF1* fit=(*Di).FittedDist->GetFunction("gaus");
    (*Di).Fitrms=fit->GetParameter(1);
    (*Di).FitrmsUncert=fit->GetParameter(2);
    E[a]=(*Di).Energy;
    R[a]=(*Di).Fitrms;
    RU[a]=(*Di).FitrmsUncert;
    EU[a]=0;
  }
  FittedPlot= new TGraphErrors(D,E,R,EU,RU);
  FittedPlot->SetTitle("Plot From Fitting Distribution of RMS results;Energy/MeV;RMS/mrad");
  delete[] E;
  delete[] R;
  delete[] RU;
  delete[] EU;
  return FittedPlot;
}

std::vector<TH1D*> RMSPlotter::GetDistributions(){
  std::vector<TH1D*> tmpV;
  std::vector<DataPoint>::iterator k=DataPoints.begin();
  for(;k!=DataPoints.end();++k){
    tmpV.push_back((*k).Distribution);
    tmpV.push_back((*k).FittedDist);
  }
  /* std::vector<FitEnergy>::iterator i=Fits.begin();
  for(;i!=Fits.end();++i){
    tmpV.push_back((*i).TheHist);
    }*/
  return tmpV;
}

/*int RMSPlotter::AddForFitting(double _Energy,TH1D* _Hist){
  if((counter==0)||(counter%eventsperenergy==0)){
    
    FitEnergy FitStruct;
    FitStruct.Energy=_Energy;
    FitStruct.rms=F->GetParameter(2);
    FitStruct.rmsUncert=F->GetParError(2);
    FitStruct.TheHist=_Hist;
    Fits.push_back(FitStruct);
    return 1;
  }
  else{
    return 0;
  }
  }*/

TGraphErrors* RMSPlotter::MakePlotFromFits(){
  int D= Fits.size();
  Double_t* E=new Double_t[D];
  Double_t* R=new Double_t[D];
  Double_t* RU=new Double_t[D];
  Double_t* EU=new Double_t[D];
  std::vector<FitEnergy>::iterator Di=Fits.begin();
  int a=0;
  for(;Di!=Fits.end();++Di,++a){
    double Xmin=((*Di).TheHist->GetMean()-(2.5*(*Di).TheHist->GetRMS()));
    double Xmax=((*Di).TheHist->GetMean()+(2.5*(*Di).TheHist->GetRMS()));
    (*Di).TheHist->Fit("gaus","E","",Xmin,Xmax);
    std::cout<<"Done Fit for E= "<<(*Di).Energy<<std::endl;
    TF1* F=(*Di).TheHist->GetFunction("gaus");
    (*Di).rms=F->GetParameter(2);
    (*Di).rmsUncert=F->GetParError(2);
    E[a]=(*Di).Energy;
    R[a]=(*Di).rms;
    RU[a]=(*Di).rmsUncert;
    EU[a]=0;
  }
  PlotFromFits= new TGraphErrors(D,E,R,EU,RU);
  PlotFromFits->SetTitle("PlotFromFits;Energy/MeV;RMS/mrad");
  delete[] E;
  delete[] R;
  delete[] RU;
  delete[] EU;
  return PlotFromFits;
}

void RMSPlotter::MakeAngleHist(double _Energy){
  if((counter==0)||(counter%eventsperenergy==0)){
    Char_t histname[50];
    std::sprintf(histname,"Angle projection at E = %5.0f MeV",_Energy);
    FitEnergy EnergyStruct;
    EnergyStruct.Energy=_Energy;
    EnergyStruct.TheHist=new TH1D(histname,histname,1000,-0.01,0.01);
    Fits.push_back(EnergyStruct);
    FitIt=Fits.end()-1;
  }
}

void RMSPlotter::FillAngleHist(double _Angle){
  (*FitIt).TheHist->Fill(_Angle);
}
    
