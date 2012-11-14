
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


class BaseGroup_t {
  public:
  BaseGroup_t(){;}
  virtual ~BaseGroup_t(){;}
  string         BaseName ;
  Int_t          Color    ;
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

class OutTTree_t    {
  public:
  OutTTree_t(){;}
  virtual ~OutTTree_t(){;}

  string    TreeName  ;
  Bool_t    Split     ;
  Float_t   SplitFrac ;
  string    CutName   ;
  string    DataName  ; 
  vector<string> BranchList ;    

  string    OutNOne  ;
  string    OutNTwo  ;  
  TFile*    OutFOne  ;
  TFile*    OutFTwo  ;  
  TTree*    OutTOne  ;
  TTree*    OutTTwo  ; 
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

class DataSetWght_t : public TreeFormula_t {
  public:
//  string         NickName ;
//  Float_t        Weight   ; 
  vector<string> DataSets ; 
};


class CommonCut_t : public TreeFormula_t {
  public:
  string   CCTitle;               
};

class ScanCut_t {
  public:
  ScanCut_t(){;}
  virtual ~ScanCut_t(){;}
  string                 ScanName ;
  vector<TreeFormula_t>  Cuts     ;
  vector<string>         SCTitle  ;
  vector<string>         SignList ;
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
  Int_t          kLogY      ;
  vector<string> CCNickName ;
  vector<string> SCNickName ;
  string         XaxisTitle ;
};

class PrintEvt_t : public TreeFormula_t {
  public:
  PrintEvt_t(){;}
  virtual ~PrintEvt_t(){;}
  int Type                       ;
  vector<TreeFormula_t> VarList  ; 
  vector<string> CCNickName      ;
  vector<string> SCNickName      ;
};  


class ExtEff_t {
  public: 
  ExtEff_t(){;}
  virtual ~ExtEff_t(){;}
  string        NickName ;
  Int_t         Method   ;  // 1: correct data / 2: Scale MC / 3: Reject MC    
  TH1F*         hEff     ;
  TreeFormula_t X        ; 
  vector<string> DataSets;
  void MakExtEff (TTree *);
  void EvaExtEff ()   ;
  void DelExtEff ()   ;
  Float_t Result(string&) ;
  private:
  bool          bEvaluated ;
  Float_t       Result_    ; 
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

class Systematic_t {
  public:
  string systName ;
  string systType ;
  vector <float>  systVal  ;
  vector <string> systMember ;
};

class SyDDEstim_t {
  public:
  string SyDDEName ;
  string SyDDEType ;
  float  SyDDEmass ;
  float  SyDDEdctrl;
  float  SyDDEderr;
  vector <string> SyDDECards  ;
  vector <string> SyDDEMember ;
};

class CutLines_t {
  public:
  string CPNickName ;
  vector<string> CutLines ;
};


class UATAnaConfig {

  private:

  string                 TAnaName   ;  
  string                 OutDir     ;

  vector<InputData_t>    InputData  ;
  vector<DataGroup_t>    DataGroups ;

  vector<OutTTree_t>     OutTTree   ;

  vector<DataSetWght_t>  DataSetWghts ;
  TreeFormula_t          TreeWeight ; 
  vector<ExtEff_t>       ExtEff     ;
  vector<ExtEffTH2_t>    ExtEffTH2  ;
  Float_t                TargetLumi ;

  vector<CommonCut_t>    CommonCuts ;  
  vector<string>         CommonSign ;  

  vector<ScanCut_t>      ScanCuts   ;

  vector<CtrlPlot_t>     CtrlPlots  ;
  vector<PrintEvt_t>     PrintEvts  ;
  vector<string>         CPExtraText;
  vector<CutLines_t>     CutLines   ;

  string                 SignalName;

  string                 LimBinName;
  float                  HiggsMass ;
  // Systematic errors
  vector<Systematic_t>   Systematic;
  vector<SyDDEstim_t>    SyDDEstim;
  // Statisctic errors
  vector<string>       StatMember;
  string               StatPrefix;
  string               StatMiddle;
  string               StatSuffix;

   bool                MaskData;
   bool		       DrawRatio;
   bool	               DrawBgError;


  public:

  UATAnaConfig(){ Reset(); MaskData = false ; }
  virtual ~UATAnaConfig(){ Reset() ; }

  void Reset();
  void ReadCfg(TString); 
  void Print();

  // Getters:
  string                  GetTAnaName()       { return TAnaName      ; } 
  string                  GetOutDir()         { return OutDir        ; } 
  vector<InputData_t>*    GetInputData()      { return &InputData    ; }
  vector<DataGroup_t>*    GetDataGroups()     { return &DataGroups   ; }
  vector<OutTTree_t>*     GetOutTTree()       { return &OutTTree     ; } 
  vector<DataSetWght_t>*  GetDataSetWghts()   { return &DataSetWghts ; }
  TreeFormula_t*          GetTreeWeight()     { return &TreeWeight   ; }
  vector<ExtEff_t>*       GetExtEff()         { return &ExtEff       ; }
  vector<ExtEffTH2_t>*    GetExtEffTH2()      { return &ExtEffTH2    ; }
  Float_t                 GetTargetLumi()     { return TargetLumi    ; }           
  vector<CommonCut_t>*    GetCommonCuts()     { return &CommonCuts   ; }
  vector<string>*         GetCommonSign()     { return &CommonSign   ; }
  vector<ScanCut_t>*      GetScanCuts()       { return &ScanCuts     ; }
  vector<CtrlPlot_t>*     GetCtrlPlots()      { return &CtrlPlots    ; }
  vector<PrintEvt_t>*     GetPrintEvt()       { return &PrintEvts   ; } 
  vector<string>*         GetCPExtraText()    { return &CPExtraText ; }
  vector<CutLines_t>*     GetCutLines()       { return &CutLines    ; } 
  bool                    GetDrawBgError()    { return DrawBgError ; }
  bool                    GetDrawRatio()      { return DrawRatio ; }
  string                 GetSignalName()      { return SignalName  ; }
  string                  GetLimBinName()     { return LimBinName    ; }
  float                  GetHiggsMass()       { return HiggsMass     ; }
  vector<Systematic_t>*   GetSystematic()     { return &Systematic   ; }
  vector<SyDDEstim_t>*   GetSyDDEstim()       { return &SyDDEstim    ; }
  vector<string>*        GetStatMember()      { return  &StatMember  ; }
  string                 GetStatPrefix()      { return  StatPrefix   ; }
  string                 GetStatMiddle()      { return  StatMiddle   ; }
  string                 GetStatSuffix()      { return  StatSuffix   ; }
  bool                   GetMaskData()        { return  MaskData     ; }  

  void                   SetDrawRatio(bool In)       { DrawRatio = In ; }
  void 			 SetDrawBgError(bool In)     { DrawBgError = In ; }

};

#endif 

