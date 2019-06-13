// modified on 2019/06/11

#define UTC_ID  0x09
#define GPS_ID  0x0A
#define S_event_ID  0x05
#define D_event_ID  0x0D
#define S_ext_Width_ID  0x06
#define D_ext_Width_ID  0x0E
#define COM_ID 0x0A
#define CH_NUM  13

{
    char filename[100];
    bool crc_flag=true;
    bool ped_flag=true;
	
    cout<< "please input the binary file name:" ;
    cin>>filename;

		
    Float_t mean_adc[CH_NUM];       // CH_NUM=12;
    Float_t rms_adc[CH_NUM];
    Float_t ch_array[CH_NUM];

    char rootname[100];
    char Hname[10];
    char Tname[10];
    int t_adc[CH_NUM]; 	
    char ch_data;
    int  count=0;
    int  seq_check=0;
    int  ch,adc=0;
    int  ch_test=0;

    strcpy(rootname,filename);
    strcat(rootname,".root");
    TFile * rootf= new TFile(rootname,"RECREATE" );

    std::ifstream Rawfile(filename,ios::in | ios::binary | ios::in);

    std::ifstream::pos_type size = Rawfile.tellg();
    cout<<"the size of the file is "<<size<<endl;

if(Rawfile)
{

//---------------make event analysis
        TTree* tree = new TTree("event" ,"data of event");

        tree->Branch("CountID",&count,"count/I");
        for (int i= 0; i< CH_NUM; i++)
        {
            sprintf(Hname, "t_Ch%d",i);
            sprintf(Tname, "adc%d/I",i);
            tree->Branch(Hname,&t_adc[i],Tname);
            t_adc[i]=0;
        }

//---------------convert to histogram;
        TH1F* h_Mean = new TH1F("Mean","Mean of RAW ADC value",384,0,384);
        TH1F* h_RMS = new TH1F("RMS","RMS of RAW ADC value",384,0,384);

        TH1F *h[CH_NUM];
        for (int i= 0; i< CH_NUM; i++)
        {
            sprintf(Hname, "Ch%d",i);
            h[i] = new TH1F(Hname,"ADC_spectrum-byGongk" ,8192,0,4096);
        }


	while (!Rawfile.eof())
	{
		
		cout<<ch_data<<endl;	
		Rawfile.read(&ch_data,4);
		Rawfile.read(( char *)&adc,2); //read the ADC
		if(Rawfile.eof()) break;       //break at file end, 可在分析一个数据包前，判断这个包是否完整
		ch_test = int((adc&0x0000f000)>>12);	
			if(ch_test <CH_NUM-1 ) ch = ch_test;
			else ch= CH_NUM-1;
	
		adc = int(adc&0x00000fff);
		
		t_adc[ch] = adc;
		h[ch]->Fill(adc);              //fill the array of histogram
		seq_check++;
		cout<<"event number is "<<seq_check<<" the value is "<<adc<<endl;
		cout<<"ch number is  "<<ch_test<<endl;
		
	}


//-------------------Draw and save

        float MEAN_min = 0.0;
        float MEAN_max = 400.0;
        float RMS_min  = 0.0;
        float RMS_max  = 100.0;

        TCanvas *c0 = new TCanvas("c0","TMP",200,10,800,700);
        c0->SetLogy(); 
        c0->SetFillColor(10);
        c0->SetGrid();	
        c0->GetFrame()->SetFillColor(21);
        c0->GetFrame()->SetBorderSize(12); 

        for(int ch=0;ch<CH_NUM;ch++){
            mean_adc[ch] = h[ch]->GetMean();             //Calculate the Pedestal
            rms_adc[ch]= h[ch]->GetRMS();                //Calculate the RMS
            h_Mean->SetBinContent(ch+1,mean_adc[ch]);
            h_RMS->SetBinContent(ch+1,rms_adc[ch]);

            h[ch]->GetXaxis()->SetRangeUser(0.,8192.);   // Set the range
            h[ch]->SetFillColor(kYellow);                // Fill fill color to yellow
            h[ch]->SetMarkerStyle(20); 
            h[ch]->SetMarkerColor(kBlue); 
            h[ch]->SetMarkerSize(.6);                    // Draw histograms with blue markers
            h[ch]->GetXaxis()->SetTitle("ADC bins"); 
            h[ch]->GetYaxis()->SetTitle("Counts"); 
            h[ch]->Draw();
            c0->Modified();
            c0->Update();

            ch_array[ch] = ch;                   //0~383
        }
        h_Mean->GetXaxis()->SetTitle("Channels of VA chips"); 
        h_Mean->GetYaxis()->SetTitle("Mean of RAW ADC value"); 
        h_RMS->GetXaxis()->SetTitle("Channels of VA chips"); 
        h_RMS->GetYaxis()->SetTitle("RMS of RAW ADC value");

        Rawfile.close();

        rootf->Write();

}
else
{
cout<<"input file error";}
}

