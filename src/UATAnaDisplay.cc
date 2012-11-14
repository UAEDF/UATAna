
#include "../includes/UATAnaDisplay.h"
#include <TROOT.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TLegend.h>
#include <TFrame.h>
#include <TText.h>
#include <TLatex.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <TMath.h>

#include "src/tdrstyle.C"
#include "src/LatinoStyle2.C"

//----------------------------------- Tools ------------------------------------------

void getsyst(TString cname, float mass, float &N, float &s, float &u) {

  ifstream card; card.open(cname.Data());
  if(!card) {
    printf("Did not find card %s\n", cname.Data());
    return;
  }
//  cout << " Reading: " << cname.Data() << endl ;

  while ( !card.eof() ) {

    //cout << "." << endl;
    float HiggsMass(0);
    float NumEventsInCtrlRegion(0);
    float scaleToSignRegion(0);
    float uncertaintyOnScaleToSignRegion(0);
    card >> HiggsMass >> NumEventsInCtrlRegion >> scaleToSignRegion >> uncertaintyOnScaleToSignRegion;
    //cout << HiggsMass << NumEventsInCtrlRegion << scaleToSignRegion << uncertaintyOnScaleToSignRegion << endl;
    // notes from Emanuele
    // n in signal region = NumEventsInCtrlRegion * scaleToSignRegion
    // uncertainty on n   = NumEventsInCtrlRegion * uncertaintyOnScaleToSignRegion    
    // fractional uncertainty on n = uncertaintyOnScaleToSignRegion / scaleToSignRegion

//    printf("looking for mass point %.1f %.1f\n", mass, HiggsMass);
    if (fabs(HiggsMass-mass)<5) {
      s = scaleToSignRegion;
      N = NumEventsInCtrlRegion;
      u = uncertaintyOnScaleToSignRegion;
//      printf("Found mass point %.0f: %.3f %.3f %.3f\n", mass, N, s, u);
      return;
    }
  }
  printf("Did not find mass point %.0f\n", mass);
  N = -1.;
  s = -1.;
  u = -1.;
  return;

}

//----------------------------------- DrawOverflow() ----------------------------------- 


TH1F * DrawOverflow(TH1F *h)
{
   TH1F *htmp = (TH1F*) h->Clone();
   UInt_t nx     = h->GetNbinsX();
   double cont   = h->GetBinContent(nx) + h->GetBinContent(nx+1) ;
   double err    = sqrt( h->GetBinError(nx)*h->GetBinError(nx) + h->GetBinError(nx+1)*h->GetBinError(nx+1) ) ;
   htmp->SetBinContent(nx,cont);
   htmp->SetBinError(nx,err);
   htmp->SetBinContent(nx+1,0.);
   htmp->SetBinError(nx+1,0.);
   return htmp;

/*
   // This function paint the histogram h with an extra bin for overflows
   UInt_t nx    = h->GetNbinsX()+1;
   Double_t *xbins= new Double_t[nx+1];
   for (UInt_t i=0;i<nx;i++)
     xbins[i]=h->GetBinLowEdge(i+1);
   xbins[nx]=xbins[nx-1]+h->GetBinWidth(nx);
   char *tempName= new char[strlen(h->GetName())+10];
   sprintf(tempName,"%swtOverFlow",h->GetName());
   // Book a temporary histogram having ab extra bin for overflows
   TH1F *htmp = new TH1F(tempName, h->GetTitle(), nx, xbins);
   // Reset the axis labels
   htmp->SetXTitle(h->GetXaxis()->GetTitle());
   htmp->SetYTitle(h->GetYaxis()->GetTitle());
   // Fill the new hitogram including the extra bin for overflows
   for (UInt_t i=1; i<=nx; i++)
     htmp->Fill(htmp->GetBinCenter(i), h->GetBinContent(i));
   // Fill the underflows
   htmp->Fill(h->GetBinLowEdge(1)-1, h->GetBinContent(0));
   // Restore the number of entries
   htmp->SetEntries(h->GetEntries());
   // FillStyle and color
   htmp->SetFillStyle(h->GetFillStyle());
   htmp->SetFillColor(h->GetFillColor());
   return htmp;
*/
}

//------------------------------------------------------------------------------
// Pad2TAxis
//------------------------------------------------------------------------------
void Pad2TAxis(TH1* hist, TString xtitle, TString ytitle)
{
  TAxis* xaxis = (TAxis*)hist->GetXaxis();
  TAxis* yaxis = (TAxis*)hist->GetYaxis();

  xaxis->SetLabelFont  (    42);
  xaxis->SetLabelOffset( 0.025);
  xaxis->SetLabelSize  (   0.1);
  xaxis->SetNdivisions (   505);
  xaxis->SetTitle      (xtitle);
  xaxis->SetTitleFont  (    42);
  xaxis->SetTitleOffset(  1.35);
  xaxis->SetTitleSize  (  0.11);
  
  yaxis->CenterTitle   (      );
  yaxis->SetLabelFont  (    42);
  yaxis->SetLabelOffset(  0.02);
  yaxis->SetLabelSize  (   0.1);
  yaxis->SetNdivisions (   505);
  yaxis->SetTitle      (ytitle);
  yaxis->SetTitleFont  (    42);
  yaxis->SetTitleOffset(  .6);
  yaxis->SetTitleSize  (  0.11);
}


//------------------------------------------------------------------------------
// GetMaximumIncludingErrors
//------------------------------------------------------------------------------
Double_t GetMaximumIncludingErrors(TH1F* h)
{
  Float_t maxWithErrors = 0;

  for (Int_t i=1; i<=h->GetNbinsX(); i++) {

    Float_t binHeight = h->GetBinContent(i) + h->GetBinError(i);

    if (binHeight > maxWithErrors) maxWithErrors = binHeight;
  }

  return maxWithErrors;
}


//----------------------------------- Init() ------------------------------------------

void UATAnaDisplay::Init ( UATAnaConfig& Cfg ) {

  //setTDRStyle();
  LatinoStyle2();
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0); 
  gStyle->SetHatchesLineWidth(2);

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

  bool DrawOvFlBin = true; 
  if ( DrawOvFlBin ) {
    for (vector<TH1F*>::iterator it = PlotCC_.begin() ; it != PlotCC_.end() ; ++it )  PlotCC.push_back ( DrawOverflow(*it) ) ; 
    for (vector<TH1F*>::iterator it = PlotSC_.begin() ; it != PlotSC_.end() ; ++it )  PlotSC.push_back ( DrawOverflow(*it) ) ; 
    for (vector<TH1F*>::iterator it = PlotCCGroup_.begin() ; it != PlotCCGroup_.end() ; ++it )  PlotCCGroup.push_back ( DrawOverflow(*it) ) ; 
    for (vector<TH1F*>::iterator it = PlotSCGroup_.begin() ; it != PlotSCGroup_.end() ; ++it )  PlotSCGroup.push_back ( DrawOverflow(*it) ) ; 
  } else {
    for (vector<TH1F*>::iterator it = PlotCC_.begin() ; it != PlotCC_.end() ; ++it )  PlotCC.push_back ( (TH1F*) (*it)->Clone() ) ; 
    for (vector<TH1F*>::iterator it = PlotSC_.begin() ; it != PlotSC_.end() ; ++it )  PlotSC.push_back ( (TH1F*) (*it)->Clone() ) ; 
    for (vector<TH1F*>::iterator it = PlotCCGroup_.begin() ; it != PlotCCGroup_.end() ; ++it )  PlotCCGroup.push_back ( (TH1F*) (*it)->Clone() ) ; 
    for (vector<TH1F*>::iterator it = PlotSCGroup_.begin() ; it != PlotSCGroup_.end() ; ++it )  PlotSCGroup.push_back ( (TH1F*) (*it)->Clone() ) ; 
  }


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


/*
  for (vector<TH1F*>::iterator it = PlotCC_.begin() ; it != PlotCC_.end() ; ++it ) delete (*it) ;
  for (vector<TH1F*>::iterator it = PlotSC_.begin() ; it != PlotSC_.end() ; ++it ) delete (*it) ;
  for (vector<TH1F*>::iterator it = PlotCCGroup_.begin() ; it != PlotCCGroup_.end() ; ++it ) delete (*it) ;
  for (vector<TH1F*>::iterator it = PlotSCGroup_.begin() ; it != PlotSCGroup_.end() ; ++it ) delete (*it) ;
*/
  PlotCC_.clear();
  PlotSC_.clear();
  PlotCCGroup_.clear();
  PlotSCGroup_.clear();

  // Close File
  File->Close(); 

  return;
}   

//----------------------------------- PlotStack() -------------------------------------

void UATAnaDisplay::PlotStack( string  DataSet , string  CutGroup , string  CutLevel , int kLogYIN ,
                               vector<TH1F*>&  vData  , vector<TH1F*>&  vSignal  , vector<TH1F*>&  vBkgd  ,
                               vector<string>  vLData , vector<string>  vLSignal , vector<string>  vLBkgd ,
                               vector<int>     vCData , vector<int>     vCSignal , vector<int>     vCBkgd ,
                               string  AxisT          , vector<string> CPExtraText , string Title           ,
                               float           fLumi  , bool SaveFig    ,  bool DrawBgError , bool DrawRatio , 
                               vector<string> CutLines  
//                             string  XAxisT = "Var" , string  YAxisT = "entries / bin", string Title = "Title"      
                             ) {


// bool DrawRatio = true ;   
// bool DrawRatio = false ;   


 TH1F* hErr ;

 // ---- Lin/Log

 int kLokMin = 0 ;
 int kLokMax = 0 ;
 if ( kLogYIN == 0 ) {
   kLokMin = 0 ;
   kLokMax = 0 ;  
 }
 if ( kLogYIN == 1 ) {
   kLokMin = 1 ;
   kLokMax = 1 ;
 }
 if ( kLogYIN == 2 ) {
   kLokMin = 0 ;
   kLokMax = 1 ;
 }

 for ( int ikLogY = kLokMin ; ikLogY <= kLokMax ; ++ikLogY ) { 

  bool kLogY = ikLogY ;

  TString  CanName = DataSet+"_"+CutGroup ;
  if ( CutLevel != "NONE" ) CanName += "_"+CutLevel ;
  if ( ! kLogY ) CanName += "_Lin" ; 
  else           CanName += "_Log" ;   

  TCanvas* Canvas ;
  TPad* pad1;
  TPad* pad2;
  if ( DrawRatio ) {

    Canvas = new TCanvas( CanName , CanName , 600 , 1.2*600 );
    //Addint the top bottom margins causes problems. pad2 has also a different ymax value then the ymin of pad1. this is necessary. If you remove the -0.03 in pad2, the letter "N" from prediction will disappear.

/*
// WORKING
    pad1 = new TPad("pad1", "pad1", 0, 0.20, 1, 1);
    //pad1->SetTopMargin   (0.05);
    //pad1->SetBottomMargin(0.02);
    //pad1->SetRightMargin(0.05);
    //pad1->SetLeftMargin(0.15);
    pad1->Draw();
    
    pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.25); 
    pad2->SetTopMargin   (-0.03);
    pad2->SetBottomMargin(0.35);
    //pad1->SetRightMargin(0.05);
    //pad1->SetLeftMargin(0.15);
    pad2->Draw();
*/
    pad1 = new TPad("pad1", "pad1", 0, 0.25, 1, 1);
    pad1->SetTopMargin   (0.05);
    pad1->SetBottomMargin(0.08);
    pad1->SetRightMargin(0.05);
    pad1->SetLeftMargin(0.15);
    pad1->Draw();
    

    pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.3); 
    pad2->SetTopMargin   (-0.08);
    pad2->SetBottomMargin(0.35);
    pad2->SetRightMargin(0.05);
    pad2->SetLeftMargin(0.15);
    pad2->Draw();

  } else {
    Canvas = new TCanvas( CanName , CanName , 600 , 600 );
    pad1 = (TPad*)Canvas->GetPad(0);
    pad1->SetRightMargin(0.05);
    pad1->SetLeftMargin(0.15);
  }


  // ---- pad1: Stack Plot ----


  pad1->cd();
  if (kLogY) pad1->SetLogy(1);
  
   vector<TH1F*> vDataStack;
   for (int iD=0 ; iD < (signed) vData.size() ; ++iD ) {
     TH1F* iStack = (TH1F*) vData.at(iD)->Clone();
     for (int iD2Sum=iD+1 ; iD2Sum < (signed)  vData.size() ; ++iD2Sum ) {
       iStack->Add(vData.at(iD2Sum));
     }
     iStack->SetMarkerStyle(20);
     if ( vCData.size() > 0) {
       iStack->SetMarkerColor(vCData.at(iD));
       iStack->SetLineWidth(2);
       iStack->SetLineColor(vCData.at(iD));
     } else {
       iStack->SetMarkerColor(iD+1);
       iStack->SetLineColor(iD+1);
       iStack->SetLineWidth(2);
     }  
     vDataStack.push_back( (TH1F*) iStack->Clone() );
     delete iStack;
   }

   vector<TH1F*> vSignalStack;
   for (int iD=0 ; iD < (signed) vSignal.size() ; ++iD ) {
     TH1F* iStack = (TH1F*) vSignal.at(iD)->Clone();
     for (int iD2Sum=iD+1 ; iD2Sum < (signed)  vSignal.size() ; ++iD2Sum ) {
       iStack->Add(vSignal.at(iD2Sum));
     }
     if ( vCSignal.size() > 0) {
       cout << "Changing Color" ;
       //iStack->SetMarkerColor(vCSignal.at(iD));
       iStack->SetMarkerColor(kRed+1);
     } else {
       //iStack->SetLineColor(iD+1);
       iStack->SetLineColor(kRed+1);
     }
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
     if ( vCBkgd.size() > 0) {
       iStack->SetLineColor(vCBkgd.at(iD));
       iStack->SetFillColor(vCBkgd.at(iD));
     } else {
       iStack->SetLineColor(iD+2);
       iStack->SetFillColor(iD+2);
     }
     vBkgdStack.push_back( (TH1F*) iStack->Clone() );
     delete iStack;
   }

   Double_t hMax = 0. ;
   if ( vBkgdStack  .size() > 0 ) hMax = TMath::Max( GetMaximumIncludingErrors(vBkgdStack  .at(0)) , hMax );
   if ( vDataStack  .size() > 0 ) hMax = TMath::Max( GetMaximumIncludingErrors(vDataStack  .at(0)) , hMax );
   if ( vSignalStack.size() > 0 ) hMax = TMath::Max( GetMaximumIncludingErrors(vSignalStack.at(0)) , hMax );


   if (kLogY) {
     if ( vBkgdStack  .size() > 0 ) vBkgdStack  .at(0)->GetYaxis()->SetRangeUser( 0.05 , 500*hMax);
     if ( vDataStack  .size() > 0 ) vDataStack  .at(0)->GetYaxis()->SetRangeUser( 0.05 , 500*hMax);
     if ( vSignalStack.size() > 0 ) vSignalStack.at(0)->GetYaxis()->SetRangeUser( 0.05 , 500*hMax);
   } else {
     if ( vBkgdStack  .size() > 0 ) vBkgdStack  .at(0)->GetYaxis()->SetRangeUser( 0.   , 1.55*hMax);
     if ( vDataStack  .size() > 0 ) vDataStack  .at(0)->GetYaxis()->SetRangeUser( 0.   , 1.55*hMax);
     if ( vSignalStack.size() > 0 ) vSignalStack.at(0)->GetYaxis()->SetRangeUser( 0.   , 1.55*hMax);
   }

   vector<string> vAxisT = UATokenize( AxisT , ';' );
   TString XAxisT ;
   if ( (signed) vAxisT.size() == 0 ) XAxisT += AxisT ;
   if ( (signed) vAxisT.size()  > 0 ) XAxisT += vAxisT.at(0);   
   if ( (signed) vAxisT.size()  > 2 ) XAxisT += " ["+vAxisT.at(2)+"]";   
   TString YAxisT ;
   float BinWidth ;
   if      ( vBkgdStack  .size() > 0 ) BinWidth = vBkgdStack  .at(0)->GetBinWidth(0);
   else if ( vSignalStack.size() > 0 ) BinWidth = vSignalStack.at(0)->GetBinWidth(0);
   else if ( vDataStack  .size() > 0 ) BinWidth = vDataStack  .at(0)->GetBinWidth(0);
   int precision = 0;
   Double_t temp = BinWidth;
   while( ! ( floor(temp)==temp)) {
    temp = (temp-floor(temp))*10; //substract floor and move digits
    while (temp>=1.) temp=temp-1;  //substract till first digit is 0
    precision++; 
    if(precision==3||temp < 0.000001) break; // fix computer errors
  }

   TString ytitle = Form("events / %s.%df", "%", precision);
   if ( (signed) vAxisT.size() <= 1 ) { 
     if ( BinWidth == 1 ) YAxisT += "events"; 
     else                 YAxisT += Form(ytitle.Data(), BinWidth );
   } else {
     if ( vAxisT.at(1) == "AUTO" ) {
       if   ( BinWidth == 1 ) {
         YAxisT += "events"; 
         if ( (signed) vAxisT.size()  > 2 ) YAxisT += " / "+vAxisT.at(2);  
       }
       else {
         YAxisT += Form(ytitle.Data(), BinWidth );
         if ( (signed) vAxisT.size()  > 2 ) YAxisT += " "+vAxisT.at(2);  
       }
     } else YAxisT += vAxisT.at(1);
   }
   


   if ( vBkgdStack  .size() > 0 ) vBkgdStack  .at(0)->GetXaxis()->SetTitle(XAxisT);
   if ( vDataStack  .size() > 0 ) vDataStack  .at(0)->GetXaxis()->SetTitle(XAxisT);
   if ( vSignalStack.size() > 0 ) vSignalStack.at(0)->GetXaxis()->SetTitle(XAxisT);

   if ( vBkgdStack  .size() > 0 ) vBkgdStack  .at(0)->GetYaxis()->SetTitle(YAxisT);
   if ( vDataStack  .size() > 0 ) vDataStack  .at(0)->GetYaxis()->SetTitle(YAxisT);
   if ( vSignalStack.size() > 0 ) vSignalStack.at(0)->GetYaxis()->SetTitle(YAxisT);

   if ( vBkgdStack  .size() > 0 ) vBkgdStack  .at(0)->GetXaxis()->SetTitleOffset(1.1);
   if ( vDataStack  .size() > 0 ) vDataStack  .at(0)->GetXaxis()->SetTitleOffset(1.1);
   if ( vSignalStack.size() > 0 ) vSignalStack.at(0)->GetXaxis()->SetTitleOffset(1.1);
   if ( vBkgdStack  .size() > 0 ) vBkgdStack  .at(0)->GetYaxis()->SetTitleOffset(1.5);
   if ( vDataStack  .size() > 0 ) vDataStack  .at(0)->GetYaxis()->SetTitleOffset(1.5);
   if ( vSignalStack.size() > 0 ) vSignalStack.at(0)->GetYaxis()->SetTitleOffset(1.5);

   if ( vBkgdStack  .size() > 0 ) vBkgdStack  .at(0)->GetXaxis()->SetTitleSize(0.05);
   if ( vDataStack  .size() > 0 ) vDataStack  .at(0)->GetXaxis()->SetTitleSize(0.05);
   if ( vSignalStack.size() > 0 ) vSignalStack.at(0)->GetXaxis()->SetTitleSize(0.05);
   if ( vBkgdStack  .size() > 0 ) vBkgdStack  .at(0)->GetYaxis()->SetTitleSize(0.05);
   if ( vDataStack  .size() > 0 ) vDataStack  .at(0)->GetYaxis()->SetTitleSize(0.05);
   if ( vSignalStack.size() > 0 ) vSignalStack.at(0)->GetYaxis()->SetTitleSize(0.05);

   if ( vBkgdStack  .size() > 0 ) vBkgdStack  .at(0)->GetXaxis()->SetLabelSize(0.04);
   if ( vDataStack  .size() > 0 ) vDataStack  .at(0)->GetXaxis()->SetLabelSize(0.04);
   if ( vSignalStack.size() > 0 ) vSignalStack.at(0)->GetXaxis()->SetLabelSize(0.04);
   if ( vBkgdStack  .size() > 0 ) vBkgdStack  .at(0)->GetYaxis()->SetLabelSize(0.04);
   if ( vDataStack  .size() > 0 ) vDataStack  .at(0)->GetYaxis()->SetLabelSize(0.04);
   if ( vSignalStack.size() > 0 ) vSignalStack.at(0)->GetYaxis()->SetLabelSize(0.04);

   if ( vBkgdStack  .size() > 0 ) vBkgdStack  .at(0)->GetXaxis()->SetLabelOffset(0.01);
   if ( vDataStack  .size() > 0 ) vDataStack  .at(0)->GetXaxis()->SetLabelOffset(0.01);
   if ( vSignalStack.size() > 0 ) vSignalStack.at(0)->GetXaxis()->SetLabelOffset(0.01);
   if ( vBkgdStack  .size() > 0 ) vBkgdStack  .at(0)->GetYaxis()->SetLabelOffset(0.01);
   if ( vDataStack  .size() > 0 ) vDataStack  .at(0)->GetYaxis()->SetLabelOffset(0.01);
   if ( vSignalStack.size() > 0 ) vSignalStack.at(0)->GetYaxis()->SetLabelOffset(0.01);


   if        ( vBkgdStack  .size() > 0 ) {
     vBkgdStack.at(0)->DrawCopy("hist");
     for (int iD=1 ; iD < (signed) vBkgdStack.size()    ; ++iD ) vBkgdStack.at(iD)   ->DrawCopy("histsame");  
     if (DrawBgError) {
       hErr =  (TH1F*) vBkgdStack.at(0)->Clone() ;
       hErr->SetFillColor(13);
       hErr->SetLineColor( 0);
       hErr->SetFillStyle(3345);
       hErr->SetMarkerSize(0);
       hErr->SetMarkerColor(kBlack);
       hErr->DrawCopy("e2same");
     }
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
    
   //int nLegEntry = 2 + (signed) vBkgdStack.size() + (signed) vSignalStack.size() + (signed) vDataStack.size() ;
   //TLegend* Legend = new TLegend (.60,.90-nLegEntry*.030,.9,.90);
   int nLegEntry = 2 + ( (signed) vBkgdStack.size() + (signed) vSignalStack.size() + (signed) vDataStack.size() ) /2 ;
   TLegend* Legend = new TLegend (.18,.88-nLegEntry*.035,.58,.88);
   Legend->SetNColumns(2);
   Legend->SetBorderSize(0);
   Legend->SetFillColor(0);
   Legend->SetFillStyle(0);
   Legend->SetTextFont(42); 
   Legend->SetTextAlign(12);
   Legend->SetTextSize(0.035);

   // fix the name of Z+jets
   for (int iD=0 ; iD < (signed) vBkgdStack.size()    ; ++iD ) {
     if (  vLBkgd  .at(iD) == "Z+jets" ) vLBkgd  .at(iD) = "Z/#gamma*";
   }

   for (int iD=0 ; iD < (signed) vDataStack.size()    ; ++iD ) Legend->AddEntry( vDataStack  .at(iD) , TString(" ")+(vLData  .at(iD)).c_str() , "lp" ); 
   for (int iD=0 ; iD < (signed) vSignalStack.size()  ; ++iD ) Legend->AddEntry( vSignalStack.at(iD) , TString(" ")+(vLSignal.at(iD)).c_str() , "l" );
   for (int iD=0 ; iD < (signed) vBkgdStack.size()    ; ++iD ) Legend->AddEntry( vBkgdStack  .at(iD) , TString(" ")+(vLBkgd  .at(iD)).c_str() , "f" );  
   if (DrawBgError) Legend->AddEntry( hErr , TString(" stat.#oplussyst.") , "f");
   Legend->Draw("same");

   //TLatex* TLTitle = new TLatex(.15,.94,Title.c_str());
   TLatex* TLTitle = new TLatex(.92,.86,"CMS Preliminary");
   TLTitle ->SetTextSize(.04);
   TLTitle ->SetTextAlign(32);
   TLTitle ->SetNDC(1);
   TLTitle ->SetTextFont(42); 
   TLTitle ->Draw("same");

   char LumiText[50];
   sprintf ( LumiText , "#sqrt{s} = 8 TeV, L = %4.1f fb^{-1}", fLumi/1000. );
   //TLatex* Lumi = new TLatex(.75,.94,"L = 2.4 fb^{-1}");
   TLatex* Lumi = new TLatex(.92,.81,LumiText);
   Lumi ->SetTextSize(.03);
   Lumi->SetTextAlign(32);
   Lumi ->SetNDC(1);
   Lumi ->SetTextFont(42);   
   Lumi ->Draw("same");

   for ( int iCPET =  0 ; iCPET < (signed) CPExtraText.size() ; ++iCPET ) {
     TLatex* TLET = new TLatex(.92,.76-iCPET*0.04,  (CPExtraText.at(iCPET)).c_str() );
     TLET->SetTextSize(.03); 
     TLET->SetTextAlign(32);
     TLET->SetNDC(1);
     TLET->SetTextFont(42);   
     TLET->Draw("same");
   }


   Double_t XRange ;
   if      ( vBkgdStack  .size() > 0 ) XRange = vBkgdStack  .at(0)->GetBinLowEdge(vBkgdStack  .at(0)->GetNbinsX()) - vBkgdStack  .at(0)->GetBinLowEdge(1);
   else if ( vSignalStack.size() > 0 ) XRange = vSignalStack.at(0)->GetBinLowEdge(vSignalStack.at(0)->GetNbinsX()) - vSignalStack.at(0)->GetBinLowEdge(1);
   else if ( vDataStack  .size() > 0 ) XRange = vDataStack  .at(0)->GetBinLowEdge(vDataStack  .at(0)->GetNbinsX()) - vDataStack  .at(0)->GetBinLowEdge(1);
   for ( int iCL = 0 ; iCL < (signed) CutLines.size() ; ++iCL ) {
     vector<string> CLConf = UATokenize( CutLines.at(iCL) , ':' );

     Double_t x[2], y[2], x2[2];
     Int_t n = 2;
     for (Int_t i=0;i<n;i++) {
       x[i] = atof(CLConf.at(1).c_str()) ;
       if ( CLConf.at(0) == ">" ) x2[i]= atof(CLConf.at(1).c_str()) - 0.05*( XRange ) ; 
       if ( CLConf.at(0) == "<" ) x2[i]= atof(CLConf.at(1).c_str()) + 0.05*( XRange ) ; 
     }
     if (kLogY) {
       y[0]=0.05;
       y[1]=hMax*1.5;
     } else {
       y[0]=0.;
       y[1]=hMax*1.05;
     }
      
     TGraph* gr = new TGraph(n,x,y);
     gr->SetLineWidth(2);
     gr->SetLineColor(kRed);
     gr->Draw("");

     TGraph* grF = new TGraph(2*n);
     grF->SetPoint(0,x2[0],y[0]); 
     grF->SetPoint(1,x2[0],y[1]); 
     grF->SetPoint(2,x [0],y[1]); 
     grF->SetPoint(3,x [0],y[0]); 
     grF->SetFillColor(kRed);
     grF->SetLineColor( 0);
     grF->SetFillStyle(3005);
     grF->Draw("f");

   }

   // ---- pad2: Ratio ----

   if ( DrawRatio && ( vDataStack.size() > 0 || vSignalStack.size() > 0 ) && vBkgdStack  .size() > 0 ) {

     pad2->cd();
     TH1F* ratio       ;
     if ( vDataStack.size() > 0  ) 
       ratio = (TH1F*) (vDataStack.at(0))->Clone("ratio");
     else
       ratio = (TH1F*) (vSignalStack.at(0))->Clone("ratio");

     TH1F* allmc       = (TH1F*) (vBkgdStack.at(0))->Clone("allmc");
     TH1F* uncertainty = (TH1F*) hErr ->Clone("uncertainty");

     for (Int_t ibin=1; ibin<= ratio->GetNbinsX(); ibin++) {
      Double_t mcValue = allmc->GetBinContent(ibin);
      Double_t mcError = allmc->GetBinError  (ibin);

      Double_t dtValue = ratio->GetBinContent(ibin);
      Double_t dtError = ratio->GetBinError  (ibin);

      Double_t ratioValue       = (mcValue > 0) ? dtValue/mcValue : 0.0;
      Double_t ratioError       = (mcValue > 0) ? dtError/mcValue : 0.0;
      Double_t uncertaintyError = (mcValue > 0) ? mcError/mcValue : 0.0;

      ratio->SetBinContent(ibin, ratioValue);
      ratio->SetBinError  (ibin, ratioError);

      uncertainty->SetBinContent(ibin, 1.0);
      uncertainty->SetBinError  (ibin, uncertaintyError);
     }

     uncertainty->GetYaxis()->SetRangeUser(0, 2.5); 
     uncertainty->Draw("e2");
     ratio      ->Draw("ep,same");
     //Pad2TAxis(uncertainty, (vDataStack.at(0))->GetXaxis()->GetTitle(), "data / MC"); 
     if ( vDataStack.size() > 0 )
       Pad2TAxis(uncertainty, ratio->GetXaxis()->GetTitle(), "data / MC"); 
     else 
       Pad2TAxis(uncertainty, ratio->GetXaxis()->GetTitle(), "signal / MC"); 

     //ratio-> DrawCopy(); 
     //uncertainty->DrawCopy("histsame");
     //ratio-> DrawCopy("same");


   } 
  //gPad->WaitPrimitive();
   // ---- Redraw ------
   if ( ! DrawRatio ) {
   pad1->Update();
   pad1->GetFrame()->DrawClone();
   pad1->RedrawAxis();
   pad1->Modified();
   pad1->Update();
   }
   //gPad->WaitPrimitive();

   if ( DrawRatio ) {
     //pad2->Update();
     //pad2->GetFrame()->DrawClone();
     //pad2->RedrawAxis();
     pad1->Update();
     pad1->Modified();
     pad2->Update();
     pad2->Modified();
     gPad->Update(); 
     gPad->Modified();
//     cout << " I am against redrawing " << endl;
   } 
   //gPad->WaitPrimitive();

   if ( SaveFig ) {
     TString Dir = "plots/" + Title + "/" ;
     if (!gSystem->OpenDirectory(Dir)) gSystem->MakeDirectory(Dir);
      
     //Canvas->SaveAs(Dir+CanName+".gif"); 
     Canvas->SaveAs(Dir+CanName+".png"); 
     Canvas->SaveAs(Dir+CanName+".pdf"); 
   }
   if ( DrawRatio ) {
      pad2->Close();
   }

 }
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
     Float_t BkgdErr = 0.;
     for ( vector<TH1F*>::iterator itH =  vData.begin() ; itH != vData.end() ; ++itH )   printf (" %10.2f |", (*itH)->GetBinContent(jCut) );  
     for ( vector<TH1F*>::iterator itH =  vBkgd.begin() ; itH != vBkgd.end() ; ++itH ) { printf (" %10.2f |", (*itH)->GetBinContent(jCut) ); BkgdSum += (*itH)->GetBinContent(jCut) ; }
     cout << endl ; 
     printf (" %10s |", itCut->c_str() );
     for ( vector<TH1F*>::iterator itH =  vData.begin() ; itH != vData.end() ; ++itH )   printf (" %10.2f |", (*itH)->GetBinError  (jCut) );
     for ( vector<TH1F*>::iterator itH =  vBkgd.begin() ; itH != vBkgd.end() ; ++itH ) { printf ("+-%10.2f|", (*itH)->GetBinError  (jCut) ); BkgdErr += ((*itH)->GetBinError(jCut))*((*itH)->GetBinError(jCut)) ;  }
     printf (" %10.2f |", BkgdSum ) ;
     printf (" %10.2f |",sqrt( BkgdErr) ) ;
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

void YieldLooper ( UATAnaConfig& Cfg , int& iGroup , int& iData , int& iContinue ) {

  iData  = 1 ;
  int iGMax = (Cfg.GetScanCuts())->size() ;
  if ( iGroup < 0 ) iGroup = 0 ;
  else {
    if ( iGroup < iGMax ) ++iGroup ;
    if ( iGroup == iGMax )  iContinue = 0;
  }
}

void UATAnaDisplay::Yields ( UATAnaConfig& Cfg , bool bPlot , bool bAll ) {

  int iContinue = 1 ;
  int iGroup  = -1 ;
  int iData = -1 ;
  while ( iContinue != 0 ) {


    int iSC = 1 ; 
    int iDG = 1 ;
   
    if ( ! bAll ) {
      iGroup  = -1 ;
      iData = -1 ;
      cout << endl ;
      cout << "CutFlow histograms " << endl ;
      cout << "  0 = CommonCuts" << endl;

      for ( vector<ScanCut_t>::iterator itSC = (Cfg.GetScanCuts())->begin() ; itSC != (Cfg.GetScanCuts())->end() ; ++itSC , ++iSC )  
        cout << "  " << iSC << " = " << itSC->ScanName << endl ;
      cout << "--> Please select Cut Group: " ;
      cin  >> iGroup;     
      cout << endl ; 
      cout << "  0 = All InputData" << endl;

      for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG , ++iDG ) 
        cout << "  " << iDG << " = " << itDG->GroupName << endl;
      cout << "--> Please select DataGroup: "  ;
      cin >> iData ;
    } else {
      YieldLooper(Cfg , iGroup , iData , iContinue ) ;
      
    } 
    cout << iGroup << endl;
  
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
    vector<int> vCData   ;
    vector<int> vCSignal ;
    vector<int> vCBkgd   ; 
 
    if (iGroup == 0 ) {
      CutGroup = "CommonCuts";  
      for ( vector<CommonCut_t>::iterator itCC = (Cfg.GetCommonCuts())->begin() ; itCC != (Cfg.GetCommonCuts())->end() ; ++itCC ) Cuts.push_back(itCC->NickName); 

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
  
    if ( bPlot) PlotStack   ( DataSet , CutGroup , CutLevel , kLogY , vData , vSignal  , vBkgd , vLData , vLSignal , vLBkgd , vCData , vCSignal , vCBkgd ) ;
    else        PrintYields ( DataSet , CutGroup , Cuts             , vData , vSignal  , vBkgd , vLData , vLSignal , vLBkgd ) ;

    if ( ! bAll ) {
      cout << "Next CutFlow choice ? [0/1] : " ;
      cin >> iContinue ;  
    }   
  }
  return;  
}


//----------------------------------- CPlot() ------------------------------------------

void UATAnaDisplay::CPlot ( UATAnaConfig& Cfg , bool SaveFig ) {


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
    vector<int> vCData   ;
    vector<int> vCSignal ;
    vector<int> vCBkgd   ;
    vector<string> CPExtraText ;
    for ( vector<string>::iterator itCPET = (Cfg.GetCPExtraText())->begin() ; itCPET != (Cfg.GetCPExtraText())->end() ; ++itCPET ) CPExtraText.push_back(*itCPET) ;

  
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
          bool iSPlotAtLvl = false ;
          for ( vector<TH1F*>::iterator itH = vData  .begin() ; itH != vData  .end() ; ++itH ) delete (*itH) ; vData  .clear() ; vLData  .clear() ; 
          for ( vector<TH1F*>::iterator itH = vSignal.begin() ; itH != vSignal.end() ; ++itH ) delete (*itH) ; vSignal.clear() ; vLSignal.clear() ;
          for ( vector<TH1F*>::iterator itH = vBkgd  .begin() ; itH != vBkgd  .end() ; ++itH ) delete (*itH) ; vBkgd  .clear() ; vLBkgd  .clear() ; 
          // Fill next histogram set
          for ( vector<string>::iterator itCC = (itCP->CCNickName).begin() ; itCC != (itCP->CCNickName).end() ; ++itCC ) {
            for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH ) {
              if (  *itCC ==  CutLevel  ) {
                 iSPlotAtLvl = true ;
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
          vector<string> CutLines ; 
          for ( vector<CutLines_t>::iterator itCL = (Cfg.GetCutLines())->begin() ; itCL != (Cfg.GetCutLines())->end() ; ++itCL ) {
            if ( itCL->CPNickName == itCP->NickName ) {
              for ( int iCL = 0 ; iCL < (signed) (itCL->CutLines).size() ; ++iCL ) CutLines.push_back((itCL->CutLines).at(iCL)) ;
            }
          }   
          if (iSPlotAtLvl) PlotStack   ( itCP->NickName+"_"+DataSet , CutGroup , CutLevel , itCP->kLogY , vData , vSignal  , vBkgd , vLData , vLSignal  , vLBkgd , vCData , vCSignal , vCBkgd , itCP->XaxisTitle , CPExtraText , Cfg.GetTAnaName() , Cfg.GetTargetLumi() , SaveFig , Cfg.GetDrawBgError() , Cfg.GetDrawRatio() , CutLines ) ;
        }

      } else {
        string GroupName = ((Cfg.GetDataGroups())->at(iData-1)).GroupName ;
        DataSet = GroupName ;

        int iH = 0 ;
        for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
          // clean
          bool iSPlotAtLvl = false ;
          for ( vector<TH1F*>::iterator itH = vData  .begin() ; itH != vData  .end() ; ++itH ) delete (*itH) ; vData  .clear() ; vLData  .clear() ; 
          for ( vector<TH1F*>::iterator itH = vSignal.begin() ; itH != vSignal.end() ; ++itH ) delete (*itH) ; vSignal.clear() ; vLSignal.clear() ;
          for ( vector<TH1F*>::iterator itH = vBkgd  .begin() ; itH != vBkgd  .end() ; ++itH ) delete (*itH) ; vBkgd  .clear() ; vLBkgd  .clear() ; 
          // Fill next histogram set
          for ( vector<string>::iterator itCC = (itCP->CCNickName).begin() ; itCC != (itCP->CCNickName).end() ; ++itCC ) {
            for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
              for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG , ++iH ) {
                if ( ( *itCC ==  CutLevel ) && ( itDG->GroupName == GroupName ) ) {
                  iSPlotAtLvl = true ;
                  if ( itBG->Data  ) { vData.push_back   ( (TH1F*) (PlotCCGroup.at(iH))->Clone() ) ; vLData  .push_back(itBG->BaseName) ;  }
                  if ( itBG->Bkgd  ) { 
                     TH1F* hTmp =  (TH1F*) (PlotCCGroup.at(iH))->Clone() ;
                     for ( vector<Systematic_t>::iterator itSyst = (Cfg.GetSystematic())->begin() ; itSyst != (Cfg.GetSystematic())->end() ; ++ itSyst ) {
                       //cout << "Syatematic : " << (itSyst->systName).c_str() << endl;
                       bool pFound = false ;
                       int  iSyst  = -1;
                       int  jSyst  =  0;
                       for ( vector<string>::iterator itSM  = (itSyst->systMember).begin() ; itSM != (itSyst->systMember).end() ; ++itSM , ++jSyst ) {
                         if ( (*itSM) == itBG->BaseName ) { pFound = true ; iSyst = jSyst ; }
                         //cout << (*itSM) << " =? " << itBG->BaseName << " " << pFound << endl ;
                       }   
                       if ( pFound ) {
                         for (Int_t i=1; i<= hTmp->GetNbinsX() ; i++) {
                           double syst = abs(hTmp->GetBinContent(i) * (itSyst->systVal).at(iSyst) - hTmp->GetBinContent(i))  ;
                           double err  = sqrt ( hTmp->GetBinError(i)*hTmp->GetBinError(i) + syst*syst) ;  
                           //cout <<  hTmp->GetBinContent(i) << " " << (itSyst->systVal).at(iSyst) << " " << syst << " " <<  hTmp->GetBinError(i) << " " << err << endl ; 
                           hTmp->SetBinError(i,err);
                         }
                       }
                     }
                     vBkgd.push_back   ( hTmp ) ;
                     vLBkgd  .push_back(itBG->BaseName) ;
                     vCBkgd.push_back(itBG->Color) ; 
                  }
                  if ( itBG->Signal) { 
                    bool iSAssoc = false ;
/*
                    if (  (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).size() != 0 ) {
                      for ( vector<string>::iterator itSL  = (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).begin() ;
                                                    itSL != (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).end()   ; ++itSL ) {
                         for ( vector<string>::iterator itBGM = (itBG->Members).begin() ;  itBGM != (itBG->Members).end() ; ++itBGM ) {
                           if ( (*itBGM) == (*itSL) ) {iSAssoc = true;}
                         } 
                      }
                    } else {iSAssoc = true;} 
*/
 
                    iSAssoc = true; 
                    if (iSAssoc) { vSignal.push_back ( (TH1F*) (PlotCCGroup.at(iH))->Clone() ) ; vLSignal.push_back(Cfg.GetSignalName() ) ; } //vLSignal.push_back(itBG->BaseName) ; }
                  }
                }
              }
            }
          }
          vector<string> CutLines ; 
          for ( vector<CutLines_t>::iterator itCL = (Cfg.GetCutLines())->begin() ; itCL != (Cfg.GetCutLines())->end() ; ++itCL ) {
            cout << itCL->CPNickName  << " " << itCP->NickName << endl;
            if ( itCL->CPNickName == itCP->NickName ) {
              for ( int iCL = 0 ; iCL < (signed) (itCL->CutLines).size() ; ++iCL ) CutLines.push_back((itCL->CutLines).at(iCL)) ;
            }
          } 
          if (iSPlotAtLvl) PlotStack   ( itCP->NickName+"_"+DataSet , CutGroup , CutLevel , itCP->kLogY , vData , vSignal  , vBkgd , vLData , vLSignal  , vLBkgd , vCData , vCSignal , vCBkgd , itCP->XaxisTitle , CPExtraText ,  Cfg.GetTAnaName() , Cfg.GetTargetLumi() , SaveFig , Cfg.GetDrawBgError() , Cfg.GetDrawRatio() , CutLines ) ;
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
          bool iSPlotAtLvl = false ;
          for ( vector<TH1F*>::iterator itH = vData  .begin() ; itH != vData  .end() ; ++itH ) delete (*itH) ; vData  .clear() ; vLData  .clear() ; 
          for ( vector<TH1F*>::iterator itH = vSignal.begin() ; itH != vSignal.end() ; ++itH ) delete (*itH) ; vSignal.clear() ; vLSignal.clear() ;
          for ( vector<TH1F*>::iterator itH = vBkgd  .begin() ; itH != vBkgd  .end() ; ++itH ) delete (*itH) ; vBkgd  .clear() ; vLBkgd  .clear() ; 
          // Fill next histogram set
          for ( vector<string>::iterator itCC = (itCP->SCNickName).begin() ; itCC != (itCP->SCNickName).end() ; ++itCC ) {
           for ( vector<ScanCut_t>::iterator itSC = (Cfg.GetScanCuts())->begin() ; itSC != (Cfg.GetScanCuts())->end() ; ++itSC , ++iSC ) { 
            for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH ) {
              if ( ( *itCC ==  CutLevel ) && ( itSC->ScanName == ScanName ) ) {
                 iSPlotAtLvl = true ;
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
          vector<string> CutLines ; 
          for ( vector<CutLines_t>::iterator itCL = (Cfg.GetCutLines())->begin() ; itCL != (Cfg.GetCutLines())->end() ; ++itCL ) {
            if ( itCL->CPNickName == itCP->NickName ) {
              for ( int iCL = 0 ; iCL < (signed) (itCL->CutLines).size() ; ++iCL ) CutLines.push_back((itCL->CutLines).at(iCL)) ;
            }
          } 
          if (iSPlotAtLvl) PlotStack   ( itCP->NickName+"_"+DataSet , CutGroup , CutLevel , itCP->kLogY , vData , vSignal  , vBkgd , vLData , vLSignal  , vLBkgd , vCData , vCSignal , vCBkgd , itCP->XaxisTitle , CPExtraText ,  Cfg.GetTAnaName() , Cfg.GetTargetLumi() ,  SaveFig , Cfg.GetDrawBgError() , Cfg.GetDrawRatio(), CutLines ) ;
        }

      } else {
        string GroupName = ((Cfg.GetDataGroups())->at(iData-1)).GroupName ;
        DataSet = GroupName ;

        int iH = 0 ;
        for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
          // clean
          bool iSPlotAtLvl = false ;
          for ( vector<TH1F*>::iterator itH = vData  .begin() ; itH != vData  .end() ; ++itH ) delete (*itH) ; vData  .clear() ; vLData  .clear() ; 
          for ( vector<TH1F*>::iterator itH = vSignal.begin() ; itH != vSignal.end() ; ++itH ) delete (*itH) ; vSignal.clear() ; vLSignal.clear() ;
          for ( vector<TH1F*>::iterator itH = vBkgd  .begin() ; itH != vBkgd  .end() ; ++itH ) delete (*itH) ; vBkgd  .clear() ; vLBkgd  .clear() ; 
          // Fill next histogram set
          for ( vector<string>::iterator itCC = (itCP->SCNickName).begin() ; itCC != (itCP->SCNickName).end() ; ++itCC ) {
           for ( vector<ScanCut_t>::iterator itSC = (Cfg.GetScanCuts())->begin() ; itSC != (Cfg.GetScanCuts())->end() ; ++itSC , ++iSC ) { 
            for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
              for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG , ++iH ) {
                if ( ( *itCC ==  CutLevel ) && ( itSC->ScanName == ScanName ) && ( itDG->GroupName == GroupName ) ) {
                  iSPlotAtLvl = true ;
                  if ( itBG->Data  ) { vData.push_back   ( (TH1F*) (PlotSCGroup.at(iH))->Clone() ) ; vLData  .push_back(itBG->BaseName) ; }
                  //if ( itBG->Bkgd  ) { vBkgd.push_back   ( (TH1F*) (PlotSCGroup.at(iH))->Clone() ) ; vLBkgd  .push_back(itBG->BaseName) ; vCBkgd.push_back(itBG->Color) ; }
                  if ( itBG->Bkgd  ) { 
                     TH1F* hTmp =  (TH1F*) (PlotSCGroup.at(iH))->Clone() ;
                     for ( vector<Systematic_t>::iterator itSyst = (Cfg.GetSystematic())->begin() ; itSyst != (Cfg.GetSystematic())->end() ; ++ itSyst ) {
                       //cout << "Syatematic : " << (itSyst->systName).c_str() << endl;
                       bool pFound = false ;
                       int  iSyst  = -1;
                       int  jSyst  =  0;
                       for ( vector<string>::iterator itSM  = (itSyst->systMember).begin() ; itSM != (itSyst->systMember).end() ; ++itSM , ++jSyst ) {
                         if ( (*itSM) == itBG->BaseName ) { pFound = true ; iSyst = jSyst ; }
                         //cout << (*itSM) << " =? " << itBG->BaseName << " " << pFound << endl ;
                       }   
                       if ( pFound ) {
                         for (Int_t i=1; i<= hTmp->GetNbinsX() ; i++) {
                           double syst = abs(hTmp->GetBinContent(i) * (itSyst->systVal).at(iSyst) - hTmp->GetBinContent(i))  ;
                           double err  = sqrt ( hTmp->GetBinError(i)*hTmp->GetBinError(i) + syst*syst) ;  
                           //cout <<  hTmp->GetBinContent(i) << " " << (itSyst->systVal).at(iSyst) << " " << syst << " " <<  hTmp->GetBinError(i) << " " << err << endl ; 
                           hTmp->SetBinError(i,err);
                         }
                       }
                     }
                     vBkgd.push_back   ( hTmp ) ;
                     vLBkgd  .push_back(itBG->BaseName) ;
                     vCBkgd.push_back(itBG->Color) ; 
                  }


                  if ( itBG->Signal) {
                    bool iSAssoc = false ; 
/*
                    if (  (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).size() != 0 ) {
                       for ( vector<string>::iterator itSL  = (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).begin() ;
                                                      itSL != (((Cfg.GetScanCuts())->at(iGroup-1)).SignList).end()   ; ++itSL ) {
                         for ( vector<string>::iterator itBGM = (itBG->Members).begin() ;  itBGM != (itBG->Members).end() ; ++itBGM ) {
                           if ( (*itBGM) == (*itSL) ) {iSAssoc = true;}
                         } 
                       }
                    } else {iSAssoc = true;} 
*/
                    iSAssoc = true ;
                    if (iSAssoc) { vSignal.push_back ( (TH1F*) (PlotSCGroup.at(iH))->Clone() ) ;  vLSignal.push_back(Cfg.GetSignalName() ) ; } // vLSignal.push_back(itBG->BaseName) ; }
                  }
                }
              }
            }
           }
          }
          vector<string> CutLines ; 
          for ( vector<CutLines_t>::iterator itCL = (Cfg.GetCutLines())->begin() ; itCL != (Cfg.GetCutLines())->end() ; ++itCL ) {
            if ( itCL->CPNickName == itCP->NickName ) {
              for ( int iCL = 0 ; iCL < (signed) (itCL->CutLines).size() ; ++iCL ) CutLines.push_back((itCL->CutLines).at(iCL)) ;
            }
          } 
          if (iSPlotAtLvl ) PlotStack   ( itCP->NickName+"_"+DataSet , CutGroup , CutLevel , itCP->kLogY , vData , vSignal  , vBkgd , vLData , vLSignal  , vLBkgd , vCData , vCSignal , vCBkgd , itCP->XaxisTitle , CPExtraText ,  Cfg.GetTAnaName() , Cfg.GetTargetLumi() , SaveFig , Cfg.GetDrawBgError() , Cfg.GetDrawRatio(), CutLines ) ;
        }
      } 


    }

    cout << "Next choice ? [0/1] : " ;
    cin >> iContinue ;     
  }

  return;
}

// --------------------------- LimitCard ()

void UATAnaDisplay::LimitCard ( UATAnaConfig& Cfg ) {

  string LimitCardName ;
  Double_t Data = 0  ;
  Double_t eStatData = 0  ;
  vector<Double_t>  Signal ;
  vector<Double_t>  eStatSignal ;
  vector<Double_t>  Background ;
  vector<Double_t>  eStatBkgd ;
  vector<string> Proc;
  vector<string> BgProc;

  cout << "[UATAnaDisplay::LimitCard] nGroup = " << (Cfg.GetDataGroups())->size() << " nScan = " << (Cfg.GetScanCuts())->size() << endl ; 

  // Loop on DataGroup (-1:All InputData)
  //for ( int iDG = -1 ; iDG < (signed) (Cfg.GetDataGroups())->size() ; ++iDG ) {  
  { int iDG = -1 ;
    // Loop on ScanCut (-1:CommonCuts)
    //for ( int iSC = -1  ; iSC < (signed) (Cfg.GetScanCuts())->size() ; ++iSC ) {  
    for ( int iSC = 0 ; iSC < (signed) (Cfg.GetScanCuts())->size() ; ++iSC ) {  

      Data = 0  ;
      eStatData = 0  ; 
      Proc.clear();
      BgProc.clear();
      Signal.clear();
      eStatSignal.clear();
      Background.clear();
      eStatBkgd.clear();

      if ( iDG == -1 ) {
        if ( iSC == -1 ) {
          // Data + Signal
          int iH = 0 ;
          for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH ) { 
            if ( itD->Data  ) { 
              Data       += (CCflow.at(iH))->GetBinContent((CCflow.at(iH))->GetNbinsX()) ;
              eStatData  += pow((CCflow.at(iH))->GetBinError((CCflow.at(iH))->GetNbinsX()),2) ; 
            }
            if ( itD->Signal) {
              bool iSAssoc = false ;
              if (  (Cfg.GetCommonSign())->size() != 0 ) {
                for ( vector<string>::iterator itSL  = (Cfg.GetCommonSign())->begin() ; itSL != (Cfg.GetCommonSign())->end() ; ++itSL ) {
                  if ( itD->NickName == (*itSL) ) {iSAssoc = true;}
                }
              } else {iSAssoc = true;}
              if (iSAssoc) {  
                 Signal     .push_back( (CCflow.at(iH))->GetBinContent((CCflow.at(iH))->GetNbinsX()) ); 
                 eStatSignal.push_back( (CCflow.at(iH))->GetBinError((CCflow.at(iH))->GetNbinsX())   );
                 Proc.push_back( itD->NickName) ;
               }
            }
          }
          eStatData = sqrt(eStatData);
          // Background
          iH = 0 ; 
          for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH ) {
            if ( itD->Bkgd  ) {
               Background.push_back( (CCflow.at(iH))->GetBinContent((CCflow.at(iH))->GetNbinsX()) ); 
               eStatBkgd .push_back( (CCflow.at(iH))->GetBinError((CCflow.at(iH))->GetNbinsX()) ); 
               Proc.push_back( itD->NickName) ;
               BgProc.push_back( itD->NickName) ;
            }
          }
        } else {
          // Data + Signal
          int iH = 0 ;
          for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH) {
            if ( itD->Data  ) { 
              if ( ! Cfg.GetMaskData() ) {
                Data      +=      (((SCflow.at(iSC)).CutFlow).at(iH))->GetBinContent((((SCflow.at(iSC)).CutFlow).at(iH))->GetNbinsX()) ;
                eStatData += pow( (((SCflow.at(iSC)).CutFlow).at(iH))->GetBinError((((SCflow.at(iSC)).CutFlow).at(iH))->GetNbinsX()) , 2 ) ;
              }
            }
            if ( itD->Signal) {
              bool iSAssoc = false ;
              if (  (((Cfg.GetScanCuts())->at(iSC)).SignList).size() != 0 ) {
                for ( vector<string>::iterator itSL  = (((Cfg.GetScanCuts())->at(iSC)).SignList).begin() ;
                                                  itSL != (((Cfg.GetScanCuts())->at(iSC)).SignList).end()   ; ++itSL ) {
                  if ( itD->NickName == (*itSL) ) {iSAssoc = true;}
                }
/*              if (  (Cfg.GetCommonSign())->size() != 0 ) {
                for ( vector<string>::iterator itSL  = (Cfg.GetCommonSign())->begin() ; itSL != (Cfg.GetCommonSign())->end() ; ++itSL ) {
                  if ( itD->NickName == (*itSL) ) {iSAssoc = true;}
                }
*/
              } else {iSAssoc = true;}
              if (iSAssoc) {
                Signal     .push_back( (((SCflow.at(iSC)).CutFlow).at(iH))->GetBinContent((((SCflow.at(iSC)).CutFlow).at(iH))->GetNbinsX()) ) ; 
                eStatSignal.push_back( (((SCflow.at(iSC)).CutFlow).at(iH))->GetBinError((((SCflow.at(iSC)).CutFlow).at(iH))->GetNbinsX())   ) ;
                Proc.push_back( itD->NickName) ;  
              }
            }
          }
          eStatData = sqrt(eStatData);
          // Background
          iH = 0 ; 
          for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH) {
            if ( itD->Bkgd  ) {
               //Background.push_back( (((SCflow.at(iSC)).CutFlow).at(iH))->GetBinContent((((SCflow.at(iSC)).CutFlow).at(iH))->GetNbinsX()) ) ; 
               //eStatBkgd .push_back( (((SCflow.at(iSC)).CutFlow).at(iH))->GetBinError((((SCflow.at(iSC)).CutFlow).at(iH))->GetNbinsX())   ) ;
               float Bkgd  = (((SCflow.at(iSC)).CutFlow).at(iH))->GetBinContent((((SCflow.at(iSC)).CutFlow).at(iH))->GetNbinsX());
               float eBkgd = (((SCflow.at(iSC)).CutFlow).at(iH))->GetBinError((((SCflow.at(iSC)).CutFlow).at(iH))->GetNbinsX()) ;
               // DDE
               for ( vector<SyDDEstim_t>::iterator itDDE = (Cfg.GetSyDDEstim())->begin() ; itDDE != (Cfg.GetSyDDEstim())->end() ; ++itDDE ) {
                 for ( vector<string>::iterator itDDEMemb = (itDDE->SyDDEMember).begin() ; itDDEMemb != (itDDE->SyDDEMember).end() ; ++itDDEMemb ) {
                   if ( itD->NickName == (*itDDEMemb) ) {
 
                     // ... Loop on DDE datacards and fetch: NumEventsInCtrlRegion scaleToSignRegion uncertaintyOnScaleToSignRegion
                     float RateOrigin = Bkgd ;
                     float RateTarget = 0. ;
                     for ( vector<string>::iterator itDDECard = (itDDE->SyDDECards).begin() ; itDDECard != (itDDE->SyDDECards).end() ; ++itDDECard ) {
                       float N (0), s(0), u(0) ;
                       getsyst(*itDDECard,Cfg.GetHiggsMass(),N,s,u);
                       if (s < 0) continue;
                       RateTarget += N*s ;
                     }

                     // ... Normalize Bkgd
                     if (RateTarget > 0) {
                       float ScaleRate  = RateTarget/RateOrigin;                    
                       Bkgd  *= ScaleRate;
                       eBkgd *= ScaleRate;
                     }

                   } 
                 }
               }
               Background.push_back( Bkgd  ) ;
               eStatBkgd .push_back( eBkgd );
               Proc.push_back( itD->NickName) ;
               BgProc.push_back( itD->NickName) ;
               if ( Cfg.GetMaskData() ) {
                 Data += Bkgd ;
               }
             }
          } 
        }
      } 
      if ( Cfg.GetMaskData() && Data > 0. ) {
        eStatData = sqrt(Data) ;
      }
  
      

      LimitCardName = "LimitCards/" + Cfg.GetTAnaName() ;
      if      ( iDG == -1 ) LimitCardName += "_AllData" ;
      else if ( iDG >=  0 ) LimitCardName += "_" + ((Cfg.GetDataGroups())->at(iDG)).GroupName ;
      if      ( iSC == -1 ) LimitCardName += "_CommonCuts" ;
      else if ( iSC >=  0 ) LimitCardName += "_" + ((Cfg.GetScanCuts())->at(iSC)).ScanName ;
      LimitCardName += "__CutBased.card" ;
      cout << "[UATAnaDisplay::LimitCard] Writing: " << LimitCardName << endl;

      FILE *cFile;
      cFile = fopen (LimitCardName.c_str(),"w");

      // ... Data, signal and bkgd rates
      fprintf (cFile,"imax 1 number of channels\n");
      fprintf (cFile,"jmax * number of background\n");
      fprintf (cFile,"kmax * number of nuisance parameters\n");
      fprintf (cFile,"----------------------------------------------------------------------------------------------------------------------------------\n");
      fprintf (cFile,"bin         %-9s \n",(Cfg.GetLimBinName()).c_str()) ;
      fprintf (cFile,"Observation %-9.3f \n",Data);
      fprintf (cFile,"----------------------------------------------------------------------------------------------------------------------------------\n");
      fprintf (cFile,"bin                                         ");
      for ( int iD=0 ; iD < (signed) Signal    .size() ; ++iD ) fprintf (cFile,"%-9s ",(Cfg.GetLimBinName()).c_str()) ;
      for ( int iD=0 ; iD < (signed) Background.size() ; ++iD ) fprintf (cFile,"%-9s ",(Cfg.GetLimBinName()).c_str()) ;
      fprintf (cFile,"\n") ;
      fprintf (cFile,"process                                     ") ;
      for ( int iD=0 ; iD < (signed) Proc      .size() ; ++iD ) fprintf (cFile,"%-9s ",(Proc.at(iD)).c_str());
      fprintf (cFile,"\n") ;  
      fprintf (cFile,"process                                     ") ;
      for ( int iD=1 ; iD <= (signed) Signal    .size() ; ++iD ) fprintf (cFile,"%-9i ", - (signed) Signal.size() + iD );
      for ( int iD=1 ; iD <= (signed) Background.size() ; ++iD ) fprintf (cFile,"%-9i ",iD);
      fprintf (cFile,"\n") ;
      fprintf (cFile,"rate                                        ") ;
      for ( int iD=0 ; iD < (signed) Signal    .size() ; ++iD ) fprintf (cFile,"%-9.3f ",Signal.at(iD)) ;
      for ( int iD=0 ; iD < (signed) Background.size() ; ++iD ) fprintf (cFile,"%-9.3f ",Background.at(iD)) ;
      fprintf (cFile,"\n") ;

      // ... Syst Errors
      fprintf (cFile,"----------------------------------------------------------------------------------------------------------------------------------\n");
      for ( vector<Systematic_t>::iterator itSyst = (Cfg.GetSystematic())->begin() ; itSyst != (Cfg.GetSystematic())->end() ; ++ itSyst ) {
        fprintf (cFile,"%-30s %-5s        ",(itSyst->systName).c_str(),(itSyst->systType).c_str());
        for ( vector<string>::iterator itProc =  Proc.begin() ; itProc != Proc.end() ; ++itProc) {
          bool pFound = false ;
          int  iSyst  = -1;
          int  jSyst  =  0;
          for ( vector<string>::iterator itSM  = (itSyst->systMember).begin() ; itSM != (itSyst->systMember).end() ; ++itSM , ++jSyst ) {
            if ( (*itSM) == (*itProc) ) { pFound = true ; iSyst = jSyst ; }
          }
          if ( pFound )  fprintf (cFile,"%-5.3f     ",(itSyst->systVal).at(iSyst));
          else           fprintf (cFile,"  -       "); 
        }
        fprintf (cFile,"\n") ; 
      } 

      // ... Data Driven Estimate Errors 
      fprintf (cFile,"----------------------------------------------------------------------------------------------------------------------------------\n");
      for ( vector<SyDDEstim_t>::iterator itDDE = (Cfg.GetSyDDEstim())->begin() ; itDDE != (Cfg.GetSyDDEstim())->end() ; ++itDDE ) {
        for ( vector<string>::iterator itDDEMemb = (itDDE->SyDDEMember).begin() ; itDDEMemb != (itDDE->SyDDEMember).end() ; ++itDDEMemb ) {

          // 0) Find back process
          int iB = -1 ;
          int iD = -1 ;
          for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD ) {
            if ( itD->Bkgd  ) { ++iD; if ( itD->NickName == (*itDDEMemb) ) iB = iD ; }
          }
          if ( iB == -1 ) {
            cout << "[UATAnaDisplay::LimitCard] ERROR: DDE Proc not found : " << (*itDDEMemb) << endl;
            continue;
          }

          // 1) Compute from datacards
          // ... N in signal region
          float Nsig = Background.at(iB) ;
         
          // ... Loop on DDE datacards and fetch: NumEventsInCtrlRegion scaleToSignRegion uncertaintyOnScaleToSignRegion
          vector<float> NSigRegion ;
          vector<float> NCtrlRegion ;
          vector<float> Scale2Sign  ;
          vector<float> UScale2Sign ;
          for ( vector<string>::iterator itDDECard = (itDDE->SyDDECards).begin() ; itDDECard != (itDDE->SyDDECards).end() ; ++itDDECard ) {
            float N (Nsig), s(0), u(0) ;
            getsyst(*itDDECard,Cfg.GetHiggsMass(),N,s,u);
            if (s < 0) continue;
            NCtrlRegion.push_back(N);
            Scale2Sign .push_back(s);
            UScale2Sign.push_back(u);
          }          

          if (NCtrlRegion.size() == 0) continue;

          // ...  Sum Up
          float Nctrl (0) ;
          Nctrl = NCtrlRegion.at(0);
          float scfac = Nsig/Nctrl;
          float unc (0) ;
          for ( int j = 0 ; j < (signed) NCtrlRegion.size() ; ++j) unc += UScale2Sign.at(j); 

          // ... and the final stuff
          float extr(0);
          if ( (itDDE->SyDDEType) == "lnN" ) extr = 1+unc/scfac;  // syst w.r.t 1
          if ( (itDDE->SyDDEType) == "gmM" ) extr = unc/scfac;    // syst w.r.t 0

          // 2) Write result
          // ... lnN/gmM
          string extrName = itDDE->SyDDEName + "_extr";
          fprintf (cFile,"%-30s %-5s        ",extrName.c_str() , (itDDE->SyDDEType).c_str() ) ;
          for ( vector<string>::iterator itProc =  Proc.begin() ; itProc != Proc.end() ; ++itProc) {
            if ( (*itDDEMemb) == (*itProc) ) fprintf (cFile,"%-5.3f     ",extr) ;
            else                                                     fprintf (cFile,"  -       ");
          }
          fprintf (cFile,"\n") ;
          // ... gmN
          extrName = itDDE->SyDDEName + "_stat";
          fprintf (cFile,"%-30s %-5s %-5.0f  ",extrName.c_str() , "gmN" , Nctrl ) ;
          for ( vector<string>::iterator itProc =  Proc.begin() ; itProc != Proc.end() ; ++itProc) {
            if ( (*itDDEMemb) == (*itProc) ) fprintf (cFile,"%-6.4f    ", scfac ) ;
            else                                                     fprintf (cFile,"  -       ");
          }
          fprintf (cFile,"\n") ;

        }
      }

      // ... Stat Errors 
      fprintf (cFile,"----------------------------------------------------------------------------------------------------------------------------------\n");
      int iProc = 0 ;
      for ( int iD=0 ; iD < (signed) Signal    .size() ; ++iD , ++iProc ) { 
        double eStaRel = 1. ;
        if (Signal.at(iD)>0.) eStaRel += eStatSignal.at(iD) / Signal.at(iD) ;
        string statName = "stat_" + Proc.at(iProc) ;
        string statType = "lnN" ;   
        fprintf (cFile,"%-30s %-5s        ",statName.c_str() , statType.c_str() ) ;
        for ( int jProc = 0 ; jProc < (signed) Proc.size() ; ++jProc ) {
           if ( jProc == iProc ) {
             if ( Signal.at(iD)>0.) fprintf (cFile,"%-5.3f     ",eStaRel) ;
             else                   fprintf (cFile,"  inf     ");
           }
           else                  fprintf (cFile,"  -       ");
        } 
        fprintf (cFile,"\n") ; 
      }
      for ( int iD=0 ; iD < (signed) Background.size() ; ++iD , ++iProc ) { 
        // skip if DDE
        bool isDDE = false ;
        for ( vector<SyDDEstim_t>::iterator itDDE = (Cfg.GetSyDDEstim())->begin() ; itDDE != (Cfg.GetSyDDEstim())->end() ; ++itDDE ) {
          for ( vector<string>::iterator itDDEMemb = (itDDE->SyDDEMember).begin() ; itDDEMemb != (itDDE->SyDDEMember).end() ; ++itDDEMemb ) {
            if ( BgProc.at(iD) == (*itDDEMemb) ) {
              for ( vector<string>::iterator itDDECard = (itDDE->SyDDECards).begin() ; itDDECard != (itDDE->SyDDECards).end() ; ++itDDECard ) {
                float N (0), s(0), u(0) ;
		getsyst(*itDDECard,Cfg.GetHiggsMass(),N,s,u);
                if ( s>0 ) isDDE = true ;
              }
            }            
          }
        }
        if ( isDDE ) continue;
        double eStaRel = 1. ;
        if (Background.at(iD)>0.) eStaRel += eStatBkgd.at(iD) / Background.at(iD) ;
        string statName = "stat_" + Proc.at(iProc) ;
        string statType = "lnN" ;   
        fprintf (cFile,"%-30s %-5s        ",statName.c_str() , statType.c_str() ) ;
        for ( int jProc = 0 ; jProc < (signed) Proc.size() ; ++jProc ) {
           if ( jProc == iProc ) {
             if (Background.at(iD)>0.) fprintf (cFile,"%-5.3f     ",eStaRel) ;
             else                      fprintf (cFile,"  inf     "); 
           }
           else                  fprintf (cFile,"  -       ");
        } 
        fprintf (cFile,"\n") ; 
      }

      fclose(cFile);

    }
  } // DataGroup Loop

}
