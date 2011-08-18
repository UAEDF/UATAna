
#ifndef __UATAnaDisPlay_H__
#define __UATAnaDisPlay_H__

#include "../includes/UATAnaConfig.h"

#include <TString.h>
#include <TFile.h>
#include <TH1F.h>

#include <string>
#include <vector>
using namespace std;

class UATAnaDisplay {

  private:

  TFile* File;

  vector<TH1F*> CCflow      ; 
  vector<TH1F*> CCflowGroup ; 
  
  vector<ScanFlow_t> SCflow      ; 
  vector<ScanFlow_t> SCflowGroup ; 

  vector<TH1F*> PlotCC ;
  vector<TH1F*> PlotSC ;
  vector<TH1F*> PlotCCGroup ;
  vector<TH1F*> PlotSCGroup ;

  void PlotStack( string  , string  , string  , bool  ,
                  vector<TH1F*>&  , vector<TH1F*>&  , vector<TH1F*>&  ,
                  vector<string>  , vector<string>  , vector<string>  ) ;

  void PrintYields ( string  , string  , vector<string>  ,
                     vector<TH1F*>&  , vector<TH1F*>&  , vector<TH1F*>&  ,
                     vector<string>  , vector<string>  , vector<string>  ) ;

  public:

  UATAnaDisplay(){;}
  virtual ~UATAnaDisplay(){;}

  void Init   ( UATAnaConfig& );
  void Yields ( UATAnaConfig& , bool );
  void CPlot  ( UATAnaConfig& );
  

};

#endif
