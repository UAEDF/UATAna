
#include "src/UATAnaConfig.cc"
#include "src/UATAnaReader.cc"
#include "src/UATAnaDisplay.cc"

/*
   R = Read Tree and Fill all histos
   T = As 'R' but also write preselected tree 
   F = Plot CutFlow Histograms
   Y = Print Yields
   C = Plot Control Plots
   L = Prepare Limit Input Cards
*/

void UATAna(TString Cfg = "Config.cfg" , TString Steps = "R"){

  Steps.ToUpper();

  UATAnaConfig Config;
  Config.ReadCfg(Cfg);
  Config.Print();

  if (    Steps.Contains ('R') 
       || Steps.Contains ('T') ) {
    bool bWTree = false ;
    if ( Steps.Contains ('T') ) bWTree = true ;
    UATAnaReader Reader;
    Reader.Init   (Config , bWTree );
    Reader.Analyze(Config , bWTree );
    Reader.End    (Config , bWTree );
  }
  
  if (    Steps.Contains ('F')
       || Steps.Contains ('Y')
       || Steps.Contains ('L')
       || Steps.Contains ('C') ) {

    UATAnaDisplay Display;
    Display.Init   (Config);

    if ( Steps.Contains ('F') ) Display.Yields(Config,1);
    if ( Steps.Contains ('Y') ) Display.Yields(Config,0);
    if ( Steps.Contains ('C') ) { 
      if ( Steps.Contains ('S') ) Display.CPlot (Config,1);
      else                        Display.CPlot (Config,0);
    }
    if ( Steps.Contains ('L') ) Display.LimitCard (Config);

  }

} 

