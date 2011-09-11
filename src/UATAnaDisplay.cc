
#include "../includes/UATAnaDisplay.h"
#include <TROOT.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TLegend.h>
#include <TText.h>
#include <TMath.h>

//----------------------------------- Init() ------------------------------------------

void UATAnaDisplay::Init ( UATAnaConfig& Cfg ) {

  string fileName = Cfg.GetOutDir() + "/" + Cfg.GetTAnaName()+".root";
  File = new TFile(fileName.c_str(),"OPEN");

  vector<TH1F*> CCflow_      ; 
  vector<TH1F*> CCflowGroup_ ; 
  
  vector<ScanFlow_t> SCflow_      ; 
  vector<ScanFlow_t> SCflowGroup_ ; 

  vector<TH1F*> PlotCC_ ;
  vector<TH1F*> PlotSC_ ;
  vector<TH1F*> PlotCCGroup_ ;
  vector<TH1F*> PlotSCGroup_ ;

  // CommonCuts cutflow histograms
  
  File->cd();
  gDirectory->cd   ("CutFlow");
  gDirectory->cd   ("CommonCuts");

  gDirectory->cd   ("InputData");
  for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD) {
    TString HistName = "CutFlow_"+itD->NickName ;
    CCflow_.push_back( (TH1F*) gDirectory->Get(HistName) ) ;
  }
  gDirectory->cd("../");

  gDirectory->cd   ("DataGroups");
  for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
    for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG  ) {
      TString HistName = "CutFlow_"+itDG->GroupName+"_"+itBG->BaseName ;
      CCflowGroup_.push_back( (TH1F*) gDirectory->Get(HistName) ) ;
    }
  }
  
  // ScanCuts cutflow histograms
  File->cd();
  gDirectory->cd   ("CutFlow");

  gDirectory->cd   ("ScanCuts");
  gDirectory->cd   ("InputData");
  for ( vector<ScanCut_t>::iterator iSCG = (Cfg.GetScanCuts())->begin() ; iSCG != (Cfg.GetScanCuts())->end() ; ++iSCG) {
    gDirectory->cd   ((iSCG->ScanName).c_str());
    ScanFlow_t ScanFlow ;
    ScanFlow.ScanName = iSCG->ScanName ;
    for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD) {
      TString HistName = "ScanFlow_"+iSCG->ScanName+"_"+itD->NickName ; 
      (ScanFlow.CutFlow).push_back( (TH1F*) gDirectory->Get(HistName) ) ;
    }
    SCflow_.push_back( ScanFlow ) ;
    gDirectory->cd("../");
  } 
  gDirectory->cd("../");

  gDirectory->cd   ("DataGroups");
  for ( vector<ScanCut_t>::iterator iSCG = (Cfg.GetScanCuts())->begin() ; iSCG != (Cfg.GetScanCuts())->end() ; ++iSCG) {
    gDirectory->cd   ((iSCG->ScanName).c_str());
    ScanFlow_t ScanFlow ;
    ScanFlow.ScanName = iSCG->ScanName ;
    for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
      for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG  ) {
        TString HistName = "ScanFlow_"+iSCG->ScanName+"_"+itDG->GroupName+"_"+itBG->BaseName ; 
        (ScanFlow.CutFlow).push_back( (TH1F*) gDirectory->Get(HistName) ) ;
      }
    }
    SCflowGroup_.push_back( ScanFlow ) ;
    gDirectory->cd("../");
  }

  // CtrlPlots histograms 

  for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
    File->cd();
    gDirectory->cd   ("CtrlPlot");
    // CommonCuts
    gDirectory->cd   ("CommonCuts");
    for ( vector<string>::iterator itCC = (itCP->CCNickName).begin() ; itCC != (itCP->CCNickName).end() ; ++itCC ) {
      gDirectory->cd   ("InputData");
      gDirectory->cd   ((*itCC).c_str());
      for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD) {
        TString HistName = "PlotCC_"+itCP->NickName+"_"+*itCC+"_"+itD->NickName;
        PlotCC_.push_back ( (TH1F*) gDirectory->Get(HistName) ) ;
      } 
      gDirectory->cd   ("../../");

      gDirectory->cd   ("DataGroups");
      gDirectory->cd   ((*itCC).c_str());
      for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
        for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG  ) {
          TString HistName = "PlotCC_"+itCP->NickName+"_"+*itCC+"_"+itDG->GroupName+"_"+itBG->BaseName ;
          PlotCCGroup_.push_back ( (TH1F*) gDirectory->Get(HistName) ) ;

        }
      } 

      gDirectory->cd   ("../../");

    }

    

    // ScanCuts
    gDirectory->cd   ("../");
    gDirectory->cd   ("ScanCuts");  
    for ( vector<string>::iterator itSC = (itCP->SCNickName).begin() ; itSC != (itCP->SCNickName).end() ; ++itSC ) {
      for ( vector<ScanCut_t>::iterator iSCG = (Cfg.GetScanCuts())->begin() ; iSCG != (Cfg.GetScanCuts())->end() ; ++iSCG) { 
        gDirectory->cd   ("InputData"); 
        gDirectory->cd   ((iSCG->ScanName).c_str());
        gDirectory->cd   ((*itSC).c_str());
        for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD) {
          TString HistName = "PlotSC_"+iSCG->ScanName+"_"+itCP->NickName+"_"+*itSC+"_"+itD->NickName;
          PlotSC_.push_back ( (TH1F*) gDirectory->Get(HistName) ) ;
        }
        gDirectory->cd   ("../../../");

        gDirectory->cd   ("DataGroups");
        gDirectory->cd   ((iSCG->ScanName).c_str());
        gDirectory->cd   ((*itSC).c_str());
        for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
          for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG  ) {
            TString HistName = "PlotSC_"+iSCG->ScanName+"_"+itCP->NickName+"_"+*itSC+"_"+itDG->GroupName+"_"+itBG->BaseName ;
            PlotSCGroup_.push_back ( (TH1F*) gDirectory->Get(HistName) ) ;
          }
        }
        gDirectory->cd   ("../../../");  

      }
    }

  }

  // Clone everyhting in gROOT:
 
  gROOT->cd();

  for (vector<TH1F*>::iterator it = CCflow_.begin() ; it != CCflow_.end() ; ++it )  CCflow.push_back ( (TH1F*) (*it)->Clone() ) ; 
  for (vector<TH1F*>::iterator it = CCflowGroup_.begin() ; it != CCflowGroup_.end() ; ++it ) CCflowGroup.push_back ( (TH1F*) (*it)->Clone() ) ; 

  for (vector<ScanFlow_t>::iterator it = SCflow_.begin() ; it != SCflow_.end() ; ++it ) {
    ScanFlow_t ScanFlow ;
    ScanFlow.ScanName = it->ScanName ;
    for (vector<TH1F*>::iterator itH = (it->CutFlow).begin() ; itH != (it->CutFlow).end() ; ++itH ) (ScanFlow.CutFlow).push_back( (TH1F*) (*itH)->Clone() ) ;
    SCflow.push_back ( ScanFlow ) ;
  } 
  for (vector<ScanFlow_t>::iterator it = SCflowGroup_.begin() ; it != SCflowGroup_.end() ; ++it ) {
    ScanFlow_t ScanFlow ;
    ScanFlow.ScanName = it->ScanName ;
    for (vector<TH1F*>::iterator itH = (it->CutFlow).begin() ; itH != (it->CutFlow).end() ; ++itH ) (ScanFlow.CutFlow).push_back( (TH1F*) (*itH)->Clone() ) ;
    SCflowGroup.push_back ( ScanFlow ) ; 
  }

  for (vector<TH1F*>::iterator it = PlotCC_.begin() ; it != PlotCC_.end() ; ++it )  PlotCC.push_back ( (TH1F*) (*it)->Clone() ) ; 
  for (vector<TH1F*>::iterator it = PlotSC_.begin() ; it != PlotSC_.end() ; ++it )  PlotSC.push_back ( (TH1F*) (*it)->Clone() ) ; 
  for (vector<TH1F*>::iterator it = PlotCCGroup_.begin() ; it != PlotCCGroup_.end() ; ++it )  PlotCCGroup.push_back ( (TH1F*) (*it)->Clone() ) ; 
  for (vector<TH1F*>::iterator it = PlotSCGroup_.begin() ; it != PlotSCGroup_.end() ; ++it )  PlotSCGroup.push_back ( (TH1F*) (*it)->Clone() ) ; 

  // Clean Temp objects 
  for (vector<TH1F*>::iterator it = CCflow_.begin() ; it != CCflow_.end() ; ++it ) delete (*it) ;
  for (vector<TH1F*>::iterator it = CCflowGroup_.begin() ; it != CCflowGroup_.end() ; ++it ) delete (*it) ;
  CCflow_.clear();
  CCflowGroup_.clear();


  for (vector<ScanFlow_t>::iterator it = SCflow_.begin() ; it != SCflow_.end() ; ++it ) {
    for (vector<TH1F*>::iterator itH = (it->CutFlow).begin() ; itH != (it->CutFlow).end() ; ++itH )  delete (*itH) ;
    (it->CutFlow).clear();  
  }
  SCflow_.clear();

  for (vector<ScanFlow_t>::iterator it = SCflowGroup_.begin() ; it != SCflowGroup_.end() ; ++it ) {
    //for (vector<TH1F*>::iterator itH = (it->CutFlow).begin() ; itH != (it->CutFlow).end() ; ++itH )  delete (*itH) ;
    (it->CutFlow).clear();  
  }
  SCflowGroup_.clear();


  for (vector<TH1F*>::iterator it = PlotCC_.begin() ; it != PlotCC_.end() ; ++it ) delete (*it) ;
  for (vector<TH1F*>::iterator it = PlotSC_.begin() ; it != PlotSC_.end() ; ++it ) delete (*it) ;
  for (vector<TH1F*>::iterator it = PlotCCGroup_.begin() ; it != PlotCCGroup_.end() ; ++it ) delete (*it) ;
  for (vector<TH1F*>::iterator it = PlotSCGroup_.begin() ; it != PlotSCGroup_.end() ; ++it ) delete (*it) ;
  PlotCC_.clear();
  PlotSC_.clear();
  PlotCCGroup_.clear();
  PlotSCGroup_.clear();

  // Close File
  File->Close(); 

  return;
}   

//----------------------------------- PlotStack() -------------------------------------

void UATAnaDisplay::PlotStack( string  DataSet , string  CutGroup , string  CutLevel , bool  kLogY ,
                               vector<TH1F*>&  vData  , vector<TH1F*>&  vSignal  , vector<TH1F*>&  vBkgd ,
                               vector<string>  vLData , vector<string>  vLSignal , vector<string>  vLBkgd ) {

  TString  CanName = DataSet+"_"+CutGroup ;
  if ( CutLevel != "NONE" ) CanName += "_"+CutLevel ;
  TCanvas* Canvas  = new TCanvas( CanName , CanName , 600 , 600 );

  if (kLogY) gPad->SetLogy(1);

   vector<TH1F*> vDataStack;
   for (int iD=0 ; iD < (signed) vData.size() ; ++iD ) {
     TH1F* iStack = (TH1F*) vData.at(iD)->Clone();
     for (int iD2Sum=iD+1 ; iD2Sum < (signed)  vData.size() ; ++iD2Sum ) {
       iStack->Add(vData.at(iD2Sum));
     }
     iStack->SetMarkerStyle(20);
     iStack->SetMarkerColor(iD+1);  
     vDataStack.push_back( (TH1F*) iStack->Clone() );
     delete iStack;
   }

   vector<TH1F*> vSignalStack;
   for (int iD=0 ; iD < (signed) vSignal.size() ; ++iD ) {
     TH1F* iStack = (TH1F*) vSignal.at(iD)->Clone();
     for (int iD2Sum=iD+1 ; iD2Sum < (signed)  vSignal.size() ; ++iD2Sum ) {
       iStack->Add(vSignal.at(iD2Sum));
     }
     iStack->SetLineColor(iD+1);
     iStack->SetLineWidth(2);
     vSignalStack.push_back( (TH1F*) iStack->Clone() );
     delete iStack;
   }

   vector<TH1F*> vBkgdStack;
   for (int iD=0 ; iD < (signed) vBkgd.size() ; ++iD ) {
     TH1F* iStack = (TH1F*) vBkgd.at(iD)->Clone();
     for (int iD2Sum=iD+1 ; iD2Sum < (signed)  vBkgd.size() ; ++iD2Sum ) {
       iStack->Add(vBkgd.at(iD2Sum));
     }
     iStack->SetLineColor(iD+2);
     iStack->SetFillColor(iD+2);
     vBkgdStack.push_back( (TH1F*) iStack->Clone() );
     delete iStack;
   }

   Double_t hMax = 0. ;
   if ( vBkgdStack  .size() > 0 ) hMax = TMath::Max( vBkgdStack  .at(0)->GetMaximum() , hMax );
   if ( vDataStack  .size() > 0 ) hMax = TMath::Max( vDataStack  .at(0)->GetMaximum() , hMax );
   if ( vSignalStack.size() > 0 ) hMax = TMath::Max( vSignalStack.at(0)->GetMaximum() , hMax );

   if (kLogY) {
     if ( vBkgdStack  .size() > 0 ) vBkgdStack  .at(0)->GetYaxis()->SetRangeUser( 0.01 , 10*hMax);
     if ( vDataStack  .size() > 0 ) vDataStack  .at(0)->GetYaxis()->SetRangeUser( 0.01 , 10*hMax);
     if ( vSignalStack.size() > 0 ) vSignalStack.at(0)->GetYaxis()->SetRangeUser( 0.01 , 10*hMax);
   } else {
     if ( vBkgdStack  .size() > 0 ) vBkgdStack  .at(0)->GetYaxis()->SetRangeUser( 0.   , 1.2*hMax);
     if ( vDataStack  .size() > 0 ) vDataStack  .at(0)->GetYaxis()->SetRangeUser( 0.   , 1.2*hMax);
     if ( vSignalStack.size() > 0 ) vSignalStack.at(0)->GetYaxis()->SetRangeUser( 0.   , 1.2*hMax);
   }

   if        ( vBkgdStack  .size() > 0 ) {
     vBkgdStack.at(0)->DrawCopy("hist");
     for (int iD=1 ; iD < (signed) vBkgdStack.size()    ; ++iD ) vBkgdStack.at(iD)   ->DrawCopy("histsame");  
     for (int iD=0 ; iD < (signed) vSignalStack.size()  ; ++iD ) vSignalStack.at(iD) ->DrawCopy("histsame");  
     for (int iD=0 ; iD < (signed) vDataStack.size()    ; ++iD ) vDataStack.at(iD)   ->DrawCopy("esame");
   } else if ( vSignalStack.size() > 0 ) {
     vSignalStack.at(0)->DrawCopy("hist");
     for (int iD=1 ; iD < (signed) vSignalStack.size()  ; ++iD ) vSignalStack.at(iD) ->DrawCopy("histsame");  
     for (int iD=0 ; iD < (signed) vDataStack.size()    ; ++iD ) vDataStack.at(iD)   ->DrawCopy("esame");
   } else if ( vDataStack  .size() > 0 ) {
     vDataStack.at(0)->DrawCopy("e"); 
     for (int iD=1 ; iD < (signed) vDataStack.size()    ; ++iD ) vDataStack.at(iD)   ->DrawCopy("esame");
   }
    
   int nLegEntry = 2 + (signed) vBkgdStack.size() + (signed) vSignalStack.size() + (signed) vDataStack.size() ;
   TLegend* Legend = new TLegend (.60,.90-nLegEntry*.030,.9,.90);
   Legend->SetBorderSize(0);
   Legend->SetFillColor(0);
   Legend->SetFillStyle(0);
   Legend->SetTextSize(0.04);
   for (int iD=0 ; iD < (signed) vDataStack.size()    ; ++iD ) Legend->AddEntry( vDataStack  .at(iD) , (vLData  .at(iD)).c_str() , "p" ); 
   for (int iD=0 ; iD < (signed) vSignalStack.size()  ; ++iD ) Legend->AddEntry( vSignalStack.at(iD) , (vLSignal.at(iD)).c_str() , "l" );
   for (int iD=0 ; iD < (signed) vBkgdStack.size()    ; ++iD ) Legend->AddEntry( vBkgdStack  .at(iD) , (vLBkgd  .at(iD)).c_str() , "f" );  
   Legend->Draw("same");

   return ;
}


//----------------------------------- PrintYields() -------------------------------------

void UATAnaDisplay::PrintYields( string  DataSet , string  CutGroup , vector<string>  Cuts , 
                                 vector<TH1F*>&  vData  , vector<TH1F*>&  vSignal  , vector<TH1F*>&  vBkgd ,
                                 vector<string>  vLData , vector<string>  vLSignal , vector<string>  vLBkgd ) {

   // Data & Backgrounds

   cout << endl;
   cout << "DataGroup : " << DataSet << " CutGroup : " << CutGroup <<endl;
   cout << "------------+" ;
   for ( vector<string>::iterator itLDL =  vLData.begin() ; itLDL != vLData.end() ; ++itLDL ) cout << "------------+" ;
   for ( vector<string>::iterator itLDL =  vLBkgd.begin() ; itLDL != vLBkgd.end() ; ++itLDL ) cout << "------------+" ;
   cout << "------------+" ;
   cout << endl;  
   printf (" %10s |","Cuts"); 
   for ( vector<string>::iterator itLDL =  vLData.begin() ; itLDL != vLData.end() ; ++itLDL ) printf (" %10s |", itLDL->c_str() );
   for ( vector<string>::iterator itLDL =  vLBkgd.begin() ; itLDL != vLBkgd.end() ; ++itLDL ) printf (" %10s |", itLDL->c_str() );
   cout << "  Bkgd Sum  |" ;
   cout << endl ; 
   cout << "------------+" ;
   for ( vector<string>::iterator itLDL =  vLData.begin() ; itLDL != vLData.end() ; ++itLDL ) cout << "------------+" ;
   for ( vector<string>::iterator itLDL =  vLBkgd.begin() ; itLDL != vLBkgd.end() ; ++itLDL ) cout << "------------+" ;
   cout << "------------+" ;
   cout << endl; 
   int nCuts = 0 ; 
   if      ( vData.size() > 0 ) nCuts = (vData.at(0))->GetNbinsX();
   else if ( vBkgd.size() > 0 ) nCuts = (vBkgd.at(0))->GetNbinsX();
   vector<string>::iterator itCut = Cuts.begin() ;
   for ( int jCut = 1 ; jCut <= nCuts ; ++jCut , ++itCut ) {
     printf (" %10s |", itCut->c_str() );
     Float_t BkgdSum = 0.;
     for ( vector<TH1F*>::iterator itH =  vData.begin() ; itH != vData.end() ; ++itH )   printf (" %10.2f |", (*itH)->GetBinContent(jCut) );  
     for ( vector<TH1F*>::iterator itH =  vBkgd.begin() ; itH != vBkgd.end() ; ++itH ) { printf (" %10.2f |", (*itH)->GetBinContent(jCut) ); BkgdSum += (*itH)->GetBinContent(jCut) ; }
     printf (" %10.2f |", BkgdSum ) ;
     cout << endl ; 
   }
   cout << "------------+" ;
   for ( vector<string>::iterator itLDL =  vLData.begin() ; itLDL != vLData.end() ; ++itLDL ) cout << "------------+" ;
   for ( vector<string>::iterator itLDL =  vLBkgd.begin() ; itLDL != vLBkgd.end() ; ++itLDL ) cout << "------------+" ;
   cout << "------------+" ;
   cout << endl; 
   cout << endl; 

   // Signals

   cout << "------------+" ;
   for ( vector<string>::iterator itLDL =  vLSignal.begin() ; itLDL != vLSignal.end() ; ++itLDL ) cout << "------------+" ;
   cout << endl; 
   printf (" %10s |","Cuts");
   for ( vector<string>::iterator itLDL =  vLSignal.begin() ; itLDL != vLSignal.end() ; ++itLDL ) printf (" %10s |", itLDL->c_str() );
   cout << endl;  
   cout << "------------+" ;
   for ( vector<string>::iterator itLDL =  vLSignal.begin() ; itLDL != vLSignal.end() ; ++itLDL ) cout << "------------+" ;
   cout << endl; 
   nCuts = 0 ;
   if      ( vSignal.size() > 0 ) nCuts = (vSignal.at(0))->GetNbinsX();
   itCut = Cuts.begin() ;
   for ( int jCut = 1 ; jCut <= nCuts ; ++jCut , ++itCut ) {
     printf (" %10s |", itCut->c_str() );
     for ( vector<TH1F*>::iterator itH =  vSignal.begin() ; itH != vSignal.end() ; ++itH )  printf (" %10.2f |", (*itH)->GetBinContent(jCut) );  
     cout << endl ; 
   }
   cout << "------------+" ;
   for ( vector<string>::iterator itLDL =  vLSignal.begin() ; itLDL != vLSignal.end() ; ++itLDL ) cout << "------------+" ;
   cout << endl; 
   cout << endl;

   return ;
}



//----------------------------------- Yields() ------------------------------------------

void UATAnaDisplay::Yields ( UATAnaConfig& Cfg , bool bPlot ) {

  int iContinue = 1 ;
  while ( iContinue != 0 ) {

    int iGroup  = -1 ;
    int iData = -1 ;
   
    cout << endl ;
    cout << "CutFlow histograms " << endl ;
    cout << "  0 = CommonCuts" << endl;
    int iSC = 1 ; 
    for ( vector<ScanCut_t>::iterator itSC = (Cfg.GetScanCuts())->begin() ; itSC != (Cfg.GetScanCuts())->end() ; ++itSC , ++iSC )  
      cout << "  " << iSC << " = " << itSC->ScanName << endl ;
    cout << "--> Please select Cut Group: " ;
    cin  >> iGroup;     
    cout << endl ; 
    cout << "  0 = All InputData" << endl;
    int iDG = 1 ;
    for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG , ++iDG ) 
      cout << "  " << iDG << " = " << itDG->GroupName << endl;
    cout << "--> Please select DataGroup: "  ;
    cin >> iData ; 
  
  /*
    PlotStack( string DataSet , string CutGroup , string CutLevel , bool kLogY ,
               vector<TH1F*>  vData  , vector<TH1F*>  vSignal  , vector<TH1F*>  vBkgd ,
               vector<string> vLData , vector<string> vLSignal , vector<string> vLBkgd ) {
  */
  
    string DataSet          ;
    string CutGroup         ;
    string CutLevel = "NONE";
    vector<string>  Cuts    ; 
    bool   kLogY    = true  ;
    vector<TH1F*>  vData    ;
    vector<TH1F*>  vSignal  ;
    vector<TH1F*>  vBkgd    ; 
    vector<string> vLData   ;
    vector<string> vLSignal ;
    vector<string> vLBkgd   ;
  
    if (iGroup == 0 ) {
      CutGroup = "CommonCuts";  
      for ( vector<TreeFormula_t>::iterator itCC = (Cfg.GetCommonCuts())->begin() ; itCC != (Cfg.GetCommonCuts())->end() ; ++itCC ) Cuts.push_back(itCC->NickName); 

      if (  iData == 0 ) {
        DataSet = "AllDataSets" ;
        int iH = 0 ;
        for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH ) {
          if ( itD->Data  ) { vData.push_back   ( (TH1F*) (CCflow.at(iH))->Clone() ) ; vLData  .push_back(itD->NickName) ; }
          if ( itD->Bkgd  ) { vBkgd.push_back   ( (TH1F*) (CCflow.at(iH))->Clone() ) ; vLBkgd  .push_back(itD->NickName) ; }
          if ( itD->Signal) {
            bool iSAssoc = false ; 
            if (  (Cfg.GetCommonSign())->size() != 0 ) {         
              for ( vector<string>::iterator itSL  = (Cfg.GetCommonSign())->begin() ; itSL != (Cfg.GetCommonSign())->end() ; ++itSL ) {
                 if ( itD->NickName == (*itSL) ) {iSAssoc = true;}
              }
            } else {iSAssoc = true;}
            if (iSAssoc || !bPlot) { vSignal.push_back ( (TH1F*) (CCflow.at(iH))->Clone() ) ; vLSignal.push_back(itD->NickName) ; }
          } 
        }
      } else {
        string GroupName = ((Cfg.GetDataGroups())->at(iData-1)).GroupName ;
        int iH = 0 ;
        DataSet = GroupName ;
        for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
          for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG , ++iH ) {
            if ( itDG->GroupName == GroupName ) {
              if ( itBG->Data  ) { vData.push_back   ( (TH1F*) (CCflowGroup.at(iH))->Clone() ) ; vLData  .push_back(itBG->BaseName) ; }
              if ( itBG->Bkgd  ) { vBkgd.push_back   ( (TH1F*) (CCflowGroup.at(iH))->Clone() ) ; vLBkgd  .push_back(itBG->BaseName) ; }
              if ( itBG->Signal) { 
                bool iSAssoc = false ; 
                if (  (Cfg.GetCommonSign())->size() != 0 ) {         
                   for ( vector<string>::iterator itSL  = (Cfg.GetCommonSign())->begin() ; itSL != (Cfg.GetCommonSign())->end() ; ++itSL ) {
                     for ( vector<string>::iterator itBGM = (itBG->Members).begin() ;  itBGM != (itBG->Members).end() ; ++itBGM ) {
                       if ( (*itBGM) == (*itSL) ) {iSAssoc = true;}
                     }
                   }
                } else {iSAssoc = true;}
                if (iSAssoc || !bPlot) { vSignal.push_back ( (TH1F*) (CCflowGroup.at(iH))->Clone() ) ; vLSignal.push_back(itBG->BaseName) ; }
              }
            }
          }
        }
      }
    } else {  
       kLogY = false ;
       string ScanName = ((Cfg.GetScanCuts())->at(iGroup-1)).ScanName ;
       CutGroup = ScanName ;   
       for ( vector<TreeFormula_t>::iterator itCC  = (((Cfg.GetScanCuts())->at(iGroup-1)).Cuts).begin() ;
                                             itCC != (((Cfg.GetScanCuts())->at(iGroup-1)).Cuts).end()   ; ++itCC ) Cuts.push_back(itCC->NickName); 
       if (  iData == 0 ) {
         DataSet = "AllDataSets" ;
         int iH = 0 ;
         for ( vector<ScanFlow_t>::iterator iSF = SCflow.begin() ; iSF != SCflow.end() ; ++iSF ) {
           if ( iSF->ScanName == ScanName ) {
             for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH) {
               if ( itD->Data  ) { vData.push_back   ( (TH1F*) ((iSF->CutFlow).at(iH))->Clone() ) ; vLData  .push_back(itD->NickName) ; }
               if ( itD->Bkgd  ) { vBkgd.push_back   ( (TH1F*) ((iSF->CutFlow).at(iH))->Clone() ) ; vLBkgd  .push_back(itD->NickName) ; }
               if ( itD->Signal) {
                 bool iSAssoc = false ;
                 if (  (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).size() != 0 ) {
                   for ( vector<string>::iterator itSL  = (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).begin() ;
                                                  itSL != (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).end()   ; ++itSL ) {
                     if ( itD->NickName == (*itSL) ) {iSAssoc = true;}
                   }
                 } else {iSAssoc = true;} 
                 if (iSAssoc || !bPlot) { vSignal.push_back ( (TH1F*) ((iSF->CutFlow).at(iH))->Clone() ) ; vLSignal.push_back(itD->NickName) ; } 
               }
             }
           }
         }
       } else {
         string GroupName = ((Cfg.GetDataGroups())->at(iData-1)).GroupName ;
         int iH = 0 ;
         DataSet  = GroupName ;
         for ( vector<ScanFlow_t>::iterator iSF = SCflowGroup.begin() ; iSF != SCflowGroup.end() ; ++iSF ) {
           if ( iSF->ScanName == ScanName ) {
             for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
               for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG , ++iH ) {
                 if ( itDG->GroupName == GroupName ) {
                   if ( itBG->Data  ) { vData.push_back   ( (TH1F*) ((iSF->CutFlow).at(iH))->Clone() ) ; vLData  .push_back(itBG->BaseName) ; }
                   if ( itBG->Bkgd  ) { vBkgd.push_back   ( (TH1F*) ((iSF->CutFlow).at(iH))->Clone() ) ; vLBkgd  .push_back(itBG->BaseName) ; }
                   if ( itBG->Signal) {
                     bool iSAssoc = false ;
                     if (  (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).size() != 0 ) {
                       for ( vector<string>::iterator itSL  = (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).begin() ;
                                                      itSL != (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).end()   ; ++itSL ) {
                         for ( vector<string>::iterator itBGM = (itBG->Members).begin() ;  itBGM != (itBG->Members).end() ; ++itBGM ) {
                           if ( (*itBGM) == (*itSL) ) {iSAssoc = true;}
                         } 
                       }
                     } else {iSAssoc = true;} 
                     if (iSAssoc || !bPlot) { vSignal.push_back ( (TH1F*) ((iSF->CutFlow).at(iH))->Clone() ) ; vLSignal.push_back(itBG->BaseName) ; }
                   }
                 }
               }
             } 
           }
         }
       } 
    }
  
    if ( bPlot) PlotStack   ( DataSet , CutGroup , CutLevel , kLogY , vData , vSignal  , vBkgd , vLData , vLSignal  , vLBkgd ) ;
    else        PrintYields ( DataSet , CutGroup , Cuts             , vData , vSignal  , vBkgd , vLData , vLSignal  , vLBkgd ) ;

    cout << "Next CutFlow choice ? [0/1] : " ;
    cin >> iContinue ;     
  }
  return;  
}


//----------------------------------- CPlot() ------------------------------------------

void UATAnaDisplay::CPlot ( UATAnaConfig& Cfg ) {


  int iContinue = 1 ;
  while ( iContinue != 0 ) {

    int iGroup  = -1 ;
    int iData   = -1 ;
    int iLevel  = -1 ;
   
    cout << endl ;
    cout << "Control Plots: " << endl ;
    cout << "  0 = CommonCuts" << endl;
    int iSC = 1 ; 
    for ( vector<ScanCut_t>::iterator itSC = (Cfg.GetScanCuts())->begin() ; itSC != (Cfg.GetScanCuts())->end() ; ++itSC , ++iSC )  
      cout << "  " << iSC << " = " << itSC->ScanName << endl ;
    cout << "--> Please select Cut Group: " ;
    cin  >> iGroup;     
    cout << endl ; 
    cout << "  0 = All InputData" << endl;
    int iDG = 1 ;
    for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG , ++iDG ) 
      cout << "  " << iDG << " = " << itDG->GroupName << endl;
    cout << "--> Please select DataGroup: "  ;
    cin >> iData ; 
  
  /*
    PlotStack( string DataSet , string CutGroup , string CutLevel , bool kLogY ,
               vector<TH1F*>  vData  , vector<TH1F*>  vSignal  , vector<TH1F*>  vBkgd ,
               vector<string> vLData , vector<string> vLSignal , vector<string> vLBkgd ) {
  */
  
    string DataSet          ;
    string CutGroup         ;
    string CutLevel = "NONE";
    vector<string>  Cuts    ;
    vector<TH1F*>  vData    ;
    vector<TH1F*>  vSignal  ;
    vector<TH1F*>  vBkgd    ; 
    vector<string> vLData   ;
    vector<string> vLSignal ;
    vector<string> vLBkgd   ;
  
    if (iGroup == 0 ) {
      CutGroup = "CommonCuts";  
      for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
        for ( vector<string>::iterator itCut = (itCP->CCNickName).begin() ; itCut != (itCP->CCNickName).end() ; ++itCut ) {
          bool Found = false ;
          for ( vector<string>::iterator itCuts = Cuts.begin() ; itCuts != Cuts.end() ; ++itCuts ) {
            if ( *itCuts == *itCut ) Found = true ;    
          }
          if ( ! Found ) Cuts.push_back(*itCut) ; 
        }
      }  
      int iCut = 0 ;
      for ( vector<string>::iterator itCuts = Cuts.begin() ; itCuts != Cuts.end() ; ++itCuts , ++iCut ) cout << "  " << iCut << " = " << *itCuts << endl ;
      cout << "--> Please select Cut Level: " ;
      cin  >> iLevel;     
      cout << endl ; 
      CutLevel = Cuts.at(iLevel); 
    
      if (  iData == 0 ) {
        DataSet = "AllDataSets" ;

        int iH = 0 ;
        for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
          // clean
          for ( vector<TH1F*>::iterator itH = vData  .begin() ; itH != vData  .end() ; ++itH ) delete (*itH) ; vData  .clear() ; vLData  .clear() ; 
          for ( vector<TH1F*>::iterator itH = vSignal.begin() ; itH != vSignal.end() ; ++itH ) delete (*itH) ; vSignal.clear() ; vLSignal.clear() ;
          for ( vector<TH1F*>::iterator itH = vBkgd  .begin() ; itH != vBkgd  .end() ; ++itH ) delete (*itH) ; vBkgd  .clear() ; vLBkgd  .clear() ; 
          // Fill next histogram set
          for ( vector<string>::iterator itCC = (itCP->CCNickName).begin() ; itCC != (itCP->CCNickName).end() ; ++itCC ) {
            for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH ) {
              if (  *itCC ==  CutLevel  ) {
                 if ( itD->Data  ) { vData.push_back   ( (TH1F*) (PlotCC.at(iH))->Clone() ) ; vLData  .push_back(itD->NickName) ; }
                 if ( itD->Bkgd  ) { vBkgd.push_back   ( (TH1F*) (PlotCC.at(iH))->Clone() ) ; vLBkgd  .push_back(itD->NickName) ; }
                 if ( itD->Signal) {
                   bool iSAssoc = false ; 
                   if (  (Cfg.GetCommonSign())->size() != 0 ) {         
                     for ( vector<string>::iterator itSL  = (Cfg.GetCommonSign())->begin() ; itSL != (Cfg.GetCommonSign())->end() ; ++itSL ) {
                       if ( itD->NickName == (*itSL) ) {iSAssoc = true;}
                     }
                   } else {iSAssoc = true;}
                   if (iSAssoc) { vSignal.push_back ( (TH1F*) (PlotCC.at(iH))->Clone() ) ; vLSignal.push_back(itD->NickName) ; }
                 }
              }
            }  
          }  
          PlotStack   ( itCP->NickName+"_"+DataSet , CutGroup , CutLevel , itCP->kLogY , vData , vSignal  , vBkgd , vLData , vLSignal  , vLBkgd ) ;
        }

      } else {
        string GroupName = ((Cfg.GetDataGroups())->at(iData-1)).GroupName ;
        DataSet = GroupName ;

        int iH = 0 ;
        for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
          // clean
          for ( vector<TH1F*>::iterator itH = vData  .begin() ; itH != vData  .end() ; ++itH ) delete (*itH) ; vData  .clear() ; vLData  .clear() ; 
          for ( vector<TH1F*>::iterator itH = vSignal.begin() ; itH != vSignal.end() ; ++itH ) delete (*itH) ; vSignal.clear() ; vLSignal.clear() ;
          for ( vector<TH1F*>::iterator itH = vBkgd  .begin() ; itH != vBkgd  .end() ; ++itH ) delete (*itH) ; vBkgd  .clear() ; vLBkgd  .clear() ; 
          // Fill next histogram set
          for ( vector<string>::iterator itCC = (itCP->CCNickName).begin() ; itCC != (itCP->CCNickName).end() ; ++itCC ) {
            for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
              for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG , ++iH ) {
                if ( ( *itCC ==  CutLevel ) && ( itDG->GroupName == GroupName ) ) {
                  if ( itBG->Data  ) { vData.push_back   ( (TH1F*) (PlotCCGroup.at(iH))->Clone() ) ; vLData  .push_back(itBG->BaseName) ; }
                  if ( itBG->Bkgd  ) { vBkgd.push_back   ( (TH1F*) (PlotCCGroup.at(iH))->Clone() ) ; vLBkgd  .push_back(itBG->BaseName) ; }
                  if ( itBG->Signal) { 
                    bool iSAssoc = false ;
                    if (  (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).size() != 0 ) {
                      for ( vector<string>::iterator itSL  = (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).begin() ;
                                                    itSL != (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).end()   ; ++itSL ) {
                         for ( vector<string>::iterator itBGM = (itBG->Members).begin() ;  itBGM != (itBG->Members).end() ; ++itBGM ) {
                           if ( (*itBGM) == (*itSL) ) {iSAssoc = true;}
                         } 
                      }
                    } else {iSAssoc = true;} 
                    if (iSAssoc) { vSignal.push_back ( (TH1F*) (PlotCCGroup.at(iH))->Clone() ) ; vLSignal.push_back(itBG->BaseName) ; }
                  }
                }
              }
            }
          }
          PlotStack   ( itCP->NickName+"_"+DataSet , CutGroup , CutLevel , itCP->kLogY , vData , vSignal  , vBkgd , vLData , vLSignal  , vLBkgd ) ;
        }
   
 
      } 

    } else { 
      string ScanName = ((Cfg.GetScanCuts())->at(iGroup-1)).ScanName ;
      CutGroup = ScanName ;   

      for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
        for ( vector<string>::iterator itCut = (itCP->SCNickName).begin() ; itCut != (itCP->SCNickName).end() ; ++itCut ) {
          bool Found = false ;
          for ( vector<string>::iterator itCuts = Cuts.begin() ; itCuts != Cuts.end() ; ++itCuts ) {
            if ( *itCuts == *itCut ) Found = true ;    
          }
          if ( ! Found ) Cuts.push_back(*itCut) ; 
        }
      }  
      int iCut = 0 ;
      for ( vector<string>::iterator itCuts = Cuts.begin() ; itCuts != Cuts.end() ; ++itCuts , ++iCut ) cout << "  " << iCut << " = " << *itCuts << endl ;
      cout << "--> Please select Cut Level: " ;
      cin  >> iLevel;     
      cout << endl ; 
      CutLevel = Cuts.at(iLevel); 


      if (  iData == 0 ) {
        DataSet = "AllDataSets" ;

        int iH = 0 ;
        for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
          // clean
          for ( vector<TH1F*>::iterator itH = vData  .begin() ; itH != vData  .end() ; ++itH ) delete (*itH) ; vData  .clear() ; vLData  .clear() ; 
          for ( vector<TH1F*>::iterator itH = vSignal.begin() ; itH != vSignal.end() ; ++itH ) delete (*itH) ; vSignal.clear() ; vLSignal.clear() ;
          for ( vector<TH1F*>::iterator itH = vBkgd  .begin() ; itH != vBkgd  .end() ; ++itH ) delete (*itH) ; vBkgd  .clear() ; vLBkgd  .clear() ; 
          // Fill next histogram set
          for ( vector<string>::iterator itCC = (itCP->SCNickName).begin() ; itCC != (itCP->SCNickName).end() ; ++itCC ) {
           for ( vector<ScanCut_t>::iterator itSC = (Cfg.GetScanCuts())->begin() ; itSC != (Cfg.GetScanCuts())->end() ; ++itSC , ++iSC ) { 
            for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH ) {
              if ( ( *itCC ==  CutLevel ) && ( itSC->ScanName == ScanName ) ) {
                 if ( itD->Data  ) { vData.push_back   ( (TH1F*) (PlotSC.at(iH))->Clone() ) ; vLData  .push_back(itD->NickName) ; }
                 if ( itD->Bkgd  ) { vBkgd.push_back   ( (TH1F*) (PlotSC.at(iH))->Clone() ) ; vLBkgd  .push_back(itD->NickName) ; }
                 if ( itD->Signal) {
                   bool iSAssoc = false ; 
                   if (  (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).size() != 0 ) {         
                     for ( vector<string>::iterator itSL  = (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).begin() ;
                                                    itSL != (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).end()   ; ++itSL ) {
                       if ( itD->NickName == (*itSL) ) {iSAssoc = true;}
                     }
                   } else {iSAssoc = true;}
                   if (iSAssoc) { vSignal.push_back ( (TH1F*) (PlotSC.at(iH))->Clone() ) ; vLSignal.push_back(itD->NickName) ; }
                 }
              }
            }
           }  
          }  
          PlotStack   ( itCP->NickName+"_"+DataSet , CutGroup , CutLevel , itCP->kLogY , vData , vSignal  , vBkgd , vLData , vLSignal  , vLBkgd ) ;
        }

      } else {
        string GroupName = ((Cfg.GetDataGroups())->at(iData-1)).GroupName ;
        DataSet = GroupName ;

        int iH = 0 ;
        for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
          // clean
          for ( vector<TH1F*>::iterator itH = vData  .begin() ; itH != vData  .end() ; ++itH ) delete (*itH) ; vData  .clear() ; vLData  .clear() ; 
          for ( vector<TH1F*>::iterator itH = vSignal.begin() ; itH != vSignal.end() ; ++itH ) delete (*itH) ; vSignal.clear() ; vLSignal.clear() ;
          for ( vector<TH1F*>::iterator itH = vBkgd  .begin() ; itH != vBkgd  .end() ; ++itH ) delete (*itH) ; vBkgd  .clear() ; vLBkgd  .clear() ; 
          // Fill next histogram set
          for ( vector<string>::iterator itCC = (itCP->SCNickName).begin() ; itCC != (itCP->SCNickName).end() ; ++itCC ) {
           for ( vector<ScanCut_t>::iterator itSC = (Cfg.GetScanCuts())->begin() ; itSC != (Cfg.GetScanCuts())->end() ; ++itSC , ++iSC ) { 
            for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
              for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG , ++iH ) {
                if ( ( *itCC ==  CutLevel ) && ( itSC->ScanName == ScanName ) && ( itDG->GroupName == GroupName ) ) {
                  if ( itBG->Data  ) { vData.push_back   ( (TH1F*) (PlotSCGroup.at(iH))->Clone() ) ; vLData  .push_back(itBG->BaseName) ; }
                  if ( itBG->Bkgd  ) { vBkgd.push_back   ( (TH1F*) (PlotSCGroup.at(iH))->Clone() ) ; vLBkgd  .push_back(itBG->BaseName) ; }
                  if ( itBG->Signal) {
                    bool iSAssoc = false ; 
                    if (  (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).size() != 0 ) {
                       for ( vector<string>::iterator itSL  = (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).begin() ;
                                                      itSL != (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).end()   ; ++itSL ) {
                         for ( vector<string>::iterator itBGM = (itBG->Members).begin() ;  itBGM != (itBG->Members).end() ; ++itBGM ) {
                           if ( (*itBGM) == (*itSL) ) {iSAssoc = true;}
                         } 
                       }
                    } else {iSAssoc = true;} 
                    if (iSAssoc) { vSignal.push_back ( (TH1F*) (PlotSCGroup.at(iH))->Clone() ) ; vLSignal.push_back(itBG->BaseName) ; }
                  }
                }
              }
            }
           }
          }
          PlotStack   ( itCP->NickName+"_"+DataSet , CutGroup , CutLevel , itCP->kLogY , vData , vSignal  , vBkgd , vLData , vLSignal  , vLBkgd ) ;
        }
      } 


    }

    cout << "Next choice ? [0/1] : " ;
    cin >> iContinue ;     
  }

  return;
}

