#include<iostream>
#include"DIS_YIELD.h"

using namespace std;


DIS_YIELD::DIS_YIELD(Int_t a)
{
}
void DIS_YIELD::ReadRunList(TString opt){
	ifstream ifs;
	string line;
	int runnum;
	if (opt == "PS3")ifs.open(RunListPathps3);
	if (opt == "PS6")ifs.open(RunListPathps6);
	while (getline(ifs, line)){
    //convert run from string to int
    runnum = stoi(line);
	RunList.push_back(runnum);
	}
	NumofRuns = static_cast<int>(RunList.size());
}

void DIS_YIELD::ReadReport(TString opt){
	ifstream file;
    string word;
	Double_t result=-999999999;
	file.open(ReportOutput);
    if(file.is_open()!=1){
		cout << "Missing Report" << endl;
	}
	if (opt == "PS3"){
		while (file >> word){
			if (word == "Ps3_factor"){
				file >> word >> pS;
				result = pS;
				cout << opt << " is " << result << endl;
			}
		}

	}
	if (opt == "PS6"){
		while (file >> word){
			if (word == "Ps6_factor"){
				file >> word >> pS;
				result = pS;
				cout << opt << " is " << result << endl;
			}
		}
	}
	if (opt == "charge"){
		while (file >> word){
			if (word == "BCM4A"){
				file >> word;
				if (word == "Beam"){
					file >> word;
					if (word == "Cut"){
						file >> word;
						if (word == "Charge:"){
							file >> Charge;
							result = Charge;
							cout << opt << " is " << result << endl;
						}
					}
				}
			}
		}
	}
	
}

void DIS_YIELD::SetupDir(Int_t runnum, Int_t numofevent){
	ROOTfiles = Form(ROOTfilesPattern,runnum);
	ReportOutput = Form(ReportOutputPattern,runnum);
}

void DIS_YIELD::GetROOTfiles(){
	f1 = new TFile(ROOTfiles);
	t1 = (TTree*) f1->Get("T");
	t1->SetBranchStatus("*",false);
	t1->SetBranchStatus("H.BCM4A.scalerCurrent",true);
	t1->SetBranchStatus("H.cal.etottracknorm",true);
	t1->SetBranchStatus("T.hms.hEL_REAL_tdcMultiplicity",true);
	t1->SetBranchStatus("T.hms.hEL_REAL_tdcTimeRaw",true);
	t1->SetBranchStatus("T.hms.hEDTM_tdcTimeRaw",true);
	t1->SetBranchStatus("H.cer.npeSum",true); 
	t1->SetBranchStatus("H.gtr.dp",true);
	t1->SetBranchStatus("H.hTRIG6.scalerCut",true);
	t1->SetBranchStatus("H.EDTM.scalerCut",true);
	t1->SetBranchStatus("H.hTRIG3.scalerCut",true);
	t1->SetBranchStatus("H.hod.goodscinhit",true);
	t1->SetBranchStatus("H.cal.etotnorm",true);
	t1->SetBranchStatus("H.dc.ntrack",true);
	t1->SetBranchStatus("H.hod.betanotrack",true);
	t1->SetBranchAddress("H.cal.etottracknorm",&etottracknorm);
	t1->SetBranchAddress("H.BCM4A.scalerCurrent",&current4A);
	t1->SetBranchAddress("T.hms.hEL_REAL_tdcMultiplicity",&elrealtdcmulti);
	t1->SetBranchAddress("T.hms.hEDTM_tdcTimeRaw",&edtmtdctime);
	t1->SetBranchAddress("H.cer.npeSum", &npesum);
	t1->SetBranchAddress("T.hms.hEL_REAL_tdcTimeRaw",&elrealtdctime);
	t1->SetBranchAddress("H.gtr.dp",&hDelta);
	t1->SetBranchAddress("H.hTRIG6.scalerCut",&hTRIG6_scaler_4a);
	t1->SetBranchAddress("H.EDTM.scalerCut",&hEDTM_scaler_4a);
	t1->SetBranchAddress("H.hTRIG3.scalerCut",&hTRIG3_scaler_4a);
	t1->SetBranchAddress("H.hod.goodscinhit",&hGoodScinHit);
	t1->SetBranchAddress("H.hod.betanotrack",&hBetanotrk);
    t1->SetBranchAddress("H.cal.etotnorm",&hCAL_etotnorm);    
	t1->SetBranchAddress("H.dc.ntrack",&hdc_ntrack);    

	
}

void DIS_YIELD::CloseROOTfiles(){
	delete t1;
	f1->Close();
	delete f1;
}

void DIS_YIELD::GetElrealEff(Double_t& x1,Double_t& x2){
	Double_t elrealtrigeff;
	Double_t elreal_should;
	Double_t elreal_did;

	TH1F *sh_elec = new TH1F("", "", 150, 0.0, 1.5);
	TH1F *sh_elec_elreal = new TH1F("", "", 150, 0.0, 1.5);

	Int_t Nentries = t1->GetEntries();
	for (int i = 0; i < Nentries; i++){
		t1->GetEntry(i);
		if (i%10000 == 0) cout << "Event " << i << endl;
		// here I am getting elreal trig eff with the current cut
		// num = edtm and cer npe > 1 and elreal tdc multi and elreal tdc > 0, totshtrk
		// dem = edtm and cer npe > 1 and elreal tdc multi, totshtrk
		bool c_edtm,c_npe,c_current,c_elreal_tdc,c_elreal_tdc_mul,c_etottrknorm,c_hdelta;
		this->SetCut(c_edtm,c_npe,c_current,c_elreal_tdc,c_elreal_tdc_mul,c_etottrknorm,c_hdelta);

		if (c_edtm && c_npe && c_current && c_elreal_tdc_mul){
			sh_elec->Fill(etottracknorm); // this is denominator
		}
		if (c_edtm && c_npe && c_current && c_elreal_tdc && c_elreal_tdc_mul){
			sh_elec_elreal->Fill(etottracknorm); // this is numerator
		}
	}//end of event loop
	Double_t int_elreal_elec_only_error;
    Double_t int_elreal_elec_only = sh_elec_elreal->IntegralAndError(60,130,int_elreal_elec_only_error);
	Double_t int_elec_only_error;
    Double_t int_elec_only = sh_elec->IntegralAndError(60,130,int_elec_only_error);
	Double_t elreal_error = fabs(int_elreal_elec_only/int_elec_only) * sqrt(pow(int_elreal_elec_only_error / int_elreal_elec_only, 2) + pow(int_elec_only_error / int_elec_only, 2));
	x1 = int_elreal_elec_only/int_elec_only;
	x2 = elreal_error;


	delete sh_elec;
	delete sh_elec_elreal;
}

void DIS_YIELD::SetCut(bool& c_edtm,bool& c_npe,bool& c_current,bool& c_elreal_tdc,bool& c_elreal_tdc_mul,bool& c_etottrknorm, bool& c_hdelta){
		c_edtm = edtmtdctime == 0;
		c_npe = npesum>1;
		c_current = current4A>2;
		c_elreal_tdc = elrealtdctime>0;
		c_elreal_tdc_mul = elrealtdcmulti == 1 || elrealtdcmulti == 0;
		c_etottrknorm = etottracknorm > 0.6;
		c_hdelta = abs(hDelta)<=8.;
}

void DIS_YIELD::GetYield(Double_t& trk_Yield_bcm4a, Double_t& trk_err_bcm4a, Double_t triggereff){
	TH1F *H_hDelta_trkY_4a = new TH1F("H_hDelta_trkY_4a", "HMS #delta, Yield_{trk}", 100, -15, 15);
	Int_t Nentries = t1->GetEntries();

	Double_t total_htrig6_accp_cut_4a;
	Double_t total_htrig6_scaler_bcm4a_cut,total_hedtm_scaler_bcm4a_cut;
	Double_t e_did_4a = 0, e_should_4a = 0;
	
	for (int i = 0; i < Nentries; i++){
		t1->GetEntry(i);
		if (i%10000 == 0) cout << "Event " << i << endl;
		bool c_edtm,c_npe,c_current,c_elreal_tdc,c_elreal_tdc_mul,c_etottrknorm,c_hdelta;
		this->SetCut(c_edtm,c_npe,c_current,c_elreal_tdc,c_elreal_tdc_mul,c_etottrknorm,c_hdelta);

		Bool_t good_elec_should = hGoodScinHit==1.&&hBetanotrk>0.85&&hBetanotrk<1.5&&hCAL_etotnorm>0.6&&npesum>0.7;
		Bool_t good_elec_did = good_elec_should&&hdc_ntrack>0.;



		if (c_edtm && c_npe && c_hdelta && c_npe && c_elreal_tdc) {
			H_hDelta_trkY_4a->Fill(hDelta);
		}
		if (c_edtm && c_elreal_tdc && c_current) {
			total_htrig6_accp_cut_4a++;
		}

		if (good_elec_should){e_should_4a++;}
		if (good_elec_did)e_did_4a++;

		if (hTRIG6_scaler_4a > total_htrig6_scaler_bcm4a_cut){
			total_htrig6_scaler_bcm4a_cut = hTRIG6_scaler_4a;
			total_hedtm_scaler_bcm4a_cut = hEDTM_scaler_4a;
		}

	}// End of Event Loop

	cout << "total_htrig6_accp_cut_4a is " << total_htrig6_accp_cut_4a << "total_htrig6_scaler_bcm4a_cut is " << total_htrig6_scaler_bcm4a_cut << " total_hedtm_scaler_bcm4a_cut is" << total_hedtm_scaler_bcm4a_cut << endl;
	Double_t cpuLT_bcm4a =  pS*total_htrig6_accp_cut_4a / (total_htrig6_scaler_bcm4a_cut-total_hedtm_scaler_bcm4a_cut);

	Double_t eTrkEff_bcm4a = e_did_4a / e_should_4a;
	Charge = Charge / 1000;

	cout << "e_did_4a is " << e_did_4a << " e_should_4a is " << e_should_4a << endl;

	cout << "Charge is " << Charge << " trkeff is " << eTrkEff_bcm4a << " trig eff is " << triggereff << " pS is " << pS << " Live Time is " << cpuLT_bcm4a << endl;
	H_hDelta_trkY_4a->Scale(1./(Charge*cpuLT_bcm4a*eTrkEff_bcm4a*triggereff));
    trk_Yield_bcm4a = pS*H_hDelta_trkY_4a->IntegralAndError(H_hDelta_trkY_4a->FindBin(-8.), H_hDelta_trkY_4a->FindBin(8.), trk_err_bcm4a);      
    trk_err_bcm4a= pS*trk_err_bcm4a;
	delete H_hDelta_trkY_4a;
}