#include"DIS_YIELD.C"

int main(){
	int a = 0;
	Double_t elrealeff;
	Double_t elrealefferror;
	DIS_YIELD obj(a);

	//everything here is to get elreal eff and error for the Yield;
	obj.SetupDir(3070,50000);
	obj.GetROOTfiles();
	obj.GetElrealEff(elrealeff,elrealefferror);
	cout << " " << elrealeff << " " << elrealefferror << endl;
	obj.CloseROOTfiles();

	DIS_YIELD objnew(a);

	vector <Double_t> Yield;
	vector <Double_t> YieldErr;
	vector <Int_t> Runnum;

	//Here is the actual Yield calculation.
	objnew.ReadRunList("PS6");


	for (int i=0; i<objnew.NumofRuns; i++){
		int runnum = objnew.RunList[i];
		Runnum.push_back(runnum);
		objnew.SetupDir(runnum,50000);
		objnew.ReadReport("PS6");
		objnew.ReadReport("charge");
		objnew.GetROOTfiles();
		Double_t yield4a; Double_t yield4aerr;
			cout << "1" << endl;
		objnew.GetYield(yield4a,yield4aerr,elrealeff);
			cout << "2" << endl;
		Yield.push_back(yield4a);
		YieldErr.push_back(yield4aerr);
		objnew.CloseROOTfiles();
		cout << yield4a <<  "  " << yield4aerr << endl;
	}

	return 0;
}