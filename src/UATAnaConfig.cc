#include "../includes/UATAnaConfig.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


using namespace std;

// ---------------------- TOOLS ------------------------------------

vector<string> UATokenize(string input , char token){
  stringstream ssinput(input);
  string Element;
  vector<string> Elements;
  while (getline(ssinput, Element , token )) {
   Elements.push_back(Element);
  }
  return Elements;
}

void SetCutLevels(string sList, vector<string>& vList){
  vector<string> Var = UATokenize( sList, ':' );
  if ( Var.size() > 0 && Var.at(0) != "NULL" )
    for (vector<string>::iterator iVar = Var.begin() ; iVar != Var.end() ; ++iVar) vList.push_back( *iVar );
}

// ---------------------- TreeFormula_t ----------------------------

void TreeFormula_t::MakFormula (TTree* Tree){
  Formula = new TTreeFormula(NickName.c_str(),Expression.c_str(),Tree);
}
void TreeFormula_t::EvaFormula (){
  if ( Formula ) {
    Result_    = Formula->EvalInstance() ;
    bEvaluated = true ;
  } else {
    cout << "[TreeFormula_t::EvaFormula] Formula not associated to a Tree: " << NickName << endl ;
    Result_ = 0. ; 
  }
}
void TreeFormula_t::DelFormula (){
  bEvaluated = false ;
  delete Formula;
}
Float_t TreeFormula_t::Result(){
  if ( bEvaluated ) { 
    return Result_ ;
  } else {
    cout << "[TreeFormula_t::EvaFormula] Formula not evaluated: " << NickName << endl ;
    return 0.;
  }
} 


// ---------------------- ExtEff_t -------------------------------

void ExtEff_t::MakExtEff (TTree* Tree){
  X.MakFormula(Tree); 
  return ;
}

void ExtEff_t::EvaExtEff (){
  X.EvaFormula(); 
  Float_t xVal = X.Result();
  Int_t   xBin = -1 ;
  for ( int i = 1 ; i<= hEff->GetNbinsX() ; ++i ) {
    if ( xVal >= (hEff->GetXaxis())->GetBinLowEdge(i) &&  xVal < (hEff->GetXaxis())->GetBinUpEdge(i) ) xBin = i ;   
  }
  if (xBin > 0 ) { Result_ = hEff->GetBinContent(xBin) ; }
  else           { Result_ = 1.0 ; }
  bEvaluated = true;
  return ;
}


void ExtEff_t::DelExtEff (){
  bEvaluated = false ;
  X.DelFormula(); 
  return ;
}


Float_t ExtEff_t::Result( string& DataSet )  {
  if ( ! bEvaluated ) {  cout << "[ExtEff_t::Result] Formula not evaluated: " << NickName << endl ; return 1. ; }
  bool FoundDS = false ;
  for ( vector<string>::iterator itDS = DataSets.begin() ; itDS != DataSets.end() ; ++itDS ) {
    if ( *itDS == DataSet ) FoundDS = true ;
  } 
  if ( ! FoundDS ) return 1. ;  
  if      ( Method == 1 ) {
     if ( Result_ > 0 )   return 1.0/Result_ ;
     else                 return 0. ;
  } 
  else if ( Method == 2)  { 
     return Result_ ;
  }
  else                    return 0. ;
}

// ---------------------- ExtEffTH2_t -------------------------------

void ExtEffTH2_t::MakExtEffTH2 (TTree* Tree){
  X.MakFormula(Tree); 
  Y.MakFormula(Tree); 
  return ;
}

void ExtEffTH2_t::EvaExtEffTH2 (){
  X.EvaFormula(); 
  Y.EvaFormula(); 
  Float_t xVal = X.Result();
  Float_t yVal = Y.Result();
  Int_t   xBin = -1 ;
  Int_t   yBin = -1 ;
  for ( int i = 1 ; i<= hEff->GetNbinsX() ; ++i ) {
    if ( xVal >= (hEff->GetXaxis())->GetBinLowEdge(i) &&  xVal < (hEff->GetXaxis())->GetBinUpEdge(i) ) xBin = i ;   
  }
  for ( int i = 1 ; i<= hEff->GetNbinsY() ; ++i ) {
    if ( yVal >= (hEff->GetYaxis())->GetBinLowEdge(i) &&  yVal < (hEff->GetYaxis())->GetBinUpEdge(i) ) yBin = i ;   
  } 
  if (xBin > 0 && yBin > 0 ) { Result_ = hEff->GetBinContent(xBin,yBin) ; }
  else                       { Result_ = 1.0 ; }
    bEvaluated = true;
  return ;
}


void ExtEffTH2_t::DelExtEffTH2 (){
  bEvaluated = false ;
  X.DelFormula(); 
  Y.DelFormula(); 
  return ;
}


Float_t ExtEffTH2_t::Result( string& DataSet )  {
  if ( ! bEvaluated ) {  cout << "[ExtEffTH2_t::Result] Formula not evaluated: " << NickName << endl ; return 1. ; }
  bool FoundDS = false ;
  for ( vector<string>::iterator itDS = DataSets.begin() ; itDS != DataSets.end() ; ++itDS ) {
    if ( *itDS == DataSet ) FoundDS = true ;
  } 
  if ( ! FoundDS ) return 1. ;  
  if      ( Method == 1 ) {
     if ( Result_ > 0 )   return 1.0/Result_ ;
     else                 return 0. ;
  } 
  else if ( Method == 2)  { 
     return Result_ ;
  }
  else                    return 0. ;
}



// ---------------------- Reset() ----------------------------------


void UATAnaConfig::Reset(){

  TAnaName = "MyTAna" ;

  CmsEnergy = 8. ; 

  InputData.clear();

  for ( vector<DataGroup_t>::iterator iDG = DataGroups.begin() ; iDG !=  DataGroups.end() ; ++iDG ) {
    for ( vector<BaseGroup_t>::iterator iBG = (iDG->Members).begin() ; iBG !=  (iDG->Members).end() ; ++iBG ) {
      (iBG->Members).clear();
    }
    (iDG->Members).clear() ;
  }
  DataGroups.clear(); 
 
  CommonCuts.clear(); 
  CPExtraText.clear();

  SignalName = "signal";

  LimBinName = "1";
  HiggsMass  = -1 ;
  Systematic.clear() ;
  SyDDEstim.clear();
  StatMember.clear();
  StatPrefix = "CMS_" ;
  StatMiddle = ""     ;
  StatSuffix = "_bin1";  
  
  DrawRatio = false;
  DrawBgError = false;
  stackSignal = false;

  Func2Load.clear();
  Func2Init.clear();

}

// ---------------------- ReadCfg() --------------------------------

void UATAnaConfig::ReadCfg(TString CfgName) {

  ifstream Cfg ;
  Cfg.open ( CfgName );

  if(!Cfg) {
    cout << "Cannot open input file.\n";
    return ;
  }

  char str[5000];
  while( Cfg ) {

    Cfg.getline(str,5000);
    if(!Cfg) continue;
    istringstream iss(str);
//    if (iss.str().find("#") != string::npos ) continue;
    vector<string> Elements;
    do
    {
        string sub;
        iss >> sub;
        if(sub.size()>0) Elements.push_back(sub);
    } while (iss);
    if ( ! (Elements.size() > 0) ) continue;

    // TAnaName
    if ( Elements.at(0) == "TAnaName" ) TAnaName = Elements.at(1);

    // OutDir
    if ( Elements.at(0) == "OutDir" ) OutDir = Elements.at(1);

    // CmsEnergy
    if ( Elements.at(0) == "CmsEnergy" ) CmsEnergy = atof(Elements.at(1).c_str()) ;


    // InputData
    if ( Elements.at(0) == "InputData" ) {
       if  ( Elements.size() == 9 ) {
          InputData_t InDat;
          InDat.NickName  = Elements.at(1);
          InDat.Signal    = atoi(Elements.at(2).c_str()) ;
          InDat.Bkgd      = atoi(Elements.at(3).c_str()) ;
          InDat.Data      = atoi(Elements.at(4).c_str()) ;
          InDat.Lumi      = atof(Elements.at(5).c_str()) ;
          InDat.ScaleFac  = atof(Elements.at(6).c_str()) ;
          InDat.FileName  = Elements.at(7);
          InDat.TreeName  = Elements.at(8);
          InputData.push_back(InDat);
       }
    } 
    
    // stackSignal
    if ( Elements.at(0) == "StackSignal" ) {
      stackSignal = true;
    }

    // DataGroup
    if ( Elements.at(0) == "DataGroup" ) {
       int     level      = atoi(Elements.at(1).c_str()) ;  
       string  GroupName  = Elements.at(2);
       string  DataName   = Elements.at(3);
       if (level == 0) {  
         int iDGFound = -1 ; 
         int iDG      =  0 ; 
         for ( vector<DataGroup_t>::iterator itDG = DataGroups.begin() ; itDG !=  DataGroups.end() ; ++itDG , ++iDG ) {
           if ( itDG->GroupName == GroupName ) iDGFound = iDG ;
         } 
         if ( iDGFound >= 0 ) {
           int iBGFound = -1 ; 
           int iBG     =  0 ;
           for ( vector<BaseGroup_t>::iterator itBG = ((DataGroups.at(iDGFound)).Members).begin() 
                                            ; itBG != ((DataGroups.at(iDGFound)).Members).end() ; ++itBG , ++iBG ) {
             if ( itBG->BaseName == DataName ) iBGFound = iBG ;
           }  
           if ( iBGFound >= 0 ) {
             for ( int iMember = 5 ; iMember < (signed)Elements.size() ; ++iMember ) 
                 ((((DataGroups.at(iDGFound)).Members).at(iBGFound)).Members).push_back(Elements.at(iMember));
           } else {
              BaseGroup_t newBG ;
              newBG.BaseName = DataName ;
              newBG.Color    = atoi(Elements.at(4).c_str()) ;
              for (vector<InputData_t>::iterator itD = GetInputData()->begin() ; itD != GetInputData()->end() ; ++itD) {
                if ( Elements.at(5) == itD->NickName ) {
                  newBG.Signal   = itD->Signal;
                  newBG.Bkgd     = itD->Bkgd  ;
                  newBG.Data     = itD->Data  ;
                }
              }
              for ( int iMember = 5 ; iMember < (signed)Elements.size() ; ++iMember ) (newBG.Members).push_back(Elements.at(iMember));
              ((DataGroups.at(iDGFound)).Members).push_back(newBG);   
           }
         } else {
           BaseGroup_t newBG ;
           newBG.BaseName = DataName ;
           newBG.Color    = atoi(Elements.at(4).c_str()) ;
           for (vector<InputData_t>::iterator itD = GetInputData()->begin() ; itD != GetInputData()->end() ; ++itD) {
             if ( Elements.at(5) == itD->NickName ) {
               newBG.Signal   = itD->Signal;
               newBG.Bkgd     = itD->Bkgd  ;
               newBG.Data     = itD->Data  ;
             }
           }
           for ( int iMember = 5 ; iMember < (signed)Elements.size() ; ++iMember ) (newBG.Members).push_back(Elements.at(iMember));
           DataGroup_t newDG ;
           newDG.GroupName = GroupName ; 
           (newDG.Members).push_back(newBG); 
           DataGroups.push_back(newDG);
         }
       // Regrouping Groups
       } else if (level == 1) {
         bool First = true ;         
         DataGroup_t newDG ;
         newDG.GroupName = GroupName ;
         for ( vector<DataGroup_t>::iterator itDG = DataGroups.begin() ; itDG !=  DataGroups.end() ; ++itDG ) {
           for ( int iMember = 5 ; iMember < (signed)Elements.size() ; ++iMember ) {
             if ( itDG->GroupName == Elements.at(iMember) ) {
               if ( First ) {    
                 First = false ;
                 for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG  ) {
                   BaseGroup_t newBG ;
                   newBG.BaseName = itBG->BaseName ;
                   newBG.Color    = itBG->Color ;
                   newBG.Signal   = itBG->Signal;
                   newBG.Bkgd     = itBG->Bkgd  ;
                   newBG.Data     = itBG->Data  ;
                   for ( vector<string>::iterator itDN = (itBG->Members).begin() ; itDN != (itBG->Members).end() ; ++itDN ) { 
                     (newBG.Members).push_back( (*itDN) );
                   }
                   (newDG.Members).push_back(newBG);
                 }
               } else {
                 for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG  ) {
                   bool FoundBG = false ;
                   for ( vector<BaseGroup_t>::iterator itnewBG = (newDG.Members).begin() ; itnewBG != (newDG.Members).end() ; ++itnewBG  ) {
                     if ( itnewBG->BaseName == itBG->BaseName ) {
                       FoundBG = true ;
                       for ( vector<string>::iterator itDN = (itBG->Members).begin() ; itDN != (itBG->Members).end() ; ++itDN ) { 
                         (itnewBG->Members).push_back( (*itDN) );
                       }                       
                     }                     
                   }
                   if ( !FoundBG ) {
                      BaseGroup_t newBG ;
                      newBG.BaseName = itBG->BaseName ;
                      newBG.Color    = itBG->Color ;
                      newBG.Signal   = itBG->Signal;
                      newBG.Bkgd     = itBG->Bkgd  ;
                      newBG.Data     = itBG->Data  ;
                      for ( vector<string>::iterator itDN = (itBG->Members).begin() ; itDN != (itBG->Members).end() ; ++itDN ) { 
                       (newBG.Members).push_back( (*itDN) );
                      }
                     (newDG.Members).push_back(newBG);
                   }  
                 }  
               }
             }
           }  
         } 
         DataGroups.push_back(newDG); 
       } else {
         cout << "DataGroup level!=0 not implementd !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << endl;
       }
    }

    // OutTTree
    if ( Elements.at(0) == "OutTTree"  ) {
      OutTTree_t OTT ;
      OTT.TreeName  = Elements.at(1) ;
      OTT.Split     = atoi(Elements.at(2).c_str()) ; 
      OTT.SplitFrac = atof(Elements.at(3).c_str()) ; 
      OTT.CutName   = Elements.at(4) ;
      if ( Elements.size() >= 6) {
        vector<string> OTBL = UATokenize( Elements.at(5) , ':' );
        for (vector<string>::iterator iOTBL = OTBL.begin() ; iOTBL != OTBL.end() ; ++iOTBL ) {          
          (OTT.BranchList).push_back( *iOTBL ) ; 
        }  
      }  
      vector<string> OTTDL = UATokenize( Elements.at(5) , ':' );
      for (vector<string>::iterator iOTTD = OTTDL.begin() ; iOTTD != OTTDL.end() ; ++iOTTD ) {
        OTT.DataName = *iOTTD ; 
        OutTTree.push_back ( OTT ) ;
      }
    }

    // DataSetWghts
    if ( Elements.at(0) == "DataSetWght" ) {
      DataSetWght_t DSWght ;
      DSWght.NickName   = Elements.at(1) ;
      DSWght.Expression = Elements.at(2) ;
      //DSWght.Weight   = atof(Elements.at(2).c_str()) ; 
      for ( int iMember = 3 ; iMember < (signed)Elements.size() ; ++iMember ) (DSWght.DataSets).push_back(Elements.at(iMember));
      DataSetWghts.push_back ( DSWght ); 
    }

    // ScaleFactors
    if ( Elements.at(0) == "ScaleFactor" ) {
      ScaleFactor_t ScFac ;
      ScFac.NickName   = Elements.at(1) ;
      ScFac.Expression = Elements.at(2) ;
      ScFac.isCC       = atoi(Elements.at(3).c_str()) ; 
      vector<string> ScDaSet = UATokenize( Elements.at(4) , ':' );
      for (vector<string>::iterator iScDaSet = ScDaSet.begin() ; iScDaSet != ScDaSet.end() ; ++iScDaSet ) (ScFac.DataSets).push_back(*iScDaSet) ;
      vector<string> ScScNam = UATokenize( Elements.at(5) , ':' );
      for (vector<string>::iterator iScScNam = ScScNam.begin() ; iScScNam != ScScNam.end() ; ++iScScNam ) (ScFac.ScanName).push_back(*iScScNam) ;
      ScaleFactors.push_back ( ScFac ) ;
    }


    // TreeWeight
    if ( Elements.at(0) == "TreeWeight" ) {
      TreeWeight.NickName   = "TreeWeight" ;
      TreeWeight.Expression = Elements.at(1); 
    }

    // ExtEff
    if ( Elements.at(0) == "ExtEff" ) {
      ExtEff_t Eff;
      Eff.NickName     = Elements.at(1) ;
      Eff.Method       = atoi(Elements.at(2).c_str()) ;
      Eff.X.NickName   = "X" ;
      Eff.X.Expression = Elements.at(3) ;
      TFile* File      = new TFile(Elements.at(5).c_str(),"READ"); 
      TH1F*  hTmp      = (TH1F*) File->Get(Elements.at(6).c_str());
      gROOT->cd();
      Eff.hEff         = (TH1F*) hTmp->Clone();
      delete hTmp;
      File->Close();
      ExtEff.push_back(Eff);
    }

    if ( Elements.at(0) == "AppEff" ) {
      string NickName = Elements.at(1) ;
      bool FoundEff = false ;
      for ( vector<ExtEff_t>::iterator itEff = ExtEff.begin() ; itEff != ExtEff.end() ; ++itEff ) { 
        if ( itEff->NickName == NickName ) {
          FoundEff = true ;
          for ( int iMember = 2 ; iMember < (signed)Elements.size() ; ++iMember ) {
            bool FoundDS = false;
            for ( vector<string>::iterator itDS = (itEff->DataSets).begin() ; itDS != (itEff->DataSets).end() ; ++itDS ) {
              if ( *itDS == Elements.at(iMember) ) FoundDS = true ;
            } 
            if ( !FoundDS ) (itEff->DataSets).push_back(Elements.at(iMember));
          }
        } 
      }
      if ( ! FoundEff ) cout << "[UATanaConfig::Init()] AppEff error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    }

    // ExtEffTH2
    if ( Elements.at(0) == "ExtEffTH2" ) {
      ExtEffTH2_t Eff;
      Eff.NickName     = Elements.at(1) ;
      Eff.Method       = atoi(Elements.at(2).c_str()) ;
      Eff.X.NickName   = "X" ;
      Eff.X.Expression = Elements.at(3) ;
      Eff.Y.NickName   = "Y" ;
      Eff.Y.Expression = Elements.at(4) ;
      TFile* File      = new TFile(Elements.at(5).c_str(),"READ"); 
      TH2F*  hTmp      = (TH2F*) File->Get(Elements.at(6).c_str());
      gROOT->cd();
      Eff.hEff         = (TH2F*) hTmp->Clone();
      delete hTmp;
      File->Close();
      ExtEffTH2.push_back(Eff);
    }

    if ( Elements.at(0) == "AppEffTH2" ) {
      string NickName = Elements.at(1) ;
      bool FoundEff = false ;
      for ( vector<ExtEffTH2_t>::iterator itEff = ExtEffTH2.begin() ; itEff != ExtEffTH2.end() ; ++itEff ) { 
        if ( itEff->NickName == NickName ) {
          FoundEff = true ;
          for ( int iMember = 2 ; iMember < (signed)Elements.size() ; ++iMember ) {
            bool FoundDS = false;
            for ( vector<string>::iterator itDS = (itEff->DataSets).begin() ; itDS != (itEff->DataSets).end() ; ++itDS ) {
              if ( *itDS == Elements.at(iMember) ) FoundDS = true ;
            } 
            if ( !FoundDS ) (itEff->DataSets).push_back(Elements.at(iMember));
          }
        } 
      }
      if ( ! FoundEff ) cout << "[UATanaConfig::Init()] AppEffTH2 error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    }

    // TargetLumi
    if ( Elements.at(0) == "TargetLumi" ) TargetLumi =  atof(Elements.at(1).c_str()) ; 

    // CommonCuts
    if ( Elements.at(0) == "CommonCut" ) {
       if  ( Elements.size() >= 3 ) {
          CommonCut_t Cut;
          Cut.NickName   = Elements.at(1);
          Cut.Expression = Elements.at(2);
          if ( Elements.size() > 3 ) {
            for ( int iE = 3 ; iE < (signed) Elements.size() ; ++iE ) {
              Cut.CCTitle += Elements.at(iE) + " " ;
            }
          }
          CommonCuts.push_back(Cut);
       }
    }


    // CommonSign
    if ( Elements.at(0) == "CommonSign" ) {
      CommonSign.clear();
      CommonSign = UATokenize( Elements.at(1) , ':' );
    }

    // ScanCuts
    if ( Elements.at(0) == "ScanCut" ) {
      TreeFormula_t Cut;
      string ScanName = Elements.at(1);
      Cut.NickName    = Elements.at(2);
      Cut.Expression  = Elements.at(3);
      string SCTitle  ;
      if ( Elements.size() > 4 ) {
        for ( int iE = 4 ; iE < (signed) Elements.size() ; ++iE ) SCTitle += Elements.at(iE) + " " ;
      }
      int iSCFound = -1 ; 
      int iSC      =  0 ; 

      for ( vector<ScanCut_t>::iterator itSC = ScanCuts.begin() ; itSC !=  ScanCuts.end() ; ++itSC , ++iSC ) {
        if ( itSC->ScanName == ScanName ) iSCFound = iSC ;
      } 
      if ( iSCFound >= 0 ) {
        ((ScanCuts.at(iSCFound)).Cuts).push_back(Cut); 
      } else {
        ScanCut_t ScanCut ;
        ScanCut.ScanName = ScanName ;
        (ScanCut.Cuts).push_back(Cut); 
        ScanCuts.push_back(ScanCut); 
      }
    }

    // ScanSign
    if ( Elements.at(0) == "ScanSign" ) {
      string ScanName = Elements.at(1);
      vector<string> DataName = UATokenize( Elements.at(2) , ':' );
      for ( vector<ScanCut_t>::iterator itSC = ScanCuts.begin() ; itSC !=  ScanCuts.end() ; ++itSC ) {
        if ( itSC->ScanName == ScanName ) {
          for ( vector<string>::iterator itN = DataName.begin() ; itN != DataName.end() ; ++itN ) (itSC->SignList).push_back(*itN);
        }
      }
    }

    // CtrlPlots
    if ( Elements.at(0) == "CtrlPlot" ) {
      CtrlPlot_t CtrlPlot;
      CtrlPlot.varBins = false;
      CtrlPlot.NickName   = Elements.at(1);
      CtrlPlot.nBins      = atoi(Elements.at(2).c_str()) ;
      if( Elements.at(3).find(":") != std::string::npos) {
        CtrlPlot.varBins = true;
        vector<string> bins = UATokenize( Elements.at(3) , ':' );
        for ( vector<string>::iterator itS = bins.begin() ; itS !=  bins.end() ; ++itS ) {
          CtrlPlot.xBins.push_back(atof((*itS).c_str()));            
        }
      }
      else {
        CtrlPlot.xMin       = atof(Elements.at(3).c_str()) ;
        CtrlPlot.xMax       = atof(Elements.at(4).c_str()) ;
      }
      CtrlPlot.kLogY      = atoi(Elements.at(5).c_str()) ;
      CtrlPlot.Expression = Elements.at(6);
      SetCutLevels(Elements.at(7),CtrlPlot.CCNickName);
      SetCutLevels(Elements.at(8),CtrlPlot.SCNickName);
      if ( Elements.size() > (signed) 9 ) {
        CtrlPlot.XaxisTitle = "";
        for ( int iE = 9 ; iE < (signed) Elements.size() ; ++iE ) CtrlPlot.XaxisTitle += Elements.at(iE) + " " ; 
      } else {
        CtrlPlot.XaxisTitle = Elements.at(1);
      }
      CtrlPlots.push_back( CtrlPlot );    
      //cout << (CtrlPlot.NickName).c_str() << " " << (CtrlPlot.Expression).c_str() << endl;
    } 
    
    // GroupPlot
    if ( Elements.at(0) == "GroupPlot" ) {
      GroupPlot_t GroupPlot;
      GroupPlot.NickName   = Elements.at(1);
      vector<string> varLegend = UATokenize( Elements.at(2) , ';' );
      for( vector<string>::iterator itS = varLegend.begin() ; itS !=  varLegend.end() ; ++itS ) {
          vector<string> varLegendElement = UATokenize( *itS, ':' );
          if( varLegendElement.size() != 3 )
              cout << "Error: GroupPlot doesn't have correct structure (legend1:nickname1:color1;legend2:nickname2:color2;....)" << endl;
          GroupPlot.plotLegend.push_back( varLegendElement[0] );
          GroupPlot.plotNickname.push_back( varLegendElement[1] );
          GroupPlot.plotColor.push_back( atoi(varLegendElement[2].c_str()) );
      }
      GroupPlot.kLogY = atoi(Elements.at(3).c_str()) ;
      if ( Elements.size() > (signed) 4 ) {
        GroupPlot.XaxisTitle = "";
        for ( int iE = 4 ; iE < (signed) Elements.size() ; ++iE ) GroupPlot.XaxisTitle += Elements.at(iE) + " " ; 
      } else {
        GroupPlot.XaxisTitle = GroupPlot.NickName;
      }
      GroupPlots.push_back( GroupPlot );    
    } 

    // PrintEvt
    if ( Elements.at(0) == "PrintEvt" ) {
      PrintEvt_t  PrintEvt;
      PrintEvt.NickName   = Elements.at(1);
      PrintEvt.Type       = atoi(Elements.at(2).c_str()) ;
      if ( PrintEvt.Type == 0 ) {
         PrintEvt.Expression = Elements.at(6);
      } else {
         PrintEvt.Expression = "1" ; 
      } 
      SetCutLevels(Elements.at(3),PrintEvt.CCNickName);
      for ( vector<string>::iterator itPEC = (PrintEvt.CCNickName).begin() ; itPEC != (PrintEvt.CCNickName).end() ; ++itPEC ) cout << *itPEC << endl ;
      SetCutLevels(Elements.at(4),PrintEvt.SCNickName);
      vector<string> VarName = UATokenize( Elements.at(5) , ':' );
      for ( vector<string>::iterator itN = VarName.begin() ; itN != VarName.end() ; ++itN ) {
//         cout << *itN << endl ;
        TreeFormula_t Var;
        Var.NickName   = *itN ;
        Var.Expression = *itN ; 
        (PrintEvt.VarList).push_back(Var);
      }
      PrintEvts.push_back( PrintEvt );
    }

    if ( Elements.at(0) == "CPExtraText" ) {
      string ExtraText ;
      for ( int iE = 1 ; iE < (signed) Elements.size() ; ++iE )  ExtraText += Elements.at(iE) + " " ; 
      CPExtraText.push_back(ExtraText) ;
    }

    if ( Elements.at(0) == "CutLines" ) {
      CutLines_t CL;
      CL.CPNickName = Elements.at(1);
      for ( int iE = 2 ; iE < (signed) Elements.size() ; ++iE ) (CL.CutLines).push_back(Elements.at(iE))  ; 
      CutLines.push_back(CL);
    }

    if ( Elements.at(0) == "SignalName" ) {
       SignalName = Elements.at(1); 
      for ( int iE = 2 ; iE < (signed) Elements.size() ; ++iE ) SignalName += " " + Elements.at(iE) ;
    }
    
     if ( Elements.at(0) == "SignalNames" ) {
       SignalName_t signalName;
       signalName.NickName = Elements.at(1); 
       signalName.Legend = Elements.at(2);
       
       for ( int iE = 3 ; iE < (signed) Elements.size() ; ++iE ) signalName.Legend += " " + Elements.at(iE) ;
       SignalNames.push_back(signalName);
    }
  
    // LimitBinName
    if ( Elements.at(0) == "LimBinName") {
      LimBinName = Elements.at(1) ;
    }

    // HiggsMass
    if ( Elements.at(0) == "HiggsMass") {
      HiggsMass = atof(Elements.at(1).c_str()) ;
    }

    // Sytematics
    if ( Elements.at(0) == "Systematic" ) {
      string Name =  Elements.at(1) ;
      int iSFound = -1;
      int iS      =  0;
      for ( vector<Systematic_t>::iterator itS = Systematic.begin() ; itS != Systematic.end() ; ++itS , ++iS ) {
        if ( itS->systName == Name ) iSFound = iS ;
      }
      if ( iSFound >= 0 ) {
        float Val  = atof((Elements.at(3)).c_str()) ;
        vector<string> Member = UATokenize( Elements.at(4) , ':' );
        for ( int iM = 0 ; iM < (signed) Member.size() ; ++iM ) {
          ((Systematic.at(iSFound)).systVal).push_back(Val);
          ((Systematic.at(iSFound)).systMember).push_back(Member.at(iM)) ;
        }
      } else {
        Systematic_t Syst ;
        Syst.systName = Elements.at(1) ;
        Syst.systType = Elements.at(2) ;
        float Val  = atof((Elements.at(3)).c_str()) ;
        vector<string> Member = UATokenize( Elements.at(4) , ':' );
        for ( int iM = 0 ; iM < (signed) Member.size() ; ++iM ) {
          (Syst.systVal).push_back(Val);
          (Syst.systMember).push_back(Member.at(iM)) ;
        }
        Systematic.push_back(Syst);
      }
    }

    // SyDDEstim
    if ( Elements.at(0) == "SyDDEstim" ) {
      SyDDEstim_t SyDDE ;
      SyDDE.SyDDEName  = Elements.at(1) ;
      SyDDE.SyDDEType  = Elements.at(2) ;
      SyDDE.SyDDEmass  = atof((Elements.at(3)).c_str()) ;
      SyDDE.SyDDEdctrl = atof((Elements.at(4)).c_str()) ;
      SyDDE.SyDDEderr  = atof((Elements.at(5)).c_str()) ;
      vector<string> Cards = UATokenize( Elements.at(6) , ':' );
      for ( int iC = 0 ; iC < (signed) Cards.size() ; ++iC ) (SyDDE.SyDDECards).push_back(Cards.at(iC)) ;
      vector<string> Member = UATokenize( Elements.at(7) , ':' );
      for ( int iM = 0 ; iM < (signed) Member.size() ; ++iM ) (SyDDE.SyDDEMember).push_back(Member.at(iM)) ;
      SyDDEstim.push_back(SyDDE);
    }

    //StatMember
    if ( Elements.at(0) == "StatMember" ) {
      vector<string> Member = UATokenize( Elements.at(1) , ':' );
      for ( int iM = 0 ; iM < (signed) Member.size() ; ++iM ) StatMember.push_back(Member.at(iM)) ;
    }

    // StatPrefix
    if ( Elements.at(0) == "StatPrefix" ) {
      StatPrefix = Elements.at(1) ;
    }

    // StatMiddle
    if ( Elements.at(0) == "StatMiddle" ) {
      StatMiddle = Elements.at(1) ;
    }

    // StatSuffix
    if ( Elements.at(0) == "StatSuffix" ) {
      StatSuffix = Elements.at(1) ;
    }

    // MaskData in LimitCards
    if (  Elements.at(0) == "MaskData" ) {
      MaskData = atoi(Elements.at(1).c_str()) ;
    }

    // External Functions to be loaded
    if (  Elements.at(0) == "Func2Load" ) {
      vector<string> Member = UATokenize( Elements.at(1) , ':' );
      for ( int iM = 0 ; iM < (signed) Member.size() ; ++iM ) Func2Load.push_back(Member.at(iM)) ;
    }

    // External Functions to be initialized
    if (  Elements.at(0) == "Func2Init" ) {
      Func2Init_t F2Init ;
      F2Init.DataName = Elements.at(1);
      F2Init.Command  = Elements.at(2);
      Func2Init.push_back( F2Init ) ;
    }



  } 


  return;
}


// ---------------------- Print() -------------------------------

void UATAnaConfig::Print(){

  cout << "----------------------------------------------------------" << endl;
  cout << "----------------[UATAnaConfig::Print()] ------------------" << endl;
  cout << "----------------------------------------------------------" << endl;

  cout << "----------------------------------------------------------" << endl;
  for ( vector<InputData_t>::iterator iD = InputData.begin() ; iD != InputData.end() ; ++iD ) {
    cout << "InputData  = " << iD->NickName << endl;
    cout << " --> FileName: " << iD->FileName << endl;
    cout << " --> TreeName: " << iD->TreeName << endl;
    cout << " --> Signal: "<<iD->Signal<<" Bkgd: "<< iD->Bkgd << " Data : "<< iD->Data << endl;
  }

  cout << "----------------------------------------------------------" << endl;



}


 
