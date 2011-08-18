
#include "src/UATAnaConfig.cc"
#include "src/UATAnaReader.cc"
#include "src/UATAnaDisplay.cc"

/*
   R = Read Tree and Fill all histos
   F = Plot CutFlow Histograms
   Y = Print Yields
   C = Plot Control Plots
*/

void UATAna(TString Cfg = "Config.cfg" , TString Steps = "R"){

  Steps.ToUpper();

  UATAnaConfig Config;
  Config.ReadCfg(Cfg);
//  Config.Print();

  if ( Steps.Contains ('R') ) {
    UATAnaReader Reader;
    Reader.Init   (Config);
    Reader.Analyze(Config);
    Reader.End    (Config);
  }
  
  if (    Steps.Contains ('F')
       || Steps.Contains ('Y')
       || Steps.Contains ('C') ) {

    UATAnaDisplay Display;
    Display.Init   (Config);

    if ( Steps.Contains ('F') ) Display.Yields(Config,1);
    if ( Steps.Contains ('Y') ) Display.Yields(Config,0);
    if ( Steps.Contains ('C') ) Display.CPlot (Config);

  }

} 

