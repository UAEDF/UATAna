
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

  void PlotStack( string  , string  , string  , int   ,
                  vector<TH1F*>&  , vector<TH1F*>&  , vector<TH1F*>&  ,
                  vector<string>  , vector<string>  , vector<string>  ,
                  vector<int>     , vector<int>     , vector<int>     ,
                  string = "Var"  , vector<string> = vector<string>() , string =""    ,
                  float  =0       , bool = 0        , bool = true     , bool = true ,
                  vector<string> = vector<string>() , float = 8, bool = true 
                ) ;

  void PrintYields ( string  , string  , vector<string>  ,
                     vector<TH1F*>&  , vector<TH1F*>&  , vector<TH1F*>&  ,
                     vector<string>  , vector<string>  , vector<string>  ) ;

  public:

  UATAnaDisplay(){;}
  virtual ~UATAnaDisplay(){;}

  void Init     ( UATAnaConfig& );
  void Yields   ( UATAnaConfig& , bool , bool = false);
  void CPlot    ( UATAnaConfig& , bool );
  void LimitCard( UATAnaConfig&  ); 

};

#endif
