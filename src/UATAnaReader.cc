
#include "../includes/UATAnaReader.h"
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>

bool direxists( string dirName) {
	TIter keys(gDirectory->GetListOfKeys());
	bool exists = false;
	while (TObject *obj = keys()) {
		if(obj->GetName() ==dirName) {
			exists = true;
			break;
		}
	}
	return exists;
}

//----------------------------------- Init() ------------------------------------------

void UATAnaReader::Init ( UATAnaConfig& Cfg , bool& bWTree ) {

  TH1::SetDefaultSumw2(1);

  // Output issues check
  if (bWTree) {
	  struct stat st;
	  if ( ! (stat((Cfg.GetOutDir()).c_str(), &st) == 0) ) {
		cout << "\E[0;31mDirectory " << Cfg.GetOutDir() << " doesn't exist.\E[m";
		mkdir((Cfg.GetOutDir()).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		cout << "\E[0;32m --> Directory " << Cfg.GetOutDir() << " created.\E[m" << endl;
	  }
	  else if ( stat((Cfg.GetOutDir()).c_str(), &st) == 0 && ! S_ISDIR(st.st_mode)) {
		cout << "\E[0;31m" << Cfg.GetOutDir() << " is not a directory. Stopping.\E[m" << endl;
		exit(0);
	  }
	  else {
	    cout << "\E[0;34mOutput in " << Cfg.GetOutDir() << ".\E[m" << endl;
	  }
  }
  
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

  // if (!direxists((*itSC).c_str())) CtrlPlots histograms 
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


  // Load External Functions
  for ( vector<string>::iterator itFunc = (Cfg.GetFunc2Load())->begin() ; itFunc !=  (Cfg.GetFunc2Load())->end() ; ++ itFunc ) {
     cout << "Loading External Function : " << *itFunc << endl;
     TString Command = ".L "+(*itFunc)+"++";
     gROOT->ProcessLineSync(Command);
  }

  // Output TTree
  // if ( bWTree ) InitOutTree (Cfg);
  

}

//----------------------------------- Analyze() ----------------------------------------

void UATAnaReader::Analyze( UATAnaConfig& Cfg , bool& bWTree ) {

 // Some PrintEvt output
 // for ( vector<PrintEvt_t>::iterator itPE = (Cfg.GetPrintEvt())->begin() ; itPE !=  (Cfg.GetPrintEvt())->end() ; ++itPE) {


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
         if ( (itOTT->BranchList).size() > 0 ) {
           Tree->SetBranchStatus("*",0);
           for ( vector<string>::iterator itBranch = (itOTT->BranchList).begin() ; itBranch != (itOTT->BranchList).end() ; ++itBranch ) {
             Tree->SetBranchStatus("*",TString(*itBranch)); 
           }
         }
         (itOTT->OutFOne) = new TFile((itOTT->OutNOne).c_str(),"RECREATE");     
         (itOTT->OutTOne) = Tree->CloneTree(0);
         (itOTT->OutTOne)->SetName((itOTT->TreeName).c_str());
         if ( itOTT->Split ) {
           (itOTT->OutFTwo) = new TFile((itOTT->OutNTwo).c_str(),"RECREATE");     
           (itOTT->OutTTwo) = Tree->CloneTree(0);
           (itOTT->OutTTwo)->SetName((itOTT->TreeName).c_str());
         }
         Tree->SetBranchStatus("*",1);
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
   // Create ScaleFactor Formula
   for ( vector<ScaleFactor_t>::iterator itSCF = (Cfg.GetScaleFactors())->begin() ; itSCF != (Cfg.GetScaleFactors())->end() ; ++itSCF ) itSCF->MakFormula(Tree);
   // Create CommonCuts Formula   
   for ( vector<CommonCut_t>::iterator itCC = (Cfg.GetCommonCuts())->begin() ; itCC != (Cfg.GetCommonCuts())->end() ; ++itCC) itCC->MakFormula(Tree);
   // Create ScanCuts Formula
   for ( vector<ScanCut_t>::iterator itSCG = (Cfg.GetScanCuts())->begin() ; itSCG != (Cfg.GetScanCuts())->end() ; ++itSCG) {
     for  ( vector<TreeFormula_t>::iterator itSC = (itSCG->Cuts).begin() ; itSC != (itSCG->Cuts).end() ; ++itSC ) itSC->MakFormula(Tree);
   }
   // Create CtrlPlots Formula
   for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP) itCP->MakFormula(Tree);
   // Create PrintEvt Formula
   /*TreeFormula_t RunForm;
   RunForm.NickName   = "Run" ;
   RunForm.Expression = "run" ;
   RunForm.MakFormula(Tree);
   TreeFormula_t EvtForm;
   EvtForm.NickName   = "Evt" ;
   EvtForm.Expression = "event" ;
   EvtForm.MakFormula(Tree);*/
   for ( vector<PrintEvt_t>::iterator itPE = (Cfg.GetPrintEvt())->begin() ; itPE !=  (Cfg.GetPrintEvt())->end() ; ++itPE) {
     if ( itPE->Type == 0 ) itPE->MakFormula(Tree);
     for  ( vector<TreeFormula_t>::iterator itVL = (itPE->VarList).begin() ; itVL != (itPE->VarList).end() ; ++itVL ) itVL->MakFormula(Tree);
   }  



   // ------------- ExtEff ------------------
   
   for ( vector<ExtEff_t>::iterator itEff = (Cfg.GetExtEff())->begin() ; itEff != (Cfg.GetExtEff())->end() ; ++itEff ) itEff->MakExtEff(Tree);  
   for ( vector<ExtEffTH2_t>::iterator itEff = (Cfg.GetExtEffTH2())->begin() ; itEff != (Cfg.GetExtEffTH2())->end() ; ++itEff ) itEff->MakExtEffTH2(Tree);  

   // ------------- External Functions Init ----

   for ( vector<Func2Init_t>::iterator itFunc = (Cfg.GetFunc2Init())->begin() ; itFunc !=  (Cfg.GetFunc2Init())->end() ; ++ itFunc ) {
     if ( itD->NickName == itFunc->DataName ||  itFunc->DataName == "*" ) {
       cout << "Init External Function : " << itFunc->Command << endl;
       gROOT->ProcessLineSync(TString(itFunc->Command));
     }
  }



   // ------------- Loop -----------------------

   Int_t nEntries = Tree->GetEntries();
   for (Int_t jEntry = 0 ;  jEntry < nEntries ; ++jEntry) { 

     Tree->GetEntry(jEntry);

     // Evaluate TreeWeight Formula
     (Cfg.GetTreeWeight())->EvaFormula();  
     // Evaluate DataSetWght Formula
     //for ( vector<DataSetWght_t>::iterator itDSW = (Cfg.GetDataSetWghts())->begin() ; itDSW != (Cfg.GetDataSetWghts())->end() ; ++itDSW ) itDSW->EvaFormula();
     // Evaluate ScaleFactor Formula
     for ( vector<ScaleFactor_t>::iterator itSCF = (Cfg.GetScaleFactors())->begin() ; itSCF != (Cfg.GetScaleFactors())->end() ; ++itSCF ) itSCF->EvaFormula();
     // Evaluate CommonCuts Formula   
     for ( vector<CommonCut_t>::iterator itCC = (Cfg.GetCommonCuts())->begin() ; itCC != (Cfg.GetCommonCuts())->end() ; ++itCC) itCC->EvaFormula();
     // Evaluate ScanCuts Formula
     for ( vector<ScanCut_t>::iterator itSCG = (Cfg.GetScanCuts())->begin() ; itSCG != (Cfg.GetScanCuts())->end() ; ++itSCG) {
       for  ( vector<TreeFormula_t>::iterator itSC = (itSCG->Cuts).begin() ; itSC != (itSCG->Cuts).end() ; ++itSC ) itSC->EvaFormula();
     }
     // Evaluate CtrlPlots Formula
     for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP) itCP->EvaFormula();
     // Evaluate PrintEvt Formula
     //RunForm.EvaFormula();
     //EvtForm.EvaFormula();
     for ( vector<PrintEvt_t>::iterator itPE = (Cfg.GetPrintEvt())->begin() ; itPE !=  (Cfg.GetPrintEvt())->end() ; ++itPE) {
       if ( itPE->Type == 0 ) itPE->EvaFormula();
       for  ( vector<TreeFormula_t>::iterator itVL = (itPE->VarList).begin() ; itVL != (itPE->VarList).end() ; ++itVL ) itVL->EvaFormula();
     } 
     // Evaluate ExtEff 
     for ( vector<ExtEff_t>::iterator itEff = (Cfg.GetExtEff())->begin() ; itEff != (Cfg.GetExtEff())->end() ; ++itEff ) itEff->EvaExtEff();  
     for ( vector<ExtEffTH2_t>::iterator itEff = (Cfg.GetExtEffTH2())->begin() ; itEff != (Cfg.GetExtEffTH2())->end() ; ++itEff ) itEff->EvaExtEffTH2();  

     // ------------- DataSetWeight --------------
     Double_t DataSetWeight = 1.0 ;
     for ( vector<DataSetWght_t>::iterator itDSW = (Cfg.GetDataSetWghts())->begin() ; itDSW != (Cfg.GetDataSetWghts())->end() ; ++itDSW ) { 
       for ( vector<string>::iterator itDSN = (itDSW->DataSets).begin() ; itDSN != (itDSW->DataSets).end() ; ++itDSN ) {
         if ( (*itDSN) == itD->NickName ) {
           itDSW->EvaFormula();
           DataSetWeight *= itDSW->Result() ;
         }
       }
     }
     // Compute Event Weight 
     Double_t Weight = DataSetWeight * (Cfg.GetTreeWeight())->Result() ;
     if ( ! itD->Data )  Weight *= Cfg.GetTargetLumi() / itD->Lumi ;
     for ( vector<ExtEff_t>::iterator itEff = (Cfg.GetExtEff())->begin() ; itEff != (Cfg.GetExtEff())->end() ; ++itEff )  Weight *= itEff->Result(itD->NickName) ;
     for ( vector<ExtEffTH2_t>::iterator itEff = (Cfg.GetExtEffTH2())->begin() ; itEff != (Cfg.GetExtEffTH2())->end() ; ++itEff )  Weight *= itEff->Result(itD->NickName) ;
     if ( Weight == 0 ) continue; 

     // CommonCuts
     bool passCC = true ;
     int  iCC = 0 ;
     for ( vector<CommonCut_t>::iterator itCC = (Cfg.GetCommonCuts())->begin() ; itCC != (Cfg.GetCommonCuts())->end() ; ++itCC , ++iCC ) {
       if ( !passCC ) continue; 
       if ( itCC->Result() ) {
         Double_t SFWeight = Weight ;
         for ( vector<ScaleFactor_t>::iterator itSCF = (Cfg.GetScaleFactors())->begin() ; itSCF != (Cfg.GetScaleFactors())->end() ; ++itSCF ) {
           if ( itSCF->isCC ) {  
             for ( vector<string>::iterator itDSN = (itSCF->DataSets).begin() ; itDSN != (itSCF->DataSets).end() ; ++itDSN ) {
               if ( (*itDSN) == itD->NickName ) SFWeight*= itSCF->Result();
             }
           }
         }
         FillCutFlow(Cfg,itD->NickName,iCC,SFWeight);
         // PrintEvt
         for ( vector<PrintEvt_t>::iterator itPE = (Cfg.GetPrintEvt())->begin() ; itPE !=  (Cfg.GetPrintEvt())->end() ; ++itPE) {
           if ( itPE->Type == 1 ) {
             for ( vector<string>::iterator itPEC = (itPE->CCNickName).begin() ; itPEC !=  (itPE->CCNickName).end() ; ++itPEC ) {
               if ( *itPEC == itCC->NickName ) { 
                 cout << itPE->NickName << ": sample=" << itD->NickName << " " ;
                 for (vector<TreeFormula_t>::iterator itVL = (itPE->VarList).begin() ; itVL != (itPE->VarList).end() ; ++itVL ) cout << setiosflags(ios::fixed) << setprecision(6) << itVL->NickName << "=" << itVL->Result() << " " ;
                 cout << endl;
               }
             }  
           } 
         }
       } else {
         passCC = false ; 
       } 
     }

     // ScanCuts
     if (passCC) {
       for ( vector<ScanCut_t>::iterator itSCG = (Cfg.GetScanCuts())->begin() ; itSCG != (Cfg.GetScanCuts())->end() ; ++itSCG) {
         Double_t SFWeight = Weight ; 
         for ( vector<ScaleFactor_t>::iterator itSCF = (Cfg.GetScaleFactors())->begin() ; itSCF != (Cfg.GetScaleFactors())->end() ; ++itSCF ) {
           for ( vector<string>::iterator itSCN = (itSCF->ScanName).begin() ; itSCN != (itSCF->ScanName).end() ; ++itSCN ) {
             if ( (*itSCN) == itSCG->ScanName ) {
               for ( vector<string>::iterator itDSN = (itSCF->DataSets).begin() ; itDSN != (itSCF->DataSets).end() ; ++itDSN ) {
                 if ( (*itDSN) == itD->NickName ) SFWeight*= itSCF->Result();
               }
             }
           }
         }
         bool passSC = true ; 
         int  iSC = 0 ;
         for ( vector<TreeFormula_t>::iterator itSC =  (itSCG->Cuts).begin() ; itSC != (itSCG->Cuts).end() ; ++itSC , ++iSC ) {
           if ( !passSC ) continue; 
           if ( itSC->Result() ) {
             FillScanFlow(Cfg,itSCG->ScanName,itD->NickName,iSC,SFWeight);
             // PrintEvt
             for ( vector<PrintEvt_t>::iterator itPE = (Cfg.GetPrintEvt())->begin() ; itPE !=  (Cfg.GetPrintEvt())->end() ; ++itPE) {
               if ( itPE->Type == 2 ) {
                 for ( vector<string>::iterator itPEC = (itPE->CCNickName).begin() ; itPEC !=  (itPE->CCNickName).end() ; ++itPEC ) {
                   if ( *itPEC == itSC->NickName ) { 
                     cout << itPE->NickName << ": sample=" << itD->NickName << " " ;
                     for (vector<TreeFormula_t>::iterator itVL = (itPE->VarList).begin() ; itVL != (itPE->VarList).end() ; ++itVL ) cout << setiosflags(ios::fixed) << setprecision(6) << itVL->NickName << "=" << itVL->Result() << " " ;
                     cout << endl;
                   }
                 }  
               } 
             }
           } else {
             passSC = false ; 
           } 
         }  
       }
     } 



     // Control Plots
     FillPlotCC( Cfg , itD->NickName , Weight );
     if (passCC) FillPlotSC( Cfg , itD->NickName , Weight );

     // PrintEvt
     for ( vector<PrintEvt_t>::iterator itPE = (Cfg.GetPrintEvt())->begin() ; itPE !=  (Cfg.GetPrintEvt())->end() ; ++itPE) {
       if ( itPE->Type == 0 ) {
         if ( itPE->Result() ) {
           cout << itPE->NickName << ": sample=" << itD->NickName << " " ;
           for (vector<TreeFormula_t>::iterator itVL = (itPE->VarList).begin() ; itVL != (itPE->VarList).end() ; ++itVL ) cout << setiosflags(ios::fixed) << setprecision(6) << itVL->NickName << "=" << itVL->Result() << " " ;
           //for (vector<TreeFormula_t>::iterator itVL = (itPE->VarList).begin() ; itVL != (itPE->VarList).end() ; ++itVL ) cout << itVL->NickName << "=" << itVL->Result() << " " ;
           cout << endl;
         } 
       }
     }

     // Output TTree
     if ( bWTree && Weight != 0 ) {
       for ( vector<OutTTree_t>::iterator itOTT = (Cfg.GetOutTTree())->begin() ; itOTT != (Cfg.GetOutTTree())->end() ; ++itOTT ) {
         if ( itD->NickName == itOTT->DataName ) {
           bool passTCC = true ;
           for ( vector<CommonCut_t>::iterator itCC = (Cfg.GetCommonCuts())->begin() ; itCC != (Cfg.GetCommonCuts())->end() ; ++itCC ) {
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
   // Delete ScaleFactor Formula
   for ( vector<ScaleFactor_t>::iterator itSCF = (Cfg.GetScaleFactors())->begin() ; itSCF != (Cfg.GetScaleFactors())->end() ; ++itSCF ) itSCF->DelFormula();

   // Delete CommonCuts Formula   
   for ( vector<CommonCut_t>::iterator iCC = (Cfg.GetCommonCuts())->begin() ; iCC != (Cfg.GetCommonCuts())->end() ; ++iCC) iCC->DelFormula();
   // Delete ScanCuts Formula
   for ( vector<ScanCut_t>::iterator iSCG = (Cfg.GetScanCuts())->begin() ; iSCG != (Cfg.GetScanCuts())->end() ; ++iSCG) {
     for  ( vector<TreeFormula_t>::iterator iSC = (iSCG->Cuts).begin() ; iSC != (iSCG->Cuts).end() ; ++iSC ) iSC->DelFormula();
   }
   // Delete CtrlPlots Formula
   for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP) itCP->DelFormula();
   // Delete PrintEvtFormula
   //RunForm.DelFormula();
   //EvtForm.DelFormula();
   for ( vector<PrintEvt_t>::iterator itPE = (Cfg.GetPrintEvt())->begin() ; itPE !=  (Cfg.GetPrintEvt())->end() ; ++itPE) {
     if ( itPE->Type == 0 ) itPE->DelFormula();
     for  ( vector<TreeFormula_t>::iterator itVL = (itPE->VarList).begin() ; itVL != (itPE->VarList).end() ; ++itVL ) itVL->DelFormula();
   } 
  
   // ------------- ExtEff ------------------
   for ( vector<ExtEff_t>::iterator itEff = (Cfg.GetExtEff())->begin() ; itEff != (Cfg.GetExtEff())->end() ; ++itEff ) itEff->MakExtEff(Tree);  
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

  Double_t SFWeight = weight ;
  for ( vector<ScaleFactor_t>::iterator itSCF = (Cfg.GetScaleFactors())->begin() ; itSCF != (Cfg.GetScaleFactors())->end() ; ++itSCF ) {
    if ( itSCF->isCC ) {  
      for ( vector<string>::iterator itDSN = (itSCF->DataSets).begin() ; itDSN != (itSCF->DataSets).end() ; ++itDSN ) {
        if ( (*itDSN) == NickName ) SFWeight*= itSCF->Result();
      }
    }
  }


  for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
    Float_t Val = itCP->Result() ;
    for ( vector<string>::iterator itCC = (itCP->CCNickName).begin() ; itCC != (itCP->CCNickName).end() ; ++itCC ) {
      bool    Cut     = false      ;
      bool    passCC  = true       ;
      for ( vector<CommonCut_t>::iterator itCut = (Cfg.GetCommonCuts())->begin() ; itCut != (Cfg.GetCommonCuts())->end() ; ++itCut ) {
        if ( !passCC ) continue; 
        if ( itCut->Result() ) {
          if ( itCut->NickName == (*itCC) ) Cut = true ;
        } else {
          passCC = false ; 
        } 
      }   
      // InputData
      for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH ) {
         if ( itD->NickName == NickName && Cut ) (PlotCC.at(iH))->Fill( Val , SFWeight  );
      }
      // DataGroups
      for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
        for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG , ++iHG ) {
          for ( vector<string>::iterator itDN = (itBG->Members).begin() ; itDN != (itBG->Members).end() ; ++itDN ) {
            if ( *itDN == NickName && Cut ) (PlotCCGroup.at(iHG))->Fill( Val , SFWeight );
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

        Double_t SFWeight = weight ; 
        for ( vector<ScaleFactor_t>::iterator itSCF = (Cfg.GetScaleFactors())->begin() ; itSCF != (Cfg.GetScaleFactors())->end() ; ++itSCF ) {
          for ( vector<string>::iterator itSCN = (itSCF->ScanName).begin() ; itSCN != (itSCF->ScanName).end() ; ++itSCN ) {
            if ( (*itSCN) == itSCG->ScanName ) {
              for ( vector<string>::iterator itDSN = (itSCF->DataSets).begin() ; itDSN != (itSCF->DataSets).end() ; ++itDSN ) {
                if ( (*itDSN) == NickName ) SFWeight*= itSCF->Result();
              }
            }
          }
        }


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
           if (itD->NickName == NickName && Cut ) (PlotSC.at(iH))->Fill( Val , SFWeight );
        }
        // DataGroups
        for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
          for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG , ++iHG ) {
            for ( vector<string>::iterator itDN = (itBG->Members).begin() ; itDN != (itBG->Members).end() ; ++itDN ) {
              if ( *itDN == NickName && Cut ) (PlotSCGroup.at(iHG))->Fill( Val , SFWeight );
            }
          }
        }
      } 
    }
  }

  return;
}


//----------------------------------- End() ------------------------------------------

void UATAnaReader::End  ( UATAnaConfig& Cfg, bool& bWTree ) { 
  (void)bWTree; 

  string fileName = Cfg.GetOutDir() + "/" + Cfg.GetTAnaName()+".root";
  File = new TFile(fileName.c_str(),"RECREATE");  
  
  // CommonCuts cutflow histograms
  File->cd();  
  if (!direxists("CutFlow")) gDirectory->mkdir("CutFlow");
  gDirectory->cd   ("CutFlow");
  if (!direxists("CommonCuts")) gDirectory->mkdir("CommonCuts");
  gDirectory->cd   ("CommonCuts");
  if (!direxists("InputData")) gDirectory->mkdir("InputData");
  gDirectory->cd   ("InputData");
  for ( vector<TH1F*>::iterator itH = CCflow.begin() ; itH != CCflow.end() ; ++itH ) (*itH)->Write();
  gDirectory->cd("../");
  if (!direxists("DataGroups")) gDirectory->mkdir("DataGroups");
  gDirectory->cd   ("DataGroups");
  for ( vector<TH1F*>::iterator itH = CCflowGroup.begin() ; itH != CCflowGroup.end() ; ++itH ) (*itH)->Write();
  gDirectory->cd("../../");
  
  // ScanCuts cutflow histograms
  if (!direxists("ScanCuts")) gDirectory->mkdir("ScanCuts");
  gDirectory->cd   ("ScanCuts");
  if (!direxists("InputData")) gDirectory->mkdir("InputData");
  gDirectory->cd   ("InputData");
  for ( vector<ScanFlow_t>::iterator iSF = SCflow.begin() ; iSF != SCflow.end() ; ++iSF ) {  
	if (!direxists((iSF->ScanName).c_str())) gDirectory->mkdir((iSF->ScanName).c_str());
    gDirectory->cd   ((iSF->ScanName).c_str());
    for ( vector<TH1F*>::iterator itH = (iSF->CutFlow).begin() ; itH !=  (iSF->CutFlow).end() ; ++itH ) (*itH)->Write();
    gDirectory->cd("../");
  }
  gDirectory->cd("../");
  if (!direxists("DataGroups")) gDirectory->mkdir("DataGroups");
  gDirectory->cd   ("DataGroups");
  for ( vector<ScanFlow_t>::iterator iSF = SCflowGroup.begin() ; iSF != SCflowGroup.end() ; ++iSF ) {   
    if (!direxists((iSF->ScanName).c_str())) gDirectory->mkdir((iSF->ScanName).c_str());
    gDirectory->cd   ((iSF->ScanName).c_str());
    for ( vector<TH1F*>::iterator itH = (iSF->CutFlow).begin() ; itH !=  (iSF->CutFlow).end() ; ++itH ) (*itH)->Write();
    gDirectory->cd("../");
  }
  
  // CtrlPlots CommonCuts
  File->cd();  
  if (!direxists("CtrlPlot")) gDirectory->mkdir("CtrlPlot");
  gDirectory->cd   ("CtrlPlot");
  if (!direxists("")) gDirectory->mkdir("CommonCuts");
  gDirectory->cd   ("CommonCuts");

  // ... InputData
  if (!direxists("InputData")) gDirectory->mkdir("InputData");
  gDirectory->cd   ("InputData");

  int iH = 0 ;
  for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
    for ( vector<string>::iterator itCC = (itCP->CCNickName).begin() ; itCC != (itCP->CCNickName).end() ; ++itCC ) {
      if (!direxists((*itCC).c_str())) gDirectory->mkdir((*itCC).c_str());
      gDirectory->cd   ((*itCC).c_str());
      for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH ) (PlotCC.at(iH))->Write();
      gDirectory->cd("../");
    }
  }
 
  // ... DataGroups
  gDirectory->cd("../");
  if (!direxists("DataGroups")) gDirectory->mkdir("DataGroups");
  gDirectory->cd   ("DataGroups");

  int iHG = 0 ;
  for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
    for ( vector<string>::iterator itCC = (itCP->CCNickName).begin() ; itCC != (itCP->CCNickName).end() ; ++itCC ) {
      if (!direxists((*itCC).c_str())) gDirectory->mkdir((*itCC).c_str());
      gDirectory->cd   ((*itCC).c_str());
      for ( vector<DataGroup_t>::iterator itDG = (Cfg.GetDataGroups())->begin() ; itDG !=  (Cfg.GetDataGroups())->end() ; ++itDG ) {
        for ( vector<BaseGroup_t>::iterator itBG = (itDG->Members).begin() ; itBG != (itDG->Members).end() ; ++itBG , ++iHG ) (PlotCCGroup.at(iHG))->Write();
      }
      gDirectory->cd("../");
    }
  } 

  // CtrlPlots CommonCuts
  File->cd();  
  if (!direxists("CtrlPlot")) gDirectory->mkdir("CtrlPlot");
  gDirectory->cd   ("CtrlPlot");
  if (!direxists("ScanCuts")) gDirectory->mkdir("ScanCuts");
  gDirectory->cd   ("ScanCuts");

  // ... InputData
  if (!direxists("InputData")) gDirectory->mkdir("InputData");
  gDirectory->cd   ("InputData");
    
  iH = 0 ;
  for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
    for ( vector<string>::iterator itSC = (itCP->SCNickName).begin() ; itSC != (itCP->SCNickName).end() ; ++itSC ) {
      for ( vector<ScanCut_t>::iterator iSCG = (Cfg.GetScanCuts())->begin() ; iSCG != (Cfg.GetScanCuts())->end() ; ++iSCG) { 
        if (!direxists((iSCG->ScanName).c_str())) gDirectory->mkdir((iSCG->ScanName).c_str());
        gDirectory->cd   ((iSCG->ScanName).c_str());
        if (!direxists((*itSC).c_str())) gDirectory->mkdir((*itSC).c_str());
        gDirectory->cd   ((*itSC).c_str());
        for ( vector<InputData_t>::iterator itD = (Cfg.GetInputData())->begin() ; itD != (Cfg.GetInputData())->end() ; ++itD , ++iH) (PlotSC.at(iH))->Write();
        gDirectory->cd("../../"); 
      } 
    }
  }

  // ... DataGroups
  gDirectory->cd("../");
  if (!direxists("DataGroups")) gDirectory->mkdir("DataGroups");
  gDirectory->cd   ("DataGroups");

  iHG = 0 ;
  for ( vector<CtrlPlot_t>::iterator itCP = (Cfg.GetCtrlPlots())->begin() ; itCP != (Cfg.GetCtrlPlots())->end() ; ++itCP ) {
    for ( vector<string>::iterator itSC = (itCP->SCNickName).begin() ; itSC != (itCP->SCNickName).end() ; ++itSC ) {
      for ( vector<ScanCut_t>::iterator iSCG = (Cfg.GetScanCuts())->begin() ; iSCG != (Cfg.GetScanCuts())->end() ; ++iSCG) { 
        if (!direxists((iSCG->ScanName).c_str())) gDirectory->mkdir((iSCG->ScanName).c_str());
        gDirectory->cd   ((iSCG->ScanName).c_str());
        if (!direxists((*itSC).c_str())) gDirectory->mkdir((*itSC).c_str());
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
