
#include "../includes/UATAnaReader.h"

//----------------------------------- Init() ------------------------------------------

void UATAnaReader::Init ( UATAnaConfig& Cfg , bool& bWTree ) {

  TH1::SetDefaultSumw2(1);

  // CommonCuts cutflow histograms
  for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD) {
    TString HistName = "CutFlow_"+itD->NickName ;
    CCflow.push_back( new TH1F( HistName , HistName , (Cfg.GetCommonCuts())->size() , 0 ,  (Cfg.GetCommonCuts())->size() )); 
  }
  for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
    for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG  ) {
      TString HistName = "CutFlow_"+itDG->GroupName+"_"+itBG->BaseName ;
      CCflowGroup.push_back( new TH1F( HistName , HistName , (Cfg.GetCommonCuts())->size() , 0 ,  (Cfg.GetCommonCuts())->size() )); 
    }
  }

  // ScanCuts cutflow histograms
  for ( vector<ScanCut_t>::iterator iSCG = (Cfg.GetScanCuts())->begin() ; iSCG != (Cfg.GetScanCuts())->end() ; ++iSCG) {
    ScanFlow_t ScanFlow ;
    ScanFlow.ScanName = iSCG->ScanName ;
    for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD) {
      TString HistName = "ScanFlow_"+iSCG->ScanName+"_"+itD->NickName ; 
      (ScanFlow.CutFlow).push_back( new TH1F( HistName , HistName , (iSCG->Cuts).size() , 0 , (iSCG->Cuts).size() )); 
    }
    SCflow.push_back( ScanFlow ) ;
  } 
  for ( vector<ScanCut_t>::iterator iSCG = (Cfg.GetScanCuts())->begin() ; iSCG != (Cfg.GetScanCuts())->end() ; ++iSCG) {
    ScanFlow_t ScanFlow ;
    ScanFlow.ScanName = iSCG->ScanName ;
    for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
      for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG  ) {
        TString HistName = "ScanFlow_"+iSCG->ScanName+"_"+itDG->GroupName+"_"+itBG->BaseName ; 
        (ScanFlow.CutFlow).push_back( new TH1F( HistName , HistName , (iSCG->Cuts).size() , 0 , (iSCG->Cuts).size() ));  
      } 
    } 
    SCflowGroup.push_back( ScanFlow ) ;
  } 

  // CtrlPlots histograms 
  for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
    // CommonCuts
    for ( vector<string>::iterator itCC = (itCP->CCNickName).begin() ; itCC != (itCP->CCNickName).end() ; ++itCC ) {
      for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD) {
        TString HistName = "PlotCC_"+itCP->NickName+"_"+*itCC+"_"+itD->NickName;
        PlotCC.push_back ( new TH1F( HistName , HistName ,itCP->nBins,itCP->xMin,itCP->xMax) ) ;  
      } 
      for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
        for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG  ) {
          TString HistName = "PlotCC_"+itCP->NickName+"_"+*itCC+"_"+itDG->GroupName+"_"+itBG->BaseName ;
          PlotCCGroup.push_back ( new TH1F( HistName , HistName ,itCP->nBins,itCP->xMin,itCP->xMax) ) ;   
        }
      } 
    }
    // ScanCuts
    for ( vector<string>::iterator itSC = (itCP->SCNickName).begin() ; itSC != (itCP->SCNickName).end() ; ++itSC ) {
      for ( vector<ScanCut_t>::iterator iSCG = (Cfg.GetScanCuts())->begin() ; iSCG != (Cfg.GetScanCuts())->end() ; ++iSCG) { 
        for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD) {
          TString HistName = "PlotSC_"+iSCG->ScanName+"_"+itCP->NickName+"_"+*itSC+"_"+itD->NickName;
          PlotSC.push_back ( new TH1F( HistName , HistName ,itCP->nBins,itCP->xMin,itCP->xMax) ) ;  
        }
        for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
          for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG  ) {
            TString HistName = "PlotSC_"+iSCG->ScanName+"_"+itCP->NickName+"_"+*itSC+"_"+itDG->GroupName+"_"+itBG->BaseName ;
            PlotSCGroup.push_back ( new TH1F( HistName , HistName ,itCP->nBins,itCP->xMin,itCP->xMax) ) ;   
          }
        } 
      }
    }
  }

  // Output TTree
  // if ( bWTree ) InitOutTree (Cfg);
  

}

//----------------------------------- Analyze() ----------------------------------------

void UATAnaReader::Analyze( UATAnaConfig& Cfg , bool& bWTree ) {

 // Loop on all tree -------------------------------------------------------

 int iD=0 ;
 for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iD) {

   // ------------ Open ------------------------ 

   cout << "Opening TTree: " << itD->NickName << " " << itD->FileName << " " << itD->TreeName << endl;
   File = new TFile((itD->FileName).c_str(),"READ");  
   Tree = (TTree*) File->Get((itD->TreeName).c_str());
   Tree->SetBranchStatus("*",1);

   // Duplicate TTree as needed

   if ( bWTree ) {
     for ( vector<OutTTree_t>::iterator itOTT = (Cfg.GetOutTTree())->begin() ; itOTT != (Cfg.GetOutTTree())->end() ; ++itOTT ) {
       if ( itD->NickName == itOTT->DataName ) {
         // Make file names 
         (itOTT->OutNOne) = Cfg.GetOutDir() + "/" + Cfg.GetTAnaName() + "_" + itOTT->DataName + "_" + itOTT->CutName ;
         if ( itOTT->Split ) {
           (itOTT->OutNTwo) = (itOTT->OutNOne) + "_Sample2" + ".root" ;
           (itOTT->OutNOne) += "_Sample1" ;
         }
         (itOTT->OutNOne) += ".root" ;
         // Create Clone
         (itOTT->OutFOne) = new TFile((itOTT->OutNOne).c_str(),"RECREATE");     
         (itOTT->OutTOne) = Tree->CloneTree(0);
         (itOTT->OutTOne)->SetName((itOTT->TreeName).c_str());
         if ( itOTT->Split ) {
           (itOTT->OutFTwo) = new TFile((itOTT->OutNTwo).c_str(),"RECREATE");     
           (itOTT->OutTTwo) = Tree->CloneTree(0);
           (itOTT->OutTTwo)->SetName((itOTT->TreeName).c_str());
         }
       }
     }

     //string OT1FileName = 
     //string tmpName = (itD->NickName) + ".tmp.root";
     //TFile* tmpFile =  new TFile(tmpName.c_str(),"RECREATE"); 
     //TTree* tmpTree = Tree->CloneTree(0);

   }

   // ------------- Formulas -------------------

   // Create TreeWeight Formula
   (Cfg.GetTreeWeight())->MakFormula(Tree);  
   // Create DataSetWght Formula
   for ( vector<DataSetWght_t>::iterator itDSW = (Cfg.GetDataSetWghts())->begin() ; itDSW != (Cfg.GetDataSetWghts())->end() ; ++itDSW ) itDSW->MakFormula(Tree); 
   // Create CommonCuts Formula   
   for ( vector<TreeFormula_t>::iterator itCC = (Cfg.GetCommonCuts())->begin() ; itCC != (Cfg.GetCommonCuts())->end() ; ++itCC) itCC->MakFormula(Tree);
   // Create ScanCuts Formula
   for ( vector<ScanCut_t>::iterator itSCG = (Cfg.GetScanCuts())->begin() ; itSCG != (Cfg.GetScanCuts())->end() ; ++itSCG) {
     for  ( vector<TreeFormula_t>::iterator itSC = (itSCG->Cuts).begin() ; itSC != (itSCG->Cuts).end() ; ++itSC ) itSC->MakFormula(Tree);
   }
   // Create CtrlPlots Formula
   for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP) itCP->MakFormula(Tree);


   // ------------- ExtEffTH2 ------------------
   for ( vector<ExtEffTH2_t>::iterator itEff = (Cfg.GetExtEffTH2())->begin() ; itEff != (Cfg.GetExtEffTH2())->end() ; ++itEff ) itEff->MakExtEffTH2(Tree);  

   // ------------- Loop -----------------------

   Int_t nEntries = Tree->GetEntries();
   for (Int_t jEntry = 0 ;  jEntry < nEntries ; ++jEntry) { 

     Tree->GetEntry(jEntry);

     // Evaluate TreeWeight Formula
     (Cfg.GetTreeWeight())->EvaFormula();  
     // Evaluate DataSetWght Formula
     for ( vector<DataSetWght_t>::iterator itDSW = (Cfg.GetDataSetWghts())->begin() ; itDSW != (Cfg.GetDataSetWghts())->end() ; ++itDSW ) itDSW->EvaFormula();
     // Evaluate CommonCuts Formula   
     for ( vector<TreeFormula_t>::iterator itCC = (Cfg.GetCommonCuts())->begin() ; itCC != (Cfg.GetCommonCuts())->end() ; ++itCC) itCC->EvaFormula();
     // Evaluate ScanCuts Formula
     for ( vector<ScanCut_t>::iterator itSCG = (Cfg.GetScanCuts())->begin() ; itSCG != (Cfg.GetScanCuts())->end() ; ++itSCG) {
       for  ( vector<TreeFormula_t>::iterator itSC = (itSCG->Cuts).begin() ; itSC != (itSCG->Cuts).end() ; ++itSC ) itSC->EvaFormula();
     }
     // Evaluate CtrlPlots Formula
     for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP) itCP->EvaFormula();
     // Evaluate ExtEffTH2 
     for ( vector<ExtEffTH2_t>::iterator itEff = (Cfg.GetExtEffTH2())->begin() ; itEff != (Cfg.GetExtEffTH2())->end() ; ++itEff ) itEff->EvaExtEffTH2();  

     // ------------- DataSetWeight --------------
     Double_t DataSetWeight = 1.0 ;
     for ( vector<DataSetWght_t>::iterator itDSW = (Cfg.GetDataSetWghts())->begin() ; itDSW != (Cfg.GetDataSetWghts())->end() ; ++itDSW ) { 
       for ( vector<string>::iterator itDSN = (itDSW->DataSets).begin() ; itDSN != (itDSW->DataSets).end() ; ++itDSN ) {
         if ( (*itDSN) == itD->NickName ) DataSetWeight *= itDSW->Result() ;
       }
     }
     // Compute Event Weight 
     Double_t Weight = DataSetWeight * (Cfg.GetTreeWeight())->Result() ;
     if ( ! itD->Data )  Weight *= Cfg.GetTargetLumi() / itD->Lumi ;
     for ( vector<ExtEffTH2_t>::iterator itEff = (Cfg.GetExtEffTH2())->begin() ; itEff != (Cfg.GetExtEffTH2())->end() ; ++itEff )  Weight *= itEff->Result(itD->NickName) ;

     // CommonCuts
     bool passCC = true ;
     int  iCC = 0 ;
     for ( vector<TreeFormula_t>::iterator itCC = (Cfg.GetCommonCuts())->begin() ; itCC != (Cfg.GetCommonCuts())->end() ; ++itCC , ++iCC ) {
       if ( !passCC ) continue; 
       if ( itCC->Result() ) {
         FillCutFlow(Cfg,itD->NickName,iCC,Weight);
       } else {
         passCC = false ; 
       } 
     }

     // ScanCuts
     if (passCC) {
       for ( vector<ScanCut_t>::iterator itSCG = (Cfg.GetScanCuts())->begin() ; itSCG != (Cfg.GetScanCuts())->end() ; ++itSCG) {
         bool passSC = true ; 
         int  iSC = 0 ;
         for ( vector<TreeFormula_t>::iterator itSC =  (itSCG->Cuts).begin() ; itSC != (itSCG->Cuts).end() ; ++itSC , ++iSC ) {
           if ( !passSC ) continue; 
           if ( itSC->Result() ) {
             FillScanFlow(Cfg,itSCG->ScanName,itD->NickName,iSC,Weight);
           } else {
             passSC = false ; 
           } 
         }  
       }
     } 

     // Control Plots
     FillPlotCC( Cfg , itD->NickName , Weight );
     if (passCC) FillPlotSC( Cfg , itD->NickName , Weight );

     // Output TTree
     if ( bWTree && Weight != 0 ) {
       for ( vector<OutTTree_t>::iterator itOTT = (Cfg.GetOutTTree())->begin() ; itOTT != (Cfg.GetOutTTree())->end() ; ++itOTT ) {
         if ( itD->NickName == itOTT->DataName ) {
           bool passTCC = true ;
           for ( vector<TreeFormula_t>::iterator itCC = (Cfg.GetCommonCuts())->begin() ; itCC != (Cfg.GetCommonCuts())->end() ; ++itCC ) {
             if ( !passTCC ) continue; 
             if ( itCC->Result() ) {
               if ( itCC->NickName == itOTT->CutName ) {
                 if   ( ! itOTT->Split ) (itOTT->OutTOne)->Fill();               
                 else {
                   if ( gRandom->Rndm() < itOTT->SplitFrac ) (itOTT->OutTOne)->Fill(); 
                   else                                      (itOTT->OutTTwo)->Fill();    
                 }
               } 
             } else {
               passTCC = false ; 
             } 
           }

         }
       } 
     }

    //tmpTree->Fill();

//    if ( bWTree ) FillOutTree ( Cfg , itD->NickName ) ;

   }

   if ( bWTree ) {
     for ( vector<OutTTree_t>::iterator itOTT = (Cfg.GetOutTTree())->begin() ; itOTT != (Cfg.GetOutTTree())->end() ; ++itOTT ) {
       if ( itD->NickName == itOTT->DataName ) {
         (itOTT->OutTOne)->AutoSave();
         delete (itOTT->OutTOne);
         (itOTT->OutFOne)->Close();
         delete  (itOTT->OutFOne); 
         if ( itOTT->Split ) {
           (itOTT->OutTTwo)->AutoSave();
           delete (itOTT->OutTTwo);
           (itOTT->OutFTwo)->Close();
           delete  (itOTT->OutFTwo); 
         }
       }
     }
   } 

   //tmpTree->AutoSave();
   //delete tmpTree;
   //tmpFile->Close();
   //delete tmpFile;

   // Delete TreeWeight Formula
   (Cfg.GetTreeWeight())->DelFormula();  
   // Delete DataSetWght Formula
   for ( vector<DataSetWght_t>::iterator itDSW = (Cfg.GetDataSetWghts())->begin() ; itDSW != (Cfg.GetDataSetWghts())->end() ; ++itDSW ) itDSW->DelFormula();
   // Delete CommonCuts Formula   
   for ( vector<TreeFormula_t>::iterator iCC = (Cfg.GetCommonCuts())->begin() ; iCC != (Cfg.GetCommonCuts())->end() ; ++iCC) iCC->DelFormula();
   // Delete ScanCuts Formula
   for ( vector<ScanCut_t>::iterator iSCG = (Cfg.GetScanCuts())->begin() ; iSCG != (Cfg.GetScanCuts())->end() ; ++iSCG) {
     for  ( vector<TreeFormula_t>::iterator iSC = (iSCG->Cuts).begin() ; iSC != (iSCG->Cuts).end() ; ++iSC ) iSC->DelFormula();
   }
   // Delete CtrlPlots Formula
   for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP) itCP->DelFormula();
  
   // ------------- ExtEffTH2 ------------------
   for ( vector<ExtEffTH2_t>::iterator itEff = (Cfg.GetExtEffTH2())->begin() ; itEff != (Cfg.GetExtEffTH2())->end() ; ++itEff ) itEff->MakExtEffTH2(Tree);  

   // Close Tree
   delete Tree ;
   File->Close();
   delete File;
 }

}

//----------------------------------- FillCutFlow() ----------------------------------

void UATAnaReader::FillCutFlow ( UATAnaConfig& Cfg , string& NickName , int& iCC , Double_t& weight ) {
  // Individuals
  int iCCHist = 0 ; 
  for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iCCHist ) {
    if ( itD->NickName == NickName ) (CCflow.at(iCCHist))->Fill(iCC,weight); 
  }
  // DataGroups
  iCCHist = 0 ; 
  for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
    for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG , ++iCCHist ) {
      for ( vector<string>::iterator itDN = (itBG->Members).begin() ; itDN != (itBG->Members).end() ; ++itDN ) {
        if ( *itDN == NickName ) (CCflowGroup.at(iCCHist))->Fill(iCC,weight); 
      }
    }
  }
}

//----------------------------------- FillCutFlow() ----------------------------------

void UATAnaReader::FillScanFlow ( UATAnaConfig& Cfg , string& ScanName , string& NickName , int& iCC , Double_t& weight ) {
  // Individuals
  for ( vector<ScanFlow_t>::iterator iSF = SCflow.begin() ; iSF != SCflow.end() ; ++iSF ) { 
    if ( iSF->ScanName == ScanName ) {
      int iCCHist = 0 ;
      for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iCCHist ) {
        if ( itD->NickName == NickName ) ((iSF->CutFlow).at(iCCHist))->Fill(iCC,weight); 
      }
    }
  }
  // DataGroups
  for ( vector<ScanFlow_t>::iterator iSF = SCflowGroup.begin() ; iSF != SCflowGroup.end() ; ++iSF ) { 
    if ( iSF->ScanName == ScanName ) {
      int iCCHist = 0 ;
      for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
        for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG , ++iCCHist ) {
          for ( vector<string>::iterator itDN = (itBG->Members).begin() ; itDN != (itBG->Members).end() ; ++itDN ) {
            if ( *itDN == NickName ) ((iSF->CutFlow).at(iCCHist))->Fill(iCC,weight); 
          }
        }
      }     
    }
  }
}

//----------------------------------- FillPlotCC() ------------------------------------

void UATAnaReader::FillPlotCC ( UATAnaConfig& Cfg , string& NickName , Double_t& weight ) {

  int iH  = 0 ;
  int iHG = 0 ;

  for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
    Float_t Val = itCP->Result() ;
    for ( vector<string>::iterator itCC = (itCP->CCNickName).begin() ; itCC != (itCP->CCNickName).end() ; ++itCC ) {
      bool    Cut     = false      ;
      bool    passCC  = true       ;
      for ( vector<TreeFormula_t>::iterator itCut = (Cfg.GetCommonCuts())->begin() ; itCut != (Cfg.GetCommonCuts())->end() ; ++itCut ) {
        if ( !passCC ) continue; 
        if ( itCut->Result() ) {
          if ( itCut->NickName == (*itCC) ) Cut = true ;
        } else {
          passCC = false ; 
        } 
      }   
      // InputData
      for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH ) {
         if ( itD->NickName == NickName && Cut ) (PlotCC.at(iH))->Fill( Val , weight );
      }
      // DataGroups
      for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
        for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG , ++iHG ) {
          for ( vector<string>::iterator itDN = (itBG->Members).begin() ; itDN != (itBG->Members).end() ; ++itDN ) {
            if ( *itDN == NickName && Cut ) (PlotCCGroup.at(iHG))->Fill( Val , weight );
          }
        }
      }
    }
  }
  return; 
}


//----------------------------------- FillPlotSC() ------------------------------------

void UATAnaReader::FillPlotSC ( UATAnaConfig& Cfg , string& NickName , Double_t& weight  ) {

  int iH  = 0 ;
  int iHG = 0 ;

  for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
    Float_t Val = itCP->Result() ;
    for ( vector<string>::iterator itSC = (itCP->SCNickName).begin() ; itSC != (itCP->SCNickName).end() ; ++itSC ) {
      for ( vector<ScanCut_t>::iterator itSCG = (Cfg.GetScanCuts())->begin() ; itSCG != (Cfg.GetScanCuts())->end() ; ++itSCG) { 
        bool    Cut     = false      ;
        bool    passSC  = true       ;
        for ( vector<TreeFormula_t>::iterator itCut =  (itSCG->Cuts).begin() ; itCut != (itSCG->Cuts).end() ; ++itCut ) {
          if ( !passSC ) continue; 
          if ( itCut->Result() ) {
             if ( itCut->NickName == (*itSC) ) Cut = true ;
          } else {
            passSC = false ; 
          } 
        } 
        // InputData
        for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH) {
           if (itD->NickName == NickName && Cut ) (PlotSC.at(iH))->Fill( Val , weight );
        }
        // DataGroups
        for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
          for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG , ++iHG ) {
            for ( vector<string>::iterator itDN = (itBG->Members).begin() ; itDN != (itBG->Members).end() ; ++itDN ) {
              if ( *itDN == NickName && Cut ) (PlotSCGroup.at(iHG))->Fill( Val , weight );
            }
          }
        }
      } 
    }
  }

  return;
}


//----------------------------------- End() ------------------------------------------

void UATAnaReader::End  ( UATAnaConfig& Cfg , bool& bWTree ) {

  string fileName = Cfg.GetOutDir() + "/" + Cfg.GetTAnaName()+".root";
  File = new TFile(fileName.c_str(),"RECREATE");  
  
  // CommonCuts cutflow histograms
  File->cd();  
  gDirectory->mkdir("CutFlow");
  gDirectory->cd   ("CutFlow");
  gDirectory->mkdir("CommonCuts");
  gDirectory->cd   ("CommonCuts");
  gDirectory->mkdir("InputData");
  gDirectory->cd   ("InputData");
  for ( vector<TH1F*>::iterator itH = CCflow.begin() ; itH != CCflow.end() ; ++itH ) (*itH)->Write();
  gDirectory->cd("../");
  gDirectory->mkdir("DataGroups");
  gDirectory->cd   ("DataGroups");
  for ( vector<TH1F*>::iterator itH = CCflowGroup.begin() ; itH != CCflowGroup.end() ; ++itH ) (*itH)->Write();
  gDirectory->cd("../../");
  
  // ScanCuts cutflow histograms
  gDirectory->mkdir("ScanCuts");
  gDirectory->cd   ("ScanCuts");
  gDirectory->mkdir("InputData");
  gDirectory->cd   ("InputData");
  for ( vector<ScanFlow_t>::iterator iSF = SCflow.begin() ; iSF != SCflow.end() ; ++iSF ) {   
    gDirectory->mkdir((iSF->ScanName).c_str());
    gDirectory->cd   ((iSF->ScanName).c_str());
    for ( vector<TH1F*>::iterator itH = (iSF->CutFlow).begin() ; itH !=  (iSF->CutFlow).end() ; ++itH ) (*itH)->Write();
    gDirectory->cd("../");
  }
  gDirectory->cd("../");
  gDirectory->mkdir("DataGroups");
  gDirectory->cd   ("DataGroups");
  for ( vector<ScanFlow_t>::iterator iSF = SCflowGroup.begin() ; iSF != SCflowGroup.end() ; ++iSF ) {   
    gDirectory->mkdir((iSF->ScanName).c_str());
    gDirectory->cd   ((iSF->ScanName).c_str());
    for ( vector<TH1F*>::iterator itH = (iSF->CutFlow).begin() ; itH !=  (iSF->CutFlow).end() ; ++itH ) (*itH)->Write();
    gDirectory->cd("../");
  }
  
  // CtrlPlots CommonCuts
  File->cd();  
  gDirectory->mkdir("CtrlPlot");
  gDirectory->cd   ("CtrlPlot");
  gDirectory->mkdir("CommonCuts");
  gDirectory->cd   ("CommonCuts");

  // ... InputData
  gDirectory->mkdir("InputData");
  gDirectory->cd   ("InputData");

  int iH = 0 ;
  for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
    for ( vector<string>::iterator itCC = (itCP->CCNickName).begin() ; itCC != (itCP->CCNickName).end() ; ++itCC ) {
      gDirectory->mkdir((*itCC).c_str());
      gDirectory->cd   ((*itCC).c_str());
      for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH ) (PlotCC.at(iH))->Write();
      gDirectory->cd("../");
    }
  }
 
  // ... DataGroups
  gDirectory->cd("../");
  gDirectory->mkdir("DataGroups");
  gDirectory->cd   ("DataGroups");

  int iHG = 0 ;
  for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
    for ( vector<string>::iterator itCC = (itCP->CCNickName).begin() ; itCC != (itCP->CCNickName).end() ; ++itCC ) {
      gDirectory->mkdir((*itCC).c_str());
      gDirectory->cd   ((*itCC).c_str());
      for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
        for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG , ++iHG ) (PlotCCGroup.at(iHG))->Write();
      }
      gDirectory->cd("../");
    }
  } 

  // CtrlPlots CommonCuts
  File->cd();  
  gDirectory->mkdir("CtrlPlot");
  gDirectory->cd   ("CtrlPlot");
  gDirectory->mkdir("ScanCuts");
  gDirectory->cd   ("ScanCuts");

  // ... InputData
  gDirectory->mkdir("InputData");
  gDirectory->cd   ("InputData");
    
  iH = 0 ;
  for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
    for ( vector<string>::iterator itSC = (itCP->SCNickName).begin() ; itSC != (itCP->SCNickName).end() ; ++itSC ) {
      for ( vector<ScanCut_t>::iterator iSCG = (Cfg.GetScanCuts())->begin() ; iSCG != (Cfg.GetScanCuts())->end() ; ++iSCG) { 
        gDirectory->mkdir((iSCG->ScanName).c_str());
        gDirectory->cd   ((iSCG->ScanName).c_str());
        gDirectory->mkdir((*itSC).c_str());
        gDirectory->cd   ((*itSC).c_str());
        for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH) (PlotSC.at(iH))->Write();
        gDirectory->cd("../../"); 
      } 
    }
  }

  // ... DataGroups
  gDirectory->cd("../");
  gDirectory->mkdir("DataGroups");
  gDirectory->cd   ("DataGroups");

  iHG = 0 ;
  for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
    for ( vector<string>::iterator itSC = (itCP->SCNickName).begin() ; itSC != (itCP->SCNickName).end() ; ++itSC ) {
      for ( vector<ScanCut_t>::iterator iSCG = (Cfg.GetScanCuts())->begin() ; iSCG != (Cfg.GetScanCuts())->end() ; ++iSCG) { 
        gDirectory->mkdir((iSCG->ScanName).c_str());
        gDirectory->cd   ((iSCG->ScanName).c_str());
        gDirectory->mkdir((*itSC).c_str());
        gDirectory->cd   ((*itSC).c_str());
        for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
          for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG , ++iHG ) (PlotSCGroup.at(iHG))->Write();
        }
        gDirectory->cd("../../"); 
      } 
    }
  }


  File->Close();
  delete File;

  //if ( bWTree ) WritOutTree ( Cfg ) ;

}

/*

//----------------------------------- InitOutTree() ------------------------------------------

void UATAnaReader::InitOutTree ( UATAnaConfig& Cfg ) {

    cout << "[UATAnaReader::Init] Creating output TTree" << endl;
    for ( vector<OutTTree_t>::iterator itOTT = (Cfg.GetOutTTree())->begin() ; itOTT != (Cfg.GetOutTTree())->end() ; ++itOTT ) {
      if ( ! itOTT->DataMode ) {
        cout << "... " << itOTT->DataName << endl ;
        for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD) {
          if ( itD->NickName == itOTT->DataName ) {
            File = new TFile((itD->FileName).c_str(),"READ");  
            Tree = (TTree*) File->Get((itD->TreeName).c_str());
            Tree->SetBranchStatus("*",1);
            gROOT->cd();
            itOTT->OutTOne = Tree->CloneTree(0);
            (itOTT->OutTOne)->SetName((itOTT->TreeName).c_str());
            if ( itOTT->Split ) {
              itOTT->OutTTwo = Tree->CloneTree(0);
              (itOTT->OutTTwo)->SetName((itOTT->TreeName).c_str());
            }
            delete Tree  ;
            File->Close();
            delete File  ;   
          }
        } 
      } else {
        cout << "... " << itOTT->DataGroup << " : " << itOTT->DataName << endl ;
        for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
          for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG  ) {
            if ( itBG->BaseName  == itOTT->DataName ) {
              for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD) {
                if ( itD->NickName == (itBG->Members).at(0) ) {
                   File = new TFile((itD->FileName).c_str(),"READ");  
                   Tree = (TTree*) File->Get((itD->TreeName).c_str());
                   Tree->SetBranchStatus("*",1);
                   gROOT->cd();
                   itOTT->OutTOne = Tree->CloneTree(0);
                   (itOTT->OutTOne)->SetName((itOTT->TreeName).c_str());
                   if ( itOTT->Split ) {
                     itOTT->OutTTwo = Tree->CloneTree(0);
                     (itOTT->OutTTwo)->SetName((itOTT->TreeName).c_str());
                   }
                   delete Tree  ;
                   File->Close();
                   delete File  ;   
                }  
              }
            }
          }
        }
      }
    }

  return ;

}



//----------------------------------- FillOutTree() ------------------------------------------

void UATAnaReader::FillOutTree ( UATAnaConfig& Cfg , string& NickName ) {

  for ( vector<OutTTree_t>::iterator itOTT = (Cfg.GetOutTTree())->begin() ; itOTT != (Cfg.GetOutTTree())->end() ; ++itOTT ) {
    if ( ! itOTT->DataMode ) {
      if ( NickName == itOTT->DataName ) {
        (itOTT->OutTOne)->Fill(); 
      }
    }
  }   

  return;
}

//----------------------------------- WritOutTree() ------------------------------------------

void UATAnaReader::WritOutTree ( UATAnaConfig& Cfg ) {
  
  for ( vector<OutTTree_t>::iterator itOTT = (Cfg.GetOutTTree())->begin() ; itOTT != (Cfg.GetOutTTree())->end() ; ++itOTT ) {

    string fileName = Cfg.GetTAnaName() + "__" + itOTT->TreeName + "__" ;
    if ( itOTT->DataMode) fileName += itOTT->DataGroup + "__" ;  
                          fileName += itOTT->DataName  + "__" ;
                          fileName += itOTT->CutName   ;
                          fileName += ".root" ;
    if (itOTT->OutTOne) {
      TFile* OTTFile = new TFile(fileName.c_str(),"RECREATE");
      OTTFile->cd();  
      (itOTT->OutTOne)->Write();
      OTTFile->Close();
      delete OTTFile;
      delete (itOTT->OutTOne);
    } else {
      cout << "[UATAnaReader::WritOutTree] Missing TTree : " << fileName << endl;
    }
  }

  return;
}

*/
