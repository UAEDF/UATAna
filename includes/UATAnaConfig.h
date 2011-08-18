
#ifndef __UATAnaConfig_H__
#define __UATAnaConfig_H__

#include <TROOT.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>
#include <TTree.h>
#include <TTreeFormula.h>

#include <TString.h>
#include <string>
#include <vector>
using namespace std;

class InputData_t {
  public:
  InputData_t(){;}
  virtual ~InputData_t(){;}
  string NickName  ;
  string FileName  ;
  string TreeName  ;
  Bool_t  Signal    ;
  Bool_t  Bkgd      ;
  Bool_t  Data      ;
  Float_t Lumi      ;
  Float_t ScaleFac  ;
};

class DataSetWght_t {
  public:
  string         NickName ;
  Float_t        Weight   ; 
  vector<string> DataSets ; 
};

class BaseGroup_t {
  public:
  BaseGroup_t(){;}
  virtual ~BaseGroup_t(){;}
  string         BaseName ;
  Bool_t         Signal   ;
  Bool_t         Bkgd     ;
  Bool_t         Data     ;
  vector<string> Members  ;  
};

class DataGroup_t {
  public:
  DataGroup_t(){;}
  virtual ~DataGroup_t(){;}

  string               GroupName; 
  vector<BaseGroup_t>  Members;
};

class TreeFormula_t {
  public:
  TreeFormula_t(){ bEvaluated = false ;}
  virtual ~TreeFormula_t(){;}
  string        NickName   ;
  string        Expression ;
  private:
  TTreeFormula* Formula    ;        
  bool          bEvaluated ;
  Float_t       Result_    ; 
  public:
  void MakFormula (TTree *);
  void EvaFormula ()       ;
  void DelFormula ()       ;
  Float_t Result()         ;
};

class ScanCut_t {
  public:
  ScanCut_t(){;}
  virtual ~ScanCut_t(){;}
  string                 ScanName ;
  vector<TreeFormula_t>  Cuts     ;
};

class ScanFlow_t {
  public:
  string        ScanName ;
  vector<TH1F*> CutFlow  ; 
}; 

class CtrlPlot_t : public TreeFormula_t {
  public:
  CtrlPlot_t(){;}
  virtual ~CtrlPlot_t(){;}
  Int_t          nBins      ;
  Float_t        xMin       ;
  Float_t        xMax       ;
  Bool_t         kLogY      ;
  vector<string> CCNickName ;
  vector<string> SCNickName ;
};


class ExtEffTH2_t   {
  public: 
  ExtEffTH2_t(){;}
  virtual       ~ExtEffTH2_t(){;}
  string        NickName ;
  Int_t         Method   ;  // 1: correct data / 2: Scale MC / 3: Reject MC    
  TH2F*         hEff     ;
  TreeFormula_t X        ; 
  TreeFormula_t Y        ; 
  vector<string> DataSets;
  void MakExtEffTH2 (TTree *);
  void EvaExtEffTH2 ()   ;
  void DelExtEffTH2 ()   ;
  Float_t Result(string&) ;
  private:
  bool          bEvaluated ;
  Float_t       Result_    ; 
};

class UATAnaConfig {

  private:

  vector<InputData_t>    InputData    ;
  vector<DataGroup_t>    DataGroups   ;

  vector<DataSetWght_t>  DataSetWghts ;
  TreeFormula_t          TreeWeight ; 
  vector<ExtEffTH2_t>    ExtEffTH2  ;
  Float_t                TargetLumi ;

  vector<TreeFormula_t>  CommonCuts ;  
  vector<ScanCut_t>      ScanCuts   ;

  vector<CtrlPlot_t>     CtrlPlots  ;
 
  public:

  UATAnaConfig(){ Reset(); }
  virtual ~UATAnaConfig(){ Reset() ; }

  void Reset();
  void ReadCfg(TString); 
  void Print();

  // Getters:
  vector<InputData_t>*    GetInputData()      { return &InputData    ; }
  vector<DataGroup_t>*    GetDataGroups()     { return &DataGroups   ; }
  vector<DataSetWght_t>*  GetDataSetWghts()   { return &DataSetWghts ; }
  TreeFormula_t*          GetTreeWeight()     { return &TreeWeight   ; }
  vector<ExtEffTH2_t>*    GetExtEffTH2()      { return &ExtEffTH2    ; }
  Float_t                 GetTargetLumi()     { return TargetLumi    ; }           
  vector<TreeFormula_t>*  GetCommonCuts()     { return &CommonCuts   ; }
  vector<ScanCut_t>*      GetScanCuts()       { return &ScanCuts     ; }
  vector<CtrlPlot_t>*     GetCtrlPlots()      { return &CtrlPlots    ; }

};

#endif 

