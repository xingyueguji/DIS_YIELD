class DIS_YIELD{
	public:

	DIS_YIELD(Int_t a);
	//~DIS_YIELD();

	void GetROOTfiles(); //open root file, get T Tree, set branch.
	void ReadRunList(TString opt);
	void ReadReport(TString opt);
	void SetupDir(Int_t runnum, Int_t numofevent); // Set ROOfiles and ReportOutput
	void CloseROOTfiles();
	void GetElrealEff(Double_t& x1,Double_t& x2);
	void SetCut(bool& c_edtm,bool& c_npe,bool& c_current,bool& c_elreal_tdc,bool& c_elreal_tdc_mul,bool& c_etottrknorm, bool& c_hdelta);
	void GetYield(Double_t& trk_Yield_bcm4a, Double_t& trk_err_bcm4a, Double_t triggereff);


    Int_t NumofRuns;
	vector <int> RunList;
	TString RunListPathps3 = "runlistps3.dat";
	TString RunListPathps6 = "runlistps6.dat";
	TString ROOTfilesPattern = "/net/cdaq/cdaql2data/cdaq/hallc-online-nps2023/ROOTfiles/COIN/PRODUCTION/nps_hms_coin_%d_50k_events.root";
	TString ReportOutputPattern = "/net/cdaq/cdaql1data/cdaq/hallc-online-nps2023/REPORT_OUTPUT/COIN/coin_NPS_HMS_report_%d_10000.report";
	TString ROOTfiles;
	TString ReportOutput;

	TFile* f1;
	TTree* t1;

	Double_t Charge;
	Double_t current4A;
	Double_t etottracknorm;
	Double_t elrealtdcmulti;
	Double_t edtmtdctime;
	Double_t npesum;
	Double_t elrealtdctime;
	Double_t hDelta;
	Double_t hTRIG6_scaler_4a;
	Double_t hEDTM_scaler_4a;
	Double_t hTRIG3_scaler_4a;
	Double_t hGoodScinHit;
	Double_t hBetanotrk;
	Double_t hCAL_etotnorm; 
	Double_t hdc_ntrack;

	Int_t pS;



};