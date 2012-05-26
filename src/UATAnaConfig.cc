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

  InputData.clear();

  for ( vector<DataGroup_t>::iterator iDG = DataGroups.begin() ; iDG !=  DataGroups.end() ; ++iDG ) {
    for ( vector<BaseGroup_t>::iterator iBG = (iDG->Members).begin() ; iBG !=  (iDG->Members).end() ; ++iBG ) {
      (iBG->Members).clear();
    }
    (iDG->Members).clear() ;
  }
  DataGroups.clear(); 
 
  CommonCuts.clear(); 


  LimBinName = "1";
  HiggsMass  = -1 ;
  Systematic.clear() ;
  SyDDEstim.clear();
  StatMember.clear();
  StatPrefix = "CMS_" ;
  StatMiddle = ""     ;
  StatSuffix = "_bin1";  


}

// ---------------------- ReadCfg() --------------------------------

void UATAnaConfig::ReadCfg(TString CfgName) {

  ifstream Cfg ;
  Cfg.open ( CfgName );

  if(!Cfg) {
    cout << "Cannot open input file.\n";
    return ;
  }

  char str[2000];
  while( Cfg ) {

    Cfg.getline(str,2000);
    if(!Cfg) continue;
    istringstream iss(str);
    if (iss.str().find("#") != string::npos ) continue;
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

    // TreeWeight
    if ( Elements.at(0) == "TreeWeight" ) {
      TreeWeight.NickName   = "TreeWeight" ;
      TreeWeight.Expression = Elements.at(1); 
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
      CtrlPlot.NickName   = Elements.at(1);
      CtrlPlot.nBins      = atoi(Elements.at(2).c_str()) ;
      CtrlPlot.xMin       = atof(Elements.at(3).c_str()) ;
      CtrlPlot.xMax       = atof(Elements.at(4).c_str()) ;
      CtrlPlot.kLogY      = atoi(Elements.at(5).c_str()) ;
      CtrlPlot.Expression = Elements.at(6);
      SetCutLevels(Elements.at(7),CtrlPlot.CCNickName);
      SetCutLevels(Elements.at(8),CtrlPlot.SCNickName);
      CtrlPlots.push_back( CtrlPlot );    
      //cout << (CtrlPlot.NickName).c_str() << " " << (CtrlPlot.Expression).c_str() << endl;
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


 
