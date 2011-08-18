
#ifndef __UATAnaReader_H__
#define __UATAnaReader_H__

#include "../includes/UATAnaConfig.h"

#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>

#include <string>
#include <vector>
using namespace std;

class UATAnaReader {

  private:

  TFile* File;
  TTree* Tree; 

  vector<TH1F*> CCflow      ; 
  vector<TH1F*> CCflowGroup ; 
  
  vector<ScanFlow_t> SCflow      ; 
  vector<ScanFlow_t> SCflowGroup ; 

  vector<TH1F*> PlotCC ;
  vector<TH1F*> PlotSC ;
  vector<TH1F*> PlotCCGroup ;
  vector<TH1F*> PlotSCGroup ;


  void FillCutFlow  ( UATAnaConfig& , TString , int , Double_t ) ;
  void FillScanFlow ( UATAnaConfig& , string ScanName ,  TString , int , Double_t ) ;

  void FillPlotCC   ( UATAnaConfig& , string , Double_t );
  void FillPlotSC   ( UATAnaConfig& , string , Double_t );  

  public:

  UATAnaReader(){ }
  virtual ~UATAnaReader(){ }

  void Init   ( UATAnaConfig& );
  void Analyze( UATAnaConfig& );
  void End    ( UATAnaConfig& );

};

#endif 

