// TheCode_NEST_Read1Climate_Console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS 1

//#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include  <math.h>
#include  <string.h>

#define maxPath_Length  200		//max characters for the length of pathes

#define maxYEARS  151  //1950 to 2100

#define MaxInputL 51
#define MaxLu	15	
#define MaxL	63            //maxi ground layers: 41 (3510), 50 (65m) 63 (11970) 81 (223.2m)
#define maxSoilL 63        //30: 15.75m, 26: 10m, 21: 5.5m

#define Pi  3.1415926
#define RAD 0.017453292222222  //convert degree to radiant Pi/180

//Thermal conductivity W cm-1 k-1
#define K_Minerals0  0.029       //Quartz: 0.08, Other minerals: 0.029
#define K_Organic    0.0025
#define K_Water      0.0057     //0.0057
#define K_Ice        0.022     //
#define K_Air        0.00025    //still dry air 0.00025

#define Stefen_Boltzmann  5.67e-8   //

#define TimeStep 1800           //1800, seconds
#define N_Steps  48				//48 number of time step in a day int(24*3600.0/TimeStep) 

//Heat capacity (J cm-3 k-1)
#define HC_Minerals 2.0  
#define HC_Organic  2.5
#define HC_Water    4.1868   
#define HC_Ice      2.05	// J/g (assume no value change)
#define HC_Air      0.0012 
#define Water_Ice   335.2       // 335.2, 333.7298 J/g
// dry moss (thickness:5cm, bulk density:33.3kg/m3, water 0.4g water/g moss)

#define Emissivity_Snow    0.98  //0.98
#define Emissivity_Veg     0.98  //0.98
#define Emissivity_Soil    0.98  //soil: 0.94 but always covered by floor or moss

#define Lamda1           2.479e6   //latent heat for vaporization of water at 10C (J/kg) 
#define Lamda2           2.813e6   //latent heat for vaporization of ice/snow  (J/kg) 
#define Ice_Specific_Volume 1.090513   // Ice_Density at 0C: 0.917 g/cm3

#define Fresh_Snow_Density_Min 50.0   //50 

#define C_Leaf_Boundary 0.08
#define C_Leaf_cuticular 0.00005
#define Soil_Water_Wi 0.92 


int Leap_Year(int Year);

double Interp_Years(int XX, int* X, double* Y);

void Monthly_Day_Length(int* DAYS, double sin_Lat, double cos_Lat, double* Day_Length);

void Solar_Radiation(int Leap_Year, double sin_Lat, double cos_Lat, int DOY, double* Rad_DayLength);
double Shusen_Integration_Eqs(double sin_Lat, double cos_Lat, int DOY, double Factors[]);

void Hydro_Parameters(int TextureID, double* Hydro_P);
void Peat_Hydro_Parameters(double Q, double* Hydro_P);
void Veg_Parameters(int Landcover, double LAImax, double* XX);

double Interpolate1(int N, double* DD, double X, double* YY);
double Layer_Weighted_Avg(int Input_LayerN, double* xDepth, double X_top, double X_bottom, double* YY);


double Temperature_Hourly(double Day_Length, double Tsunset_Yesterday, double Tmax_Today, double Tmin_Today, double Tmin_Nextday, double* Tair_Hr);
void Slope_Solar_Rad(int DOY, double Time, double sin_Lat, double cos_Lat, double cos_Slope, double sin_Slope, double cos_sFactors0,
	double DayF_Diffuse, double Rad0, double Slope, double Aspect, double ViewShed[], double Diffuse_Rad_F, double sFactors[], double Mid[]);
int Find_Snow_Layer(double Depth, int Layers, double* XX);

int Pick_Name(char* A0, char* A1);
int Checking_Landcover_Parameters(int vegYear, int vegType, double vegLAI, double vegH, double vegDrift,
double vegAlbedo, double veg_dPeat);


	// NEST variables
	double Mid[4];

	char InputPath[maxPath_Length], OutputPath[maxPath_Length], Input_FileName[maxPath_Length], Climate_FileName[maxPath_Length];

	int IF_Show_Monthly, IF_Show_Daily, IF_Show_Profile, IF_Slope_Variation, 
		Site_Sequence_Num, AllSites_1Year_Output_Num, AllSite_Output_Years;

	int YEAR0, YEARS;
	int Year, Month, Day, DOY, DAYS[12], Calendar_Year;
	int Leap_Year_Flag[maxYEARS], Current_Leap_Year_Flag;

	int Landcover, Input_LayerN, RootZone_Layers, Fire_DOY, Peat_Build_Layer;
	int Show_SW_Layers;

	double Albedo, Land_Albedo, Height, Ext_K, Woody_AI, Omega, SnowDrift_F;

	double Latitude, sin_Lat, cos_Lat,
		Slope, cos_Slope, sin_Slope,
		Aspect, ViewShed[16], Diffuse_Rad_F, sFactors[366][4], cos_sFactors0[366],
		Snow_Compaction_Factor0, Snow_K_Min, Geothermal_Flux0, MThermal_Conductivity;

	//  double Floor_Thick, Peat_Thick, MineralC, PeatC, Ground_Ice, Soil_Thick;
	double xDepth[MaxInputL], xMTexture[MaxInputL], xOTexture[MaxInputL],
		xForganicW[MaxInputL], xForganicV[MaxInputL],
		xSoil_Fxice[MaxInputL], xSoil_Fstone[MaxInputL];
	double PreFire_xDepth[2], PreFire_xOTexture[2], dPeat;

	double Para_a, Para_b, Para_c, Para_Tm, Para_AA;  //thawing/freezing parameters
	double Root_Frac[6];
	double Soil_Fstone[maxSoilL], Soil_Fxice[maxSoilL];

	double SurfOutflow_Depth, F_SurfOutflow,
		GrndOutflow_Depth, F_GrndOutflow,
		GrndInflow_Depth, F_GrndInflow,
		F_SurfInflow, F_Drain;

	double Day_Length[366],
		Daily_LAI[366], Daily_Tair[366], Daily_DTair[366], Daily_Precip[366],
		Daily_Vap[366], Daily_Solar_Rad[366], Daily_Emissivity_Air[366], Daily_Wind[366], Daily_Snowfall_Frac[366]; //input

	double Monthly_Tair[12], Monthly_Soil_Temp[12], Monthly_Soil_Water[12],
		Monthly_Snow_Pack[12], Monthly_AET[12], Monthly_Precip[12], Monthly_Water_Table[12],
		Monthly_Thaw_Depth[12], Monthly_Freeze_Depth[12], Annual_Values[15];

	double Annual_ProfileT[MaxL], Ta_Year_All[maxYEARS], Ta_Year;

	//Water-temperature
	int Total_Layers, Soil_Layers, SurfOutflow_Layer, Floor_Layers, Peat_Layers,
		Thaw_Layers, Lu, Last_Lu;

	double Subsidence, Subs_Layer;

	double MineralO, PeatO, C_Leaf_max;

	double Total_Depth, Total_Soil_Depth, Snow_Depth, Lu_Depth, Last_Lu_Depth, Last_Snow_Depth,
		Ground_Rainfall, Pond, SnowPond_Evap,
		Daily_Snow_Melt, Fresh_Snow_Density,
		Intercept_Rain, Intercept_Snow, Tsurface, Tcanopy, Tsunset_Yesterday, LE_Surface_Dew;

	double Depth[MaxL], Accum_Depth[MaxL], K_Minerals[MaxL], Tsoil[MaxL], DepthFsoil[MaxL], KsFsoil[MaxL];
	double Daily_Thaw_Depth[2], Daily_Freeze_Depth[3];

	double Mineral[maxSoilL], Organic[maxSoilL], Fsoil[maxSoilL],
		Porosity[maxSoilL], Field_Capacity[maxSoilL], Wilting_Point[maxSoilL],
		Water_b[maxSoilL], Water_Ks[maxSoilL], Water_Fs[maxSoilL],
		Soil_Water[maxSoilL], Fwater[maxSoilL], Fice[maxSoilL],
		DxAir[maxSoilL], DxWater[maxSoilL], DxIce[maxSoilL], DxWater_Last[maxSoilL];
	double Suction[maxSoilL];  //--------------------------

	double Tsnow[MaxLu], Snow_Density[MaxLu], Snow_Water[MaxLu], dLu_Depth[MaxLu];
	double Water_Table;

	FILE* fpt_Out_All_Sites, * fpt_Out_Day, * fpt_Out_Month, * fpt_Out_Year;
	int Out_Day[2], Out_Month[2], Out_Year[2], Out_Layer[2],
		Out_Year_AllSites[2], Out_Layer_AllSites[2];

	double FDDa_hy, FDDs_hy, Annual_Snowfall_hy;             //hydro-year

 //NEST functions
	int Run_From_Command_Line_Inputs(int Clim_Sequence, int Veg_Type, int *Site_Range);
	int Read_All_Climate_Data(int* Start_Sequence_N,
		double* Daily_Tair_All, double* Daily_DTair_All, double* Daily_Precip_All, double* Daily_Vap_All,
		double* Daily_Solar_Rad_All, double* Daily_Emissivity_Air_All, double* Daily_Wind_All, double* Daily_Snowfall_Frac_All);

	int A_Site_Daily_Data(int* Start_Sequence_N, double* Daily_Tair_All, double* Daily_DTair_All, double* Daily_Precip_All,
		double* Daily_Vap_All, double* Daily_Solar_Rad_All, double* Daily_Emissivity_Air_All, double* Daily_Wind_All,
		double* Daily_Snowfall_Frac_All, double* AllSites_Output);
	int Check_Range(char * String, double X, double Xmin, double Xmax);
	int Check_Range_Int(char *Name, int X, int Xmin, int Xmax);
	int Input_Soil_Profle(FILE* fpt_site);
	void Interpolate_To_Model_Layers_From_Input_Layers(int KK);
	//	int Find_Soil_Layer(double DD);

	double Interpolate(int IF_Interp, int Last_L, int Current_L, int L0, double* Y);

	void Estimate_Initials(double Tamplitude_Year);

	double Soil_Temp_Profile(int IF_Ini, double* AllSites_Output);

	void Update_Snow_Layers(int Lu, double Fresh_Snow_Depth);

	void APT_Thawing_Freezing(double apT, double LastT, double LastFice, double SW, double HC, double Mid[]);

	void Interpolate_To_Daily(int* DAYS, int j, double* Monthly_Values[], double* Daily_Values);
	void Interpolate_To_Daily1(int* DAYS, double* Monthly_Values, double* Daily_Values);
	double Penman_ET(double Tair, double VPD, double Rn_G, double ra, double rc);
	void rc(int Landcover, double C_Leaf_max, double LAI, double LE_Surface_Dew, double Intercept, double Snow, double Pond, double Soil_Temp0,
		double  SW0, double  Poro0, double  SWrootzone, double  FCrootzone,
		double  Radiation, double  Temp, double  Tmin, double  VPD, double R[]);

	void Soil_Daily_Gravity_Drainage();
	void Soil_Water_Distrib(int ET_Layers, int Wflux_Layers, double ET_Soil, double Evap);


int main(int argc, char *argv[])  //4, 
{
	int Clim_Sequence, Veg_Type, Site_Range[2];

/*
//for using call-exe 
	if (argc < 5) {
		printf("Five input values are needed: Climate_Sequence, Veg_type, and Site_Range[2]\n");
		return 0;
	}

	Veg_Type = atoi(argv[1]);
	Clim_Sequence = atoi(argv[2]);
	Site_Range[0] = atoi(argv[3]);
	Site_Range[1] = atoi(argv[4]);
*/

//for running the following sites 
	Veg_Type = 2;   //1:sedge, 2:shrub,
	Clim_Sequence = 1; //1-2 CLPX, 3-4:HV
	Site_Range[0] = 34473;
	Site_Range[1] = 34473;  //32

	printf("Clim_Sequence=%d, Veg_Type=%d, Site_Range=%d to %d\n", Clim_Sequence, Veg_Type, Site_Range[0], Site_Range[1]);

	Run_From_Command_Line_Inputs(Clim_Sequence, Veg_Type, Site_Range);

	return 1;
}



int Run_From_Command_Line_Inputs(int Clim_Sequence, int Veg_Type, int *Site_Range)
{
	int i, L;
	int Return_Value;

	i = 366 * maxYEARS;
	double* Daily_Tair_All = new double[i], * Daily_DTair_All = new double[i], * Daily_Precip_All = new double[i],
		* Daily_Vap_All = new double[i], * Daily_Solar_Rad_All = new double[i], * Daily_Emissivity_Air_All = new double[i],
		* Daily_Wind_All = new double[i], * Daily_Snowfall_Frac_All = new double[i];
	int Start_Sequence_N[maxYEARS];

	char Common_Path[150] = "E:\\Snow_Alaska\\Modelling_2026\\Model_Input_Output\\";
	char Veg_Type_Name[3][20] = {"NotUsed", "Sedge", "Shrub"};
	char Climet_Site_Name[5][20] = { "NotUsed", "CLPX_Ob_Precip", "CLPX_ERA5_Precip", "HV_Top", "HV_Bot" };

//	char RCP[6] = "RCP85";  //"RCP45"

	char Output_FileName_All_Sites[maxPath_Length];
	char String[2 * maxPath_Length];

	double P, F;

	sprintf(Climate_FileName, "%s\\Climate_Data\\%s_2010_2025.txt", Common_Path, Climet_Site_Name[Clim_Sequence]);
	
	IF_Slope_Variation = 1;  //no variation in slope/aspect, 1: with variations in slopw/aspect

	for (i = 0; i < 12; i++) DAYS[i] = 31;
	DAYS[1] = 28; DAYS[3] = 30; DAYS[5] = 30; DAYS[8] = 30; DAYS[10] = 30;

	sprintf(InputPath,  "%s\\%s\\Input\\",  Common_Path, Veg_Type_Name[Veg_Type]);
	sprintf(OutputPath, "%s\\%s\\Output\\", Common_Path, Veg_Type_Name[Veg_Type]);

	sprintf(Output_FileName_All_Sites, "%s_%s_Sites_%d_%d.txt", Climet_Site_Name[Clim_Sequence], Veg_Type_Name[Veg_Type], Site_Range[0], Site_Range[1]);  //no path

	Out_Year_AllSites[0] = 2010;   //1950
	Out_Year_AllSites[1] = 2025;
	Out_Layer_AllSites[0] = 10; Out_Layer_AllSites[1] = 1;  //Ts output

	if (Out_Layer_AllSites[1] > Out_Layer_AllSites[0]) L = Out_Layer_AllSites[1] - Out_Layer_AllSites[0] + 1;
	else L = 0;  //no Ts output

	AllSites_1Year_Output_Num = 8 + 12*3 + 13 + L;  //88+L. April 6 to 18 (13 days)
	AllSite_Output_Years = Out_Year_AllSites[1] - Out_Year_AllSites[0] + 1;

	double* AllSites_Output = new double[AllSite_Output_Years * AllSites_1Year_Output_Num];

	Total_Layers = MaxL;
	P = 1.0;
	Depth[0] = 10 * P;
	Accum_Depth[0] = Depth[0] * 0.5;
	F = Depth[0];
	for (L = 1; L < MaxL; L++) {
		if (L <= 9) Depth[L] = 10.0 * P;
		else if (L > 9 && L <= 13)  Depth[L] = 15.0 * P;
		else if (L > 13 && L <= 18) Depth[L] = Depth[L - 1] + 5.0 * P;
		else Depth[L] = Depth[L - 1] + 10.0 * P;
		F += Depth[L];
		Accum_Depth[L] = F - Depth[L] * 0.5;
	}   //end of for loop
	Total_Depth = F;

	if (Out_Year_AllSites[1] - Out_Year_AllSites[0] >= 0) {  
			sprintf(String, "%s%s", OutputPath, Output_FileName_All_Sites);
			if ((fpt_Out_All_Sites = fopen(String, "w")) == NULL) {
				printf("Output: Couldn't open the file %s", String);
				return 0;
			}
			fprintf(fpt_Out_All_Sites, "Year Thaw_max Freeze_max PermBase SnowCoverstart1 End1 Start2 End2 "); //
			for (i = 0; i < 12; i++) fprintf(fpt_Out_All_Sites, "%d ", i + 1); //snow depth
			for (i = 0; i < 12; i++) fprintf(fpt_Out_All_Sites, "%d ", i + 1); //SWE
//			for (L = 0; L < 20; L++) fprintf(fpt_Out_All_Sites, "%.2lf ", Accum_Depth[L]*0.01); //Annual_Ts_min in top 20 layers
//			for (L = 0; L < 20; L++) fprintf(fpt_Out_All_Sites, "%.2lf ", Accum_Depth[L] * 0.01); //Annual_Ts_max in top 20 layers
//			for (L = Out_Layer_AllSites[0]; L <= Out_Layer_AllSites[1]; L++) fprintf(fpt_Out_All_Sites, "%.2lf ", Accum_Depth[L] * 0.01);

			fprintf(fpt_Out_All_Sites, "SnowDepthApr5 6 7 8 9 10 11 12 13 14 15 16 17");
			fprintf(fpt_Out_All_Sites, "\n");
	}

	sprintf(Input_FileName, "Site_%d.txt", Site_Range[0]);  //the first site name for some inputs which are the same for other sites. No path 

	Return_Value = Read_All_Climate_Data(Start_Sequence_N, Daily_Tair_All, Daily_DTair_All, Daily_Precip_All, Daily_Vap_All,
		Daily_Solar_Rad_All, Daily_Emissivity_Air_All, Daily_Wind_All, Daily_Snowfall_Frac_All);

	if (Return_Value > 0) {
		printf("%s, %s\n", Climet_Site_Name[Clim_Sequence], Veg_Type_Name[Veg_Type]);

		for (Site_Sequence_Num = Site_Range[0]; Site_Sequence_Num <= Site_Range[1]; Site_Sequence_Num++) {
			sprintf(Input_FileName, "Site_%d.txt", Site_Sequence_Num);  //no path

			if (Return_Value > 0) {
				Return_Value = A_Site_Daily_Data(Start_Sequence_N, Daily_Tair_All, Daily_DTair_All, Daily_Precip_All, Daily_Vap_All,
					Daily_Solar_Rad_All, Daily_Emissivity_Air_All, Daily_Wind_All, Daily_Snowfall_Frac_All, AllSites_Output);
			}
		}  //Site_Sequence_Num
	}

	if (Out_Year_AllSites[1] >= Out_Year_AllSites[0]) fclose(fpt_Out_All_Sites);

	delete[]Daily_Tair_All;
	delete[]Daily_DTair_All;
	delete[]Daily_Precip_All;
	delete[]Daily_Vap_All;
	delete[]Daily_Solar_Rad_All;
	delete[]Daily_Emissivity_Air_All;
	delete[]Daily_Wind_All;
	delete[]Daily_Snowfall_Frac_All;

	delete[]AllSites_Output;

	if (Return_Value == 1) 	printf("The simulation is finished !\n");
	return 1;
}

int Read_All_Climate_Data(int* Start_Sequence_N, double* Daily_Tair_All, double* Daily_DTair_All, double* Daily_Precip_All,
	double* Daily_Vap_All, double* Daily_Solar_Rad_All, double* Daily_Emissivity_Air_All, double* Daily_Wind_All, double* Daily_Snowfall_Frac_All)
{
	char A[205];
	char String[2*maxPath_Length];

	int Return_Value = 0;
	int ID_LWaveRad_Cloudiness = 2;  //0:input Longwave radiation, 1: input cloudiness. 2: input Longwave radiation replaced, 
	int Y, M, D, Sequence_N;

	double Year_double, Daily_Tmin, Daily_Tmax;
	double RAD_Above[366], LWaveRad_Cloudiness[366];

	FILE* fpt_site, * fpt_Clm;   // input file pointers

	int i, j, L;
	double F, P;
	double Vap_Rad_Ps[4] = { 0.92, 0.70, 0.01, 2.4 };  //Inuvik

	sprintf(String, "%s%s", InputPath, Input_FileName);  
	if ((fpt_site = fopen(String, "r")) == NULL) {
		printf("Input site: Couldn't open %s", String);
		return -1;
	}
	fgets(A, 200, fpt_site);   //site description
	fgets(A, 200, fpt_site);   
//	Pick_Name(A, Climate_FileName);  //climate file name, not used
// 
	sprintf(String, "%s", Climate_FileName);  //not need for InputPath 
	if ((fpt_Clm = fopen(String, "r")) == NULL) {
		printf("Input climate: Couldn't open %s", String);

		fclose(fpt_site);
		return -2;
	}
	fscanf(fpt_Clm, "%lf", &Latitude); fgets(A, 200, fpt_Clm);   //replacing latitude
	for (i = 0; i < 2; i++) fgets(A, 200, fpt_Clm); //8, skip lines


	fscanf(fpt_site, "%d", &YEAR0);		fgets(A, 200, fpt_site);
	fscanf(fpt_site, "%d", &YEARS);		fgets(A, 200, fpt_site);

	if (YEARS > maxYEARS) {
		printf("The maximum years in the code (%d) is smaller than the input years (%d)!\n Modified the code or the input years!", maxYEARS, YEARS);
		fclose(fpt_site);
		return -2;
	}

	fgets(A, 200, fpt_site);  //for  Yrs_Ini_min and Yrs_Ini_max
	fgets(A, 200, fpt_site);  //A string for output file  

	fscanf(fpt_site, "%d%d", &Out_Day[0], &Out_Day[1]);		    fgets(A, 200, fpt_site);
	fscanf(fpt_site, "%d%d", &Out_Month[0], &Out_Month[1]);		fgets(A, 200, fpt_site);
	fscanf(fpt_site, "%d%d", &Out_Year[0], &Out_Year[1]);		fgets(A, 200, fpt_site);
	fscanf(fpt_site, "%d%d", &Out_Layer[0], &Out_Layer[1]);		fgets(A, 200, fpt_site);

	sprintf(String,"The Top layer for output " );
	Return_Value += Check_Range_Int(String, Out_Layer[0], 0, MaxL - 1);

	sprintf(String, "The bottom layer for output ");
	Return_Value += Check_Range_Int(String, Out_Layer[1], -1, MaxL - 1);

	if (Return_Value < 0) {
		fclose(fpt_Clm);
		fclose(fpt_site);
		return -3;
	}

	//--latitude
//	fscanf(fpt_site, "%lf", &Latitude);	fgets(A, 200, fpt_site);  //read from climate data, not here
//	sprintf(String, "Latitude");
//	Return_Value += Check_Range(String, Latitude, -90, 90);
	fgets(A, 200, fpt_site);

	fscanf(fpt_site, "%lf", &Slope);	fgets(A, 200, fpt_site);
	sprintf(String, "Slope");
	Return_Value += Check_Range(String, Slope, 0, 90);

	fscanf(fpt_site, "%lf", &Aspect);	fgets(A, 200, fpt_site);
	sprintf(String,"Aspect"); 
	Return_Value += Check_Range(String, Aspect, 0, 360);

	if (Return_Value < 0) {
		fclose(fpt_Clm);
		fclose(fpt_site);
		return -4;
	}

	//--viewshed blocking angles
	fgets(A, 200, fpt_site);   //a note line

	for (i = 0; i < 8; i++) fscanf(fpt_site, "%lf", &ViewShed[i]);
	fgets(A, 200, fpt_site);
	for (i = 8; i < 16; i++) fscanf(fpt_site, "%lf", &ViewShed[i]);
	fgets(A, 200, fpt_site);

	for (i = 0; i < 16; i++) {
		sprintf(String, "View shed %d ", i);
		Return_Value += Check_Range(String, ViewShed[i], 0., 90.);
	}
	if (Return_Value < 0) {
		fclose(fpt_Clm);
		fclose(fpt_site);
		return -5;
	}

	fclose(fpt_site);  //close site file here

	//convert to degree to radiant
	Latitude *= RAD;
	Slope *= RAD;
	Aspect *= RAD;
	for (i = 0; i < 16; i++) ViewShed[i] *= RAD;

	sin_Lat = sin(Latitude);
	cos_Lat = cos(Latitude);

	sin_Slope = sin(Slope);
	cos_Slope = cos(Slope);

	//-----------calculating daylength       
	//diffusing factor
	P = 0;
	Diffuse_Rad_F = 0;
	for (i = 0; i < 16; i++) {   //directions
		for (j = 0; j < 90; j++) {  //degrees
			F = sin(j * RAD) * cos(j * RAD);
			P += F;
			if (j >= ViewShed[i] / RAD)  Diffuse_Rad_F += F;
		}
	}
	Diffuse_Rad_F /= P;        //fraction of sky not blocked

	Current_Leap_Year_Flag = 1;
	DAYS[1] = 29;

	DOY = 0;
	for (j = 0; j < 12; j++) {
		for (i = 0; i < DAYS[j]; i++) {
			Day_Length[DOY] = Shusen_Integration_Eqs(sin_Lat, cos_Lat, DOY, sFactors[DOY]);
			cos_sFactors0[DOY] = cos(sFactors[DOY][0]);

			Solar_Radiation(Current_Leap_Year_Flag, sin_Lat, cos_Lat, DOY, Mid);
			RAD_Above[DOY] = Mid[0];    //daily solar radiation above the atmosphere MJ/m2/day
			DOY++;
		}
	}

	//==================================================================
	Sequence_N = 0;
	for (Year = 0; Year < YEARS; Year++) {
		Calendar_Year = Year + YEAR0;
		if (Leap_Year(Calendar_Year) == 1) {
			DAYS[1] = 29;
			Leap_Year_Flag[Year] = 1;
		}
		else {
			DAYS[1] = 28;
			Leap_Year_Flag[Year] = 0;
		}
		Current_Leap_Year_Flag = Leap_Year_Flag[Year];

		DOY = 0;
		for (Month = 0; Month < 12; Month++) {
			for (Day = 0; Day < DAYS[Month]; Day++) {

				//Met1km format	
				fscanf(fpt_Clm, "%lf %d %d %lf %lf %lf %lf %lf %lf %lf", &Year_double, &M, &D, &Daily_Tmin, &Daily_Tmax,
					&Daily_Precip[DOY], &Daily_Vap[DOY], &Daily_Solar_Rad[DOY], &LWaveRad_Cloudiness[DOY], &Daily_Wind[DOY]);  //Met1km format
				Y = int(Year_double);

				//		Daily_Tmin += dTmin[DOY];  //adjust the site conditions
				//		Daily_Tmax += dTmax[DOY];
				//		Daily_Precip[DOY] *= dPrecip[DOY]*0.01;

				Daily_Tair[DOY] = (Daily_Tmax + Daily_Tmin) * 0.5;
				Daily_DTair[DOY] = Daily_Tmax - Daily_Tmin;
				if (Daily_DTair[DOY] < 0.5) Daily_DTair[DOY] = 0.5;
				if (Daily_Wind[DOY] < 0.1) Daily_Wind[DOY] = 0.1;

				//		Daily_Tair[DOY] += Sens_Test_F[0];                              //SensTest
				//		Daily_Precip[DOY] += Daily_Precip[DOY] * Sens_Test_F[1]*0.01;	//SensTest

				if (Daily_Tair[DOY] < 0) Daily_Snowfall_Frac[DOY] = 1.0;
//				if (Daily_Tmin < 0) Daily_Snowfall_Frac[DOY] = 1.0;   //
				else Daily_Snowfall_Frac[DOY] = 0.0;

				//		Daily_Vap[DOY] = 6.11*exp(17.27*Daily_Tmin/(Daily_Tmin+237.3)) * Vap_Rad_Ps[0];  
				//		Daily_Solar_Rad[DOY] = Vap_Rad_Ps[1]*(1-exp(-Vap_Rad_Ps[2]*pow(Daily_DTair[DOY], Vap_Rad_Ps[3])))* RAD_Above[DOY];  
				//		Daily_Solar_Rad[DOY] = RAD_Above[DOY] * LWaveRad_Cloudiness[DOY];  //Hay River:  LWaveRad_Cloudiness is transmittance

				//---------------
				if (Y != Calendar_Year || M != Month + 1 || D != Day + 1) {
					printf("Data mismatch: %d %d %d\n", Calendar_Year, Month + 1, Day + 1);
					fclose(fpt_Clm);
					return -6;
				}
				DOY++;
			} //Day
		}  //Month  

		//estimate effective downward emissivity
		L = 365 + Current_Leap_Year_Flag;
		if (ID_LWaveRad_Cloudiness == 2) {   //estimate cloudiness based on solar radiation (if SolarRad not avaialbe, it is estimated)
			for (DOY = 0; DOY < L; DOY++) {
				if (Daily_Solar_Rad[DOY] < 0.1) P = Vap_Rad_Ps[1] * (1 - exp(-Vap_Rad_Ps[2] * pow(Daily_DTair[DOY], Vap_Rad_Ps[3])));  //darkness
				else                        P = Daily_Solar_Rad[DOY] / (RAD_Above[DOY] + 0.0001);
				if (P > 0.724) P = 0.724;  //sqrt(1.05/2)=0.724569
				F = 1.02 * sqrt(1.05 - 2.0 * P * P);
				if (F > 0.99)      F = 0.99;
				else if (F < 0.05) F = 0.05;
				LWaveRad_Cloudiness[DOY] = F;   //cloudiness
			}
		}

		if (ID_LWaveRad_Cloudiness == 0) {  //estimated based on Longwave radiation
			for (DOY = 0; DOY < L; DOY++) {
				F = Daily_Tair[DOY] + 273.15;
				Daily_Emissivity_Air[DOY] = LWaveRad_Cloudiness[DOY] * 1000000.0 / (24. * 60. * 60.) / (Stefen_Boltzmann * F * F * F * F); //based on Tair
				if (Daily_Emissivity_Air[DOY] > 0.99) Daily_Emissivity_Air[DOY] = 0.99;
				else if (Daily_Emissivity_Air[DOY] < 0.35) Daily_Emissivity_Air[DOY] = 0.35;
			}
		}
		else {   //ID_LWaveRad_Cloudiness==1, estimated based on cloudiness
			double Rlw, c, T0, Tc, e0, W, Lclr, Lcld, t8, e8z, f8;
			for (DOY = 0; DOY < L; DOY++) {
				//		  if(Daily_Vap[DOY]>2.) F = 0.72+0.009*(Daily_Vap[DOY]-2.0); //Emissivity of clear sky based on Niemela et al., Atmospheric Research 58, 2001, 1–18
				//		  else                  F = 0.72-0.076*(Daily_Vap[DOY]-2.0);
				//		  Daily_Emissivity_Air[DOY] = (1 + 0.22*pow(LWaveRad_Cloudiness[DOY], 2.75))*F; //Maykut and Church, J. Appl. Meteorol. 1973, 12, 620–628. Alaska 

				c = LWaveRad_Cloudiness[DOY];  //cloudiness

				T0 = Daily_Tair[DOY] + 273.15;  //K
				Tc = T0 - 5.0;    //9,   5
				e0 = Daily_Vap[DOY] * 0.1; //mb to kPa

				W = 4650.0 * e0 / T0;
				Lclr = 59.38 + 113.7 * pow((T0 / 273.16), 6.0) + 96.96 * sqrt(W / 25.0);

				e8z = 0.24 + 2.98e-6 * e0 * e0 * exp(3000.0 / T0);
				t8 = 1 - e8z * (1.4 - 0.4 * e8z);
				f8 = -0.6732 + 0.6240e-2 * Tc - 0.9140e-5 * Tc * Tc;

				Lcld = t8 * c * f8 * Stefen_Boltzmann * Tc * Tc * Tc * Tc;

				Rlw = Lclr + Lcld;   //w/m2

				Daily_Emissivity_Air[DOY] = Rlw / (Stefen_Boltzmann * T0 * T0 * T0 * T0);
				if (Daily_Emissivity_Air[DOY] > 0.99) Daily_Emissivity_Air[DOY] = 0.99;
			}
		}  //else

		Start_Sequence_N[Year] = Sequence_N;
		DOY = 0;
		F = 0.0;
		for (Month = 0; Month < 12; Month++) {
			Monthly_Tair[Month] = 0.0;
			for (Day = 0; Day < DAYS[Month]; Day++) {
				F += Daily_Tair[DOY];

				Daily_Tair_All[Sequence_N] = Daily_Tair[DOY];
				Daily_DTair_All[Sequence_N] = Daily_DTair[DOY];
				Daily_Precip_All[Sequence_N] = Daily_Precip[DOY];
				Daily_Vap_All[Sequence_N] = Daily_Vap[DOY];
				Daily_Solar_Rad_All[Sequence_N] = Daily_Solar_Rad[DOY];
				Daily_Emissivity_Air_All[Sequence_N] = Daily_Emissivity_Air[DOY];
				Daily_Wind_All[Sequence_N] = Daily_Wind[DOY];
				Daily_Snowfall_Frac_All[Sequence_N] = Daily_Snowfall_Frac[DOY];

				DOY++;
				Sequence_N++;
			}
		}
		Ta_Year_All[Year] = F / (365. + Current_Leap_Year_Flag);

	}  //Year loop
	fclose(fpt_Clm);

	return 1;
}

int A_Site_Daily_Data(int* Start_Sequence_N, double* Daily_Tair_All, double* Daily_DTair_All, double* Daily_Precip_All,
	double* Daily_Vap_All, double* Daily_Solar_Rad_All, double* Daily_Emissivity_Air_All, double* Daily_Wind_All,
	double* Daily_Snowfall_Frac_All, double* AllSites_Output)
{
	int Sequence_N;
	int i, j;   // , Y, M, D;
	int L, Return_Value = 0;
	//	double Year_double;

	char Output_FileName[maxPath_Length];  //Climate_FileName[maxPath_Length],
	//   CString Dir_FileName, InputPath, OutputPath;
	double Input_LAI, Input_PeatThickness, Input_Fsnow, Input_Slope, Input_Aspect;
	int Yrs_Ini_min, Yrs_Ini_max;

	char String[maxPath_Length*2], Site_Note[maxPath_Length*2];
	char A[205];

	double Frac_Quartz, LAImax, Last_LAImax, LAImin, GDDLeaf1, GDDLeaf2, X0[10], Accum_dPeat;

	double Tamplitude_Year; // , RAD_Above[366], LWaveRad_Cloudiness[366];
  //  double Daily_Tmin, Daily_Tmax;

	FILE* fpt_site;  // , * fpt_Clm;   // input file pointers

//	double Ymin, Ymax, Profile_Ymin, Profile_Ymax;  // Actual Y value range
//	int PointNum, Xleft, Xright, Ytop, Ybottom;

	double Last_FreezeMax, Diff, F, P;

	int vegYear[3], vegType[2], vegFire[2], Last_Landcover;
	double vegLAI[2], vegH[2], vegDrift[2], vegAlbedo[2], veg_dPeat[2];

	int Calendar_Year_For_Initialization; //, ID_LWaveRad_Cloudiness;
  //  double dX[12], dTmin[366], dTmax[366], dPrecip[366];

 //   double Sens_Test_F[2];

 //   double Adjust_Precip[366];
 //   double Adjust_Precip_RCP45[12] = {6.03,1.40,4.12,0.99,0.72,0.80,1.04,0.96,2.25,4.60,9.62,4.32};   //North Klodike River RCP45
 //   double Adjust_Precip_RCP85[12] = {3.30,1.63,1.40,1.06,0.79,0.75,0.97,0.93,2.64,4.28,1.82,3.12};  //North Klodike River RCP85

 //	double Vap_Rad_Ps[4] = {1.0, 0.70, 0.01, 2.2};  //Ottawa
 //	double Vap_Rad_Ps[4] = {1.0, 0.86, 0.01, 2.4};  //VancouverUBC
 //	double Vap_Rad_Ps[4] = {0.92, 0.70, 0.01, 2.4};  //Inuvik
 //	double Vap_Rad_Ps[4] = {0.95, 0.8, 0.01, 2.4};  //Hay River
 //	double Vap_Rad_Ps[4] = {0.98, 0.87, 0.02, 1.8};  //Churchill Falls
 //	double Vap_Rad_Ps[4] = {1.02, 0.78, 0.08, 2.2};  //Resolut, Mould Bay
 //	double Vap_Rad_Ps[4] = {0.99, 0.75, 0.01, 2.4};  //Fort Resolution
 //	double Vap_Rad_Ps[4] = {1.04, 0.80, 0.01, 2.4};  //Churchill,  Assumed: Beaverlodge, Creelake, Thompson,  Winnipeg, Wynyard, Yorkton, Beaverlodge, BakerLake
 //	double Vap_Rad_Ps[4] = {1.05, 0.79, 0.02, 2.0};  //CartWright, Assumed: Goose, StJonhs West, Kentville, Truro, Valdor, Clyde, Kuujjuaq,
 //	double Vap_Rad_Ps[4] = {0.97, 0.87, 0.02, 2.1};  //Schefferville

 //	double Vap_Rad_Ps[4] = {0.815, 0.8, 0.01, 2.4};  //Yellowknife
 //	double Vap_Rad_Ps[4] = {0.865, 0.75, 0.01, 2.0};  //Whitehorse, Haines Junction, 

 //	double Vap_Rad_Ps[4] = {1.00, 0.75, 0.01, 2.4};  //default 

	FDDa_hy = 0; FDDs_hy = 0;  Annual_Snowfall_hy = 0;

	//-----------Initialize data
	Peat_Build_Layer = 0;

	sprintf(String, "%s%s", InputPath, Input_FileName);
	if ((fpt_site = fopen(String, "r")) == NULL) {
		printf("Input site: Couldn't open %s", String);
		return -1;
	}

	fscanf(fpt_site, "%lf%lf%lf%lf", &Input_LAI, &Input_Slope, &Input_Aspect, &Input_Fsnow); //&Input_PeatThickness,
	fgets(A, 200, fpt_site);   //site description
	sprintf(Site_Note, "%s", A);

	printf("%d ", Site_Sequence_Num);

	//	fscanf(fpt_site, "%lf%lf", &Sens_Test_F[0], &Sens_Test_F[1]); fgets(A,200, fpt_site); //for sensitivity tests
	/*
	   DAYS[1] = 29;
	   for(M=0; M<12; M++) fscanf(fpt_site, "%lf", &dX[M]);
	   fgets(A, 200, fpt_site);								//New
	   Interpolate_To_Daily1(DAYS, dX, dTmin);

	   for(M=0; M<12; M++) fscanf(fpt_site, "%lf", &dX[M]);
	   fgets(A, 200, fpt_site);								//New
	   Interpolate_To_Daily1(DAYS, dX, dTmax);

	   for(M=0; M<12; M++) fscanf(fpt_site, "%lf", &dX[M]);
	   Interpolate_To_Daily1(DAYS, dX, dPrecip);
	   fgets(A,200, fpt_site);
	//-----------------
	*/

	/*
	   fgets(A,200, fpt_site);  Climate_FileName = Pick_Name(A);  //climate file name

	   String=InputPath+Climate_FileName;
	   if((fpt_Clm = fopen(String, "r")) == NULL) {
			String= "Input climate: Couldn't open "+ String;
			AfxMessageBox(String);

			fclose(fpt_site);
			return -2;
	   }
	*/

	//	fscanf(fpt_site, "%d", &YEAR0);		fgets(A,200, fpt_site); 
	//	fscanf(fpt_site, "%d", &YEARS);		fgets(A,200, fpt_site); 

	//	fscanf(fpt_site, "%d", &Calendar_Year_For_Initialization);		fgets(A,200, fpt_site); 
	//    Return_Value = Check_Range("The year to intializing the model ", Calendar_Year_For_Initialization, YEAR0, YEAR0+YEARS);
	//	if(Return_Value < 0) {
	//		fclose(fpt_Clm);
	//		fclose(fpt_site);
	//		return -1;
	//	}

	for (i = 0; i < 3; i++) fgets(A, 200, fpt_site);
	fscanf(fpt_site, "%d%d", &Yrs_Ini_min, &Yrs_Ini_max);		fgets(A, 200, fpt_site);
	fgets(A, 200, fpt_site);  
	Pick_Name(A, Output_FileName);  //output file name


	fscanf(fpt_site, "%d%d", &Out_Day[0], &Out_Day[1]);		fgets(A,200, fpt_site);
	fscanf(fpt_site, "%d%d", &Out_Month[0], &Out_Month[1]);		fgets(A,200, fpt_site);
	fscanf(fpt_site, "%d%d", &Out_Year[0], &Out_Year[1]);		fgets(A,200, fpt_site);

	fscanf(fpt_site, "%d%d", &Out_Layer[0], &Out_Layer[1]);		fgets(A,200, fpt_site);
	sprintf(String, "The top layer for output ");
	Return_Value += Check_Range_Int(String, Out_Layer[0], 0, MaxL-1);
	sprintf(String, "The bottom layer for output ");
	Return_Value += Check_Range_Int(String, Out_Layer[1], -1, MaxL-1);
	if(Return_Value < 0) {
//		fclose(fpt_Clm);
		fclose(fpt_site);
		return -1;
	}

//--latitude
	fgets(A, 200, fpt_site);  //using latitude from climate data
//	fscanf(fpt_site, "%lf", &Latitude);	fgets(A,200, fpt_site);
//	Return_Value += Check_Range("Latitude", Latitude, -90, 90);

//		Latitude *= RAD;
//		sin_Lat = sin(Latitude);
//		cos_Lat = cos(Latitude);

	if (IF_Slope_Variation == 1) {
		fscanf(fpt_site, "%lf", &Slope);	fgets(A, 200, fpt_site);
		sprintf(String, "Slope");
		Return_Value += Check_Range(String, Slope, 0, 90);

		fscanf(fpt_site, "%lf", &Aspect);	fgets(A, 200, fpt_site);
		sprintf(String, "Aspect");
		Return_Value += Check_Range(String, Aspect, 0, 360);

		Slope *= RAD;
		Aspect *= RAD;

		sin_Slope = sin(Slope);
		cos_Slope = cos(Slope);

		if (Return_Value < 0) {
//			fclose(fpt_Clm);
			fclose(fpt_site);
			return -1;
		}

		//--viewshed blocking angles
		fgets(A, 200, fpt_site);   //a note line

		for (i = 0; i < 8; i++) fscanf(fpt_site, "%lf", &ViewShed[i]);
		fgets(A, 200, fpt_site);
		for (i = 8; i < 16; i++) fscanf(fpt_site, "%lf", &ViewShed[i]);
		fgets(A, 200, fpt_site);

		//convert to degree to radiant
		for (i = 0; i < 16; i++) ViewShed[i] *= RAD;
	}
	else {
		for (i = 0; i < 6; i++) fgets(A, 200, fpt_site);  //skip 5 lines
	}

	Calendar_Year_For_Initialization = YEAR0;

	//soil/ground profile
	L = Input_Soil_Profle(fpt_site);
	if (L < 0) {
		//		fclose(fpt_Clm);
		fclose(fpt_site);
		return -2;
	}
	Interpolate_To_Model_Layers_From_Input_Layers(0);

	fscanf(fpt_site, "%lf", &Frac_Quartz);			    fgets(A, 200, fpt_site);
	sprintf(String, "Fraction of Quartz");
	Return_Value += Check_Range(String, Frac_Quartz, 0, 1);

	fscanf(fpt_site, "%lf", &MThermal_Conductivity);	fgets(A, 200, fpt_site);
	sprintf(String, "Thermal conductivity of bedrock (W/cm/k)");
	Return_Value += Check_Range(String, MThermal_Conductivity, 1.0, 10.0);
	MThermal_Conductivity *= 0.01;   //W/(m k) to W/(cm k)

	fscanf(fpt_site, "%lf", &Geothermal_Flux0);			fgets(A, 200, fpt_site);

	if (fabs(Geothermal_Flux0) < 0.000001) {
		if (Geothermal_Flux0 < 0) Geothermal_Flux0 = -0.000001;
		else                      Geothermal_Flux0 = 0.000001;
	}
	if (Return_Value < 0) {
		//		fclose(fpt_Clm);
		fclose(fpt_site);
		return -3;
	}

	for (L = 0; L < Soil_Layers; L++) {
		K_Minerals[L] = pow(0.08, Frac_Quartz) * pow(K_Minerals0, (1.0 - Frac_Quartz));
		//	 if(K_Minerals[L]<MThermal_Conductivity) K_Minerals[L] = MThermal_Conductivity; 
	}

	for (L = Soil_Layers; L < Total_Layers; L++)  K_Minerals[L] = MThermal_Conductivity;

	fscanf(fpt_site, "%lf", &Snow_Compaction_Factor0);	fgets(A,200, fpt_site); 
	sprintf(String, "Snow compaction factor");
	Return_Value += Check_Range(String, Snow_Compaction_Factor0, 0.01, 3);
//	Snow_Compaction_Factor0 = 1.0;

	fscanf(fpt_site, "%lf", &Snow_K_Min);	fgets(A, 200, fpt_site);
	sprintf(String, "Minimum snow conductivity (W/(m k)");
	Return_Value += Check_Range(String, Snow_K_Min, 0.02, 1.0);
	Snow_K_Min *= 0.01; //unit conversion

	fscanf(fpt_site, "%lf", &F_SurfInflow);			fgets(A, 200, fpt_site);
	fscanf(fpt_site, "%lf %lf", &GrndInflow_Depth, &F_GrndInflow);	fgets(A, 200, fpt_site);

	fscanf(fpt_site, "%lf %lf", &SurfOutflow_Depth, &F_SurfOutflow);	fgets(A, 200, fpt_site);
	fscanf(fpt_site, "%lf %lf", &GrndOutflow_Depth, &F_GrndOutflow);	fgets(A, 200, fpt_site);

	if (F_GrndOutflow > 0 && F_GrndInflow > 0 && GrndOutflow_Depth > GrndInflow_Depth) {
		printf("Ground outflow depth (%.2lf) is deeper than ground inflow depth (%.2lf)", GrndOutflow_Depth, GrndInflow_Depth);

		Return_Value += -1;
	}

	sprintf(String, "Fraction of ground inflow");
	Return_Value += Check_Range(String, F_GrndInflow, 0.0, 0.9999);
	sprintf(String, "Fraction of surface outflow");
	Return_Value += Check_Range(String, F_SurfOutflow, 0.0, 0.9999);
	sprintf(String, "Fraction of ground outflow");
	Return_Value += Check_Range(String, F_GrndOutflow, 0.0, 0.9999);

	if (Return_Value < 0) {
		//	  fclose(fpt_Clm);
		fclose(fpt_site);
		return -4;
	}

	//open output files, and print head
	if (Out_Day[1] >= Out_Day[0]) {
		sprintf(String,"%s%s_Day.csv", OutputPath, Output_FileName);
		if ((fpt_Out_Day = fopen(String, "w")) == NULL) {
			printf("Output daily: Couldn't open %s", String);

			fclose(fpt_site);
			//fclose(fpt_Clm);
			return -5;
		}

		fprintf(fpt_Out_Day, "YY, YearMonthDay, DOY, Tair, Precip(mm), SnowDepth(cm), WaterTable(m), Thaw0(m), Thaw1(m), Freeze0(m), Freeze1(m), Freeze2(m),");
//		fprintf(fpt_Out_Day, "5cm, 10cm, 20cm, 50cm, 100cm, 150cm\n");
//		fprintf(fpt_Out_Day, "5cm, 10cm, 12.5cm, 15cm, 20cm, 25cm, ");  //for iButton sites

		for (L = Out_Layer[0]; L <= Out_Layer[1]; L++) 	fprintf(fpt_Out_Day, "%.2lf,", Accum_Depth[L] * 0.01);

//for Soil water
//		for (L = Out_Layer[0]; L <= Out_Layer[1]; L++) 	fprintf(fpt_Out_Day, "%.2lf,", Accum_Depth[L] * 0.01);

		fprintf(fpt_Out_Day, "\n");
	}  //if(Out_Day[1]>0

	if (Out_Month[1] >= Out_Month[0]) {
		sprintf(String,"%s%s_Month.csv", OutputPath, Output_FileName);
		if ((fpt_Out_Month = fopen(String, "w")) == NULL) {
			printf("Output monthly: Couldn't open %s", String);

			fclose(fpt_site);
//			fclose(fpt_Clm);
			if (Out_Day[1] >= Out_Day[0]) fclose(fpt_Out_Day);

			return -5;
		}
		fprintf(fpt_Out_Month, "YY, Year, Month, Tair, Precip(mm), Snowfall(mm), SnowDepth(cm), SnowcoverDays, WaterTable(m), Soil_Water(cm),");
		//		fprintf(fpt_Out_Month, "5cm, 10cm, 20cm, 50cm, 100cm, 150cm\n");
		for (L = Out_Layer[0]; L <= Out_Layer[1]; L++) 	fprintf(fpt_Out_Month, "%.2lf,", Accum_Depth[L] * 0.01);
		fprintf(fpt_Out_Month, "\n");
	}  //if Out_Month

	if (Out_Year[1] >= Out_Year[0]) {
		sprintf(String,"%s%s_Year.csv", OutputPath, Output_FileName);
		if ((fpt_Out_Year = fopen(String, "w")) == NULL) {
			printf("output annually: Couldn't open %s", String);

			fclose(fpt_site);
//			fclose(fpt_Clm);
			if (Out_Day[1] >= Out_Day[0])     fclose(fpt_Out_Day);
			if (Out_Month[1] >= Out_Month[0]) fclose(fpt_Out_Month);
			return -5;
		}
		fprintf(fpt_Out_Year, "Year, Tair, Precip(mm), Snowfall(mm), Snowfall_hy(mm), Thaw_freeze(m), MaxThaw(m), MaxFreeze(m), PermMin(m), SnowD_Jan, SnowD_Yr, SnowCover_Days, Subsidence(cm), ");  // Subsidence(cm), FreezeBackDOY1, FreezeBackDOY2
		fprintf(fpt_Out_Year, "5, 10, 20, 50, 100, TTOP, TDD, FDD, FDD_hy, nt, nf, nf_hy, Ratio_K, Offset_Surf, Offset_Thermal, E_t, E_f, TopPerm_T, ");

		for (L = Out_Layer[0]; L <= Out_Layer[1]; L++) 	fprintf(fpt_Out_Year, "%.2lf,", Accum_Depth[L] * 0.01);
		fprintf(fpt_Out_Year, "\n");

	}  //if Out_Year

/*
//-----------calculating daylength
//diffusing factor
   P=0;
   Diffuse_Rad_F = 0;
   for(i=0; i<16; i++) {   //directions
	   for(j=0; j<90; j++) {  //degrees
		   F = sin(j*RAD)*cos(j*RAD);
		   P += F;
		   if(j>=ViewShed[i]/RAD)  Diffuse_Rad_F += F;
	   }
   }
   Diffuse_Rad_F /= P;        //fraction of sky not blocked

	Current_Leap_Year_Flag=1;
	DAYS[1] = 29;

	DOY=0;
	for(j=0; j<12; j++) {
	   for(i=0; i<DAYS[j]; i++) {
		   Day_Length[DOY] = Shusen_Integration_Eqs(sin_Lat, cos_Lat, DOY, sFactors[DOY]);
		   cos_sFactors0[DOY] = cos(sFactors[DOY][0]);

		   Solar_Radiation(Current_Leap_Year_Flag, sin_Lat, cos_Lat, DOY, Mid);
		   RAD_Above[DOY] = Mid[0];    //daily solar radiation above the atmosphere MJ/m2/day
		   DOY++;
	  }
	}
//==================================================================
for(i=0; i<3; i++) fgets(A,200, fpt_Clm); //8, skip lines

//  fscanf(fpt_Clm, "%d", &ID_LWaveRad_Cloudiness);	fgets(A,200, fpt_Clm);
//  if(ID_LWaveRad_Cloudiness != 0 && ID_LWaveRad_Cloudiness != 1) {
//		String.Format("Climate Input: 2nd line. The values should be 0 or 1. Your input is %d", ID_LWaveRad_Cloudiness);
//		AfxMessageBox(String);
//		exit(0);
//  }
ID_LWaveRad_Cloudiness = 2;  //0:input Longwave radiation, 1: input cloudiness. 2: input Longwave radiation replaced,
*/
	fgets(A, 200, fpt_site);  //read a note line before vegetation

	//DAYS[1] = 29;
	//Interpolate_To_Daily1(DAYS, Adjust_Precip_RCP85, Adjust_Precip);

	//----------------------------------------------------------year loop
	Subsidence = 0.0;
	Subs_Layer = 0;

	for (Year = 0; Year < YEARS; Year++) {
		Calendar_Year = Year + YEAR0;
		Current_Leap_Year_Flag = Leap_Year_Flag[Year];

		if (Current_Leap_Year_Flag == 1)  DAYS[1] = 29;
		else  DAYS[1] = 28;


		DOY = 0;
		Sequence_N = Start_Sequence_N[Year];
		for (Month = 0; Month < 12; Month++) {
			for (Day = 0; Day < DAYS[Month]; Day++) {
				Daily_Tair[DOY] = Daily_Tair_All[Sequence_N];
				Daily_DTair[DOY] = Daily_DTair_All[Sequence_N];
				Daily_Precip[DOY] = Daily_Precip_All[Sequence_N];
				Daily_Vap[DOY] = Daily_Vap_All[Sequence_N];
				Daily_Solar_Rad[DOY] = Daily_Solar_Rad_All[Sequence_N];
				Daily_Emissivity_Air[DOY] = Daily_Emissivity_Air_All[Sequence_N];
				Daily_Wind[DOY] = Daily_Wind_All[Sequence_N];
				Daily_Snowfall_Frac[DOY] = Daily_Snowfall_Frac_All[Sequence_N];

				DOY++;
				Sequence_N++;
			}
		}
		/*
	   //Met1km format
			   fscanf(fpt_Clm, "%lf %d %d %lf %lf %lf %lf %lf %lf %lf", &Year_double, &M, &D, &Daily_Tmin, &Daily_Tmax, &Daily_Precip[DOY], &Daily_Vap[DOY], &Daily_Solar_Rad[DOY], &LWaveRad_Cloudiness[DOY], &Daily_Wind[DOY]);  //Met1km format
			   Y = int(Year_double);

	   //		Daily_Tmin += dTmin[DOY];  //adjust the site conditions
	   //		Daily_Tmax += dTmax[DOY];
	   //		Daily_Precip[DOY] *= dPrecip[DOY]*0.01;

			   Daily_Tair[DOY] = (Daily_Tmax + Daily_Tmin)*0.5;
			   Daily_DTair[DOY] = Daily_Tmax - Daily_Tmin;
			   if (Daily_DTair[DOY] < 0.5) Daily_DTair[DOY] = 0.5;
			   if(Daily_Wind[DOY]<0.1) Daily_Wind[DOY] = 0.1;

	   //		Daily_Tair[DOY] += Sens_Test_F[0];                              //SensTest
	   //		Daily_Precip[DOY] += Daily_Precip[DOY] * Sens_Test_F[1]*0.01;	//SensTest

			   if(Daily_Tair[DOY]<0) Daily_Snowfall_Frac[DOY] = 1.0;
			   else Daily_Snowfall_Frac[DOY] = 0.0;

	   //		Daily_Vap[DOY] = 6.11*exp(17.27*Daily_Tmin/(Daily_Tmin+237.3)) * Vap_Rad_Ps[0];
	   //		Daily_Solar_Rad[DOY] = Vap_Rad_Ps[1]*(1-exp(-Vap_Rad_Ps[2]*pow(Daily_DTair[DOY], Vap_Rad_Ps[3])))* RAD_Above[DOY];
	   //		Daily_Solar_Rad[DOY] = RAD_Above[DOY] * LWaveRad_Cloudiness[DOY];  //Hay River:  LWaveRad_Cloudiness is transmittance

	   //---------------
			   if(Y != Calendar_Year || M != Month+1 || D != Day+1) {
				   pDC->SetTextColor(White);
				   String.Format("Data mismatch: %d %d %d\n", Calendar_Year, Month+1, Day+1);
				   AfxMessageBox(String);

				   fclose(fpt_site);
				   fclose(fpt_Clm);
				   if(Out_Day[1]   >= Out_Day[0])   fclose(fpt_Out_Day);
				   if(Out_Month[1] >= Out_Month[0]) fclose(fpt_Out_Month);
				   if(Out_Year[1]  >= Out_Year[0])  fclose(fpt_Out_Year);
				   return -5;
			   }
			   DOY++;
		   } //Day
	   }  //Month

	   //estimate effective downward emissivity
		 L = 365 + Current_Leap_Year_Flag;
		 if(ID_LWaveRad_Cloudiness==2) {   //estimate cloudiness based on solar radiation (if SolarRad not avaialbe, it is estimated)
				for(DOY=0; DOY<L; DOY++) {
				  if(Daily_Solar_Rad[DOY]<0.1) P = Vap_Rad_Ps[1]*(1-exp(-Vap_Rad_Ps[2]*pow(Daily_DTair[DOY], Vap_Rad_Ps[3])));  //darkness
				   else                        P = Daily_Solar_Rad[DOY]/(RAD_Above[DOY]+0.0001);
				   if(P>0.724) P = 0.724;  //sqrt(1.05/2)=0.724569
				   F = 1.02*sqrt(1.05-2.0*P*P);
				   if(F>0.99)      F = 0.99;
				   else if(F<0.05) F = 0.05;
				   LWaveRad_Cloudiness[DOY] = F;   //cloudiness
				}
		 }

		 if(ID_LWaveRad_Cloudiness==0) {  //estimated based on Longwave radiation
			 for(DOY=0; DOY<L; DOY++) {
				  F = Daily_Tair[DOY] + 273.15;
				  Daily_Emissivity_Air[DOY] = LWaveRad_Cloudiness[DOY] * 1000000.0 / (24.*60.*60.) / (Stefen_Boltzmann * F*F*F*F); //based on Tair
				  if (Daily_Emissivity_Air[DOY] > 0.99) Daily_Emissivity_Air[DOY] = 0.99;
				  else if (Daily_Emissivity_Air[DOY] <0.35) Daily_Emissivity_Air[DOY] = 0.35;
			 }
		 }
		 else {   //ID_LWaveRad_Cloudiness==1, estimated based on cloudiness
			 double Rlw, c, T0, Tc, e0, W, Lclr, Lcld, t8, e8z, f8;
			 for(DOY=0; DOY<L; DOY++) {
	   //		  if(Daily_Vap[DOY]>2.) F = 0.72+0.009*(Daily_Vap[DOY]-2.0); //Emissivity of clear sky based on Niemela et al., Atmospheric Research 58, 2001, 1–18
	   //		  else                  F = 0.72-0.076*(Daily_Vap[DOY]-2.0);
	   //		  Daily_Emissivity_Air[DOY] = (1 + 0.22*pow(LWaveRad_Cloudiness[DOY], 2.75))*F; //Maykut and Church, J. Appl. Meteorol. 1973, 12, 620–628. Alaska

			   c = LWaveRad_Cloudiness[DOY];  //cloudiness

			   T0 = Daily_Tair[DOY] + 273.15;  //K
			   Tc = T0 - 5.0;    //9,   5
			   e0 = Daily_Vap[DOY]*0.1; //mb to kPa

			   W = 4650.0*e0/T0;
			   Lclr = 59.38 + 113.7 * pow( (T0/273.16), 6.0) +96.96 * sqrt(W/25.0);

			   e8z = 0.24 + 2.98e-6 * e0*e0 * exp(3000.0/T0);
			   t8 = 1 - e8z * (1.4 - 0.4 * e8z);
			   f8 = -0.6732 + 0.6240e-2 * Tc - 0.9140e-5 * Tc*Tc;

			   Lcld = t8 * c * f8 * Stefen_Boltzmann * Tc*Tc*Tc*Tc;

			   Rlw = Lclr + Lcld;   //w/m2

			   Daily_Emissivity_Air[DOY] = Rlw / (Stefen_Boltzmann * T0 * T0 * T0 * T0);
			   if(Daily_Emissivity_Air[DOY]>0.99) Daily_Emissivity_Air[DOY] = 0.99;
			 }
		 }  //else
	   */

		Ta_Year = Ta_Year_All[Year];

		DOY = 0;
		for (Month = 0; Month < 12; Month++) {
			Monthly_Tair[Month] = 0.0;
			for (Day = 0; Day < DAYS[Month]; Day++) {
				Monthly_Tair[Month] += Daily_Tair[DOY];
				DOY++;
			}
			Monthly_Tair[Month] /= DAYS[Month];
		}

		Tamplitude_Year = Monthly_Tair[6] - Monthly_Tair[0];

		//-----Land cover and vegetation
		if (Year == 0) {
			Accum_dPeat = 0;

			fscanf(fpt_site, "%d %d %lf %lf %lf %lf %lf %d",
				&vegYear[0], &vegType[0], &vegLAI[0], &vegH[0], &vegDrift[0], &vegAlbedo[0], &veg_dPeat[0], &vegFire[0]);
			fgets(A, 200, fpt_site);

			if (vegYear[0] != YEAR0) {
				printf("The first year %d should be %d\n", vegYear[0], YEAR0);
				fclose(fpt_site);
				//			fclose(fpt_Clm);

				if (Out_Day[1] >= Out_Day[0])   fclose(fpt_Out_Day);
				if (Out_Month[1] >= Out_Month[0]) fclose(fpt_Out_Month);
				if (Out_Year[1] >= Out_Year[0])  fclose(fpt_Out_Year);
				return -6;
			}
			L = Checking_Landcover_Parameters(vegYear[0], vegType[0], vegLAI[0], vegH[0], vegDrift[0], vegAlbedo[0], veg_dPeat[0]);
			if (L < 0) {
				fclose(fpt_site);
				//			fclose(fpt_Clm);
				if (Out_Day[1] >= Out_Day[0])   fclose(fpt_Out_Day);
				if (Out_Month[1] >= Out_Month[0]) fclose(fpt_Out_Month);
				if (Out_Year[1] >= Out_Year[0])  fclose(fpt_Out_Year);
				return -6;
			}

			if (vegFire[0] > 0 || fabs(veg_dPeat[0]) > 0.00001) {
				printf("The first year (%d) cannot have fire and peat depth change. They are modifed to 0\n", vegYear[0]);
				vegYear[0] = 0;
				veg_dPeat[0] = 0;
			}
			Fire_DOY = -1;  //no fire in the first year

			Landcover = vegType[0];
			LAImax = vegLAI[0];
			Height = vegH[0];
			SnowDrift_F = vegDrift[0];
			Land_Albedo = vegAlbedo[0];
			dPeat = veg_dPeat[0];

			Last_LAImax = LAImax;
			Last_Landcover = Landcover + 1;  //Arbitrarily +1 to made the difference so to get parameters at the first year 

			fscanf(fpt_site, "%d ", &vegYear[1]);

			if (vegYear[1] > 0) {
				fscanf(fpt_site, "%d %lf %lf %lf %lf %lf %d",
					&vegType[1], &vegLAI[1], &vegH[1], &vegDrift[1], &vegAlbedo[1], &veg_dPeat[1], &vegFire[1]);
				fgets(A, 200, fpt_site);

				if (vegH[1] < 0.5) vegH[1] = 0.5;

				if (vegYear[1] <= vegYear[0]) {
					printf("The year %d should be later than the previous recod (%d)\n", vegYear[1], vegYear[0]);
					L -= 1;
				}
				L = Checking_Landcover_Parameters(vegYear[1], vegType[1], vegLAI[1], vegH[1], vegDrift[1], vegAlbedo[1], veg_dPeat[1]);
				if (L < 0) {
					fclose(fpt_site);
					//				fclose(fpt_Clm);
					if (Out_Day[1] >= Out_Day[0])   fclose(fpt_Out_Day);
					if (Out_Month[1] >= Out_Month[0]) fclose(fpt_Out_Month);
					if (Out_Year[1] >= Out_Year[0])  fclose(fpt_Out_Year);
					return -6;
				}

				fscanf(fpt_site, "%d ", &vegYear[2]);
			}
		}

		//LAImax = 0.1;  //sens test

		if (vegYear[1] > 0) {  //
			if (vegYear[1] == Calendar_Year) {
				Landcover = vegType[1];
				LAImax = vegLAI[1];
				Height = vegH[1];
				SnowDrift_F = vegDrift[1];
				Land_Albedo = vegAlbedo[1];
				dPeat = veg_dPeat[1];

				if (vegFire[1] > 0) {
					if (vegYear[1] - vegYear[0] > 1) {
						printf("Please define the vegetation status in year %d (One-year before fire)\n", vegYear[1] - 1);

						fclose(fpt_site);
						//					fclose(fpt_Clm);
						if (Out_Day[1] >= Out_Day[0])     fclose(fpt_Out_Day);
						if (Out_Month[1] >= Out_Month[0]) fclose(fpt_Out_Month);
						if (Out_Year[1] >= Out_Year[0])   fclose(fpt_Out_Year);
						return -7;
					}
				}

				if (dPeat < -0.0001) vegFire[1] = 1;  //

				if (vegFire[1] >= 1) Fire_DOY = 213;  //assuming fire on Aug.1
				else               Fire_DOY = -1;   //not fire

				vegYear[0] = vegYear[1];
				vegType[0] = vegType[1];
				vegLAI[0] = vegLAI[1];
				vegH[0] = vegH[1];
				vegDrift[0] = vegDrift[1];
				vegAlbedo[0] = vegAlbedo[1];
				veg_dPeat[0] = veg_dPeat[1];
				vegFire[0] = vegFire[1];

				vegYear[1] = vegYear[2];

				if (vegYear[1] > 0) {
					fscanf(fpt_site, "%d %lf %lf %lf %lf %lf %d",
						&vegType[1], &vegLAI[1], &vegH[1], &vegDrift[1], &vegAlbedo[1], &veg_dPeat[1], &vegFire[1]);
					fgets(A, 200, fpt_site);
					if (vegH[1] < 0.5) vegH[1] = 0.5;

					L = Checking_Landcover_Parameters(vegYear[1], vegType[1], vegLAI[1], vegH[1], vegDrift[1], vegAlbedo[1], veg_dPeat[1]);

					if (-veg_dPeat[1] > 0.5 * PreFire_xDepth[0] && -veg_dPeat[1] <= PreFire_xDepth[0] && xForganicW[0] < 0.8) {
						printf("Year %d: Peat burned (%lf) > half of the 1st input layer (%lf) but its peat content (%.1lf) <80%%\n",
								vegYear[1], veg_dPeat[1], PreFire_xDepth[0], xForganicW[0] * 100.0);
						L -= 1;
					}
					if (-veg_dPeat[1] > 0.5 * PreFire_xDepth[1] && -veg_dPeat[1] <= PreFire_xDepth[1] && xForganicW[1] < 0.8) {
						printf("Year %d: Peat burned (%lf) > half of the 2nd input layer (%lf) but its peat content (%.1lf) <80%%\n",
								vegYear[1], veg_dPeat[1], PreFire_xDepth[1], xForganicW[1] * 100.0);
						L -= 1;
					}

					if (vegYear[1] <= vegYear[0]) {
						printf("The year %d should be later than the previous recod (%d)\n", vegYear[1], vegYear[0]);
						L -= 1;
					}

					if (L < 0) {
						fclose(fpt_site);
						//					 fclose(fpt_Clm);
						if (Out_Day[1] >= Out_Day[0])   fclose(fpt_Out_Day);
						if (Out_Month[1] >= Out_Month[0]) fclose(fpt_Out_Month);
						if (Out_Year[1] >= Out_Year[0])  fclose(fpt_Out_Year);
						return -6;
					}

					fscanf(fpt_site, "%d ", &vegYear[2]);

					//				 if(vegYear[2]<0 && fabs(veg_dPeat[1]) > 0.0001) {
					//						String.Format("Warning: Peat thickness changes continuously after year %d (%lf). It was forced to no change\n", vegYear[1], veg_dPeat[1]);
					//						AfxMessageBox(String);
					//						veg_dPeat[1] = 0;
					//				 }
				}
			}  //end of if vegYear[1]==Year+YEAR0
			else {
				Landcover = vegType[0];
				LAImax = Interp_Years(Calendar_Year, vegYear, vegLAI);
				Height = Interp_Years(Calendar_Year, vegYear, vegH);
				SnowDrift_F = Interp_Years(Calendar_Year, vegYear, vegDrift);
				Land_Albedo = Interp_Years(Calendar_Year, vegYear, vegAlbedo);
				dPeat = Interp_Years(Calendar_Year, vegYear, veg_dPeat);

				Fire_DOY = -1;   //no fire. fire year has to be defined specifically

			}  //
		}	//else if vegYear[1] > 0

		if (Landcover != Last_Landcover || fabs(LAImax - Last_LAImax) > 0.1) {
			Veg_Parameters(Landcover, LAImax, X0);
			Albedo = X0[0];
			Ext_K = X0[1];
			C_Leaf_max = X0[2];
			Woody_AI = X0[3];
			LAImin = X0[4];
			GDDLeaf1 = X0[5];
			GDDLeaf2 = X0[6];
			Omega = X0[7];  //clumping index
			Last_LAImax = LAImax;  //LAImax before uptating

		}
		Last_Landcover = Landcover;

		Daily_LAI[0] = LAImin;
		P = 0;
		for (i = 1; i < 212; i++) {    //to July. 31
			F = 0;
			if (Daily_Tair[i] > 0) {
				P += Daily_Tair[i];
				if (P >= GDDLeaf1 && P <= GDDLeaf2) F = (LAImax - LAImin) * Daily_Tair[i] / (GDDLeaf2 - GDDLeaf1);
				else F = 0;
			}
			Daily_LAI[i] = Daily_LAI[i - 1] + F;
		}

		j = 365 + Current_Leap_Year_Flag;
		for (i = 212; i < j; i++) {    //to July. 31
			P = Daily_Tair[i] - 0.5 * Daily_DTair[i]; //Tmin
			if (P < 0) {
				F = 0.01 - P * 0.01;
				if (F > 0.1) F = 0.1;
				P = (LAImax - LAImin) * F;
				Daily_LAI[i] = Daily_LAI[i - 1] - P;
				if (Daily_LAI[i] < LAImin) Daily_LAI[i] = LAImin;
			}
			else Daily_LAI[i] = Daily_LAI[i - 1];
		}

		//--------
		Last_FreezeMax = -1;
		if (Calendar_Year == Calendar_Year_For_Initialization) {
			Estimate_Initials(Tamplitude_Year);

			for (i = 1; i <= Yrs_Ini_max; i++) {  //i=>1: initializing
				Diff = Soil_Temp_Profile(i, AllSites_Output);

				if (fabs(Annual_Values[1] - Last_FreezeMax) < 0.01 && Diff < 0.0001 && i >= Yrs_Ini_min) break;
				else Last_FreezeMax = Annual_Values[1];

			}  //end for i
		}  //if Calendar_Year == Calendar_Year_For_Initialization

		if (Calendar_Year >= Calendar_Year_For_Initialization) {
			if (dPeat > 0) Accum_dPeat += dPeat;  //excluding fire consumption year
			if (Accum_dPeat > 1.0) {
				for (L = 0; L < Input_LayerN; L++) xDepth[L] += Accum_dPeat;

				if (Peat_Build_Layer == 0) {
					F = xOTexture[0] - PreFire_xOTexture[0];
					if (F < 0) F = 0;
					xOTexture[0] -= 0.1 * F;
				}
				else {  //Peat_Build_Layer ==1
					if (xDepth[0] <= PreFire_xDepth[1] - PreFire_xDepth[0] + 1) {  //top layer at least 1cm
						F = xOTexture[0] - PreFire_xOTexture[1];
						if (F < 0) F = 0;
						xOTexture[0] -= 0.1 * F;
					}
					else {
						F = xDepth[0] - (PreFire_xDepth[1] - PreFire_xDepth[0]);

						xDepth[0] = PreFire_xDepth[1] - PreFire_xDepth[0] + F;
						xOTexture[0] = PreFire_xOTexture[1];

						Input_LayerN += 1;
						for (L = Input_LayerN - 1; L >= 1; L--) {
							xDepth[L] = xDepth[L - 1];
							xMTexture[L] = xMTexture[L - 1];
							xOTexture[L] = xOTexture[L - 1];
							xForganicW[L] = xForganicW[L - 1];
							xSoil_Fstone[L] = xSoil_Fstone[L - 1];
						}
						xDepth[0] = F;
						xOTexture[0] = PreFire_xOTexture[0];
						Peat_Build_Layer = 0;
					}
				}
				Interpolate_To_Model_Layers_From_Input_Layers(1);
				Accum_dPeat = 0;
			}

//			if (Calendar_Year >= Out_Year_AllSites[0] && Calendar_Year <= Out_Year_AllSites[1]) {
//	   fprintf(fpt_Out_All_Sites, "%lf %lf ",  Sens_Test_F[0],  Sens_Test_F[1]);
//			}
			Soil_Temp_Profile(0, AllSites_Output);

		}  //if Calendar_Year >= Calendar_Year_For_Initialization
	}    //loop Year
	fclose(fpt_site);
	//	fclose(fpt_Clm);

	//  for(i=1; i<100; i++) {  //run more years with the same climate and gound conditions
	//		  Soil_Temp_Profile(pDC, 0);
	//  }

	if (Out_Day[1] >= Out_Day[0])     fclose(fpt_Out_Day);
	if (Out_Month[1] >= Out_Month[0]) fclose(fpt_Out_Month);
	if (Out_Year[1] >= Out_Year[0])   fclose(fpt_Out_Year);

	fprintf(fpt_Out_All_Sites, "%d %.2lf %.2lf %.2lf %.2lf\n",
		Site_Sequence_Num, Input_LAI, Input_Slope, Input_Aspect, Input_Fsnow);  //Input_PeatThickness,

	i = 0;
	for (int Y = 0; Y < AllSite_Output_Years; Y++) {
//		fprintf(fpt_Out_All_Sites, "%d ", Site_Sequence_Num); //used for test

		fprintf(fpt_Out_All_Sites, "%.0lf %.2lf %.2lf %.2lf ",
			AllSites_Output[i], AllSites_Output[i + 1], AllSites_Output[i + 2], AllSites_Output[i + 3]);  //Year, Thaw_Max, Freeze_Max, PermBase_min

//		Snowcover_Start[0], Snowcover_End[0], Snowcover_Start[1], Snowcover_End[1]
		fprintf(fpt_Out_All_Sites, "%.0lf %.0lf %.0lf %.0lf ", AllSites_Output[i + 4], AllSites_Output[i + 5], AllSites_Output[i + 6], AllSites_Output[i + 7]);

		for (Month = 0; Month < 12; Month++) fprintf(fpt_Out_All_Sites, "%.1lf ", AllSites_Output[i + 8 + Month]);  //snow depth
		for (Month = 0; Month < 12; Month++) fprintf(fpt_Out_All_Sites, "%.1lf ", AllSites_Output[i + 20 + Month]);  //SWE

//		for (L = 0; L < 20; L++) fprintf(fpt_Out_All_Sites, "%.2lf ", AllSites_Output[i + 40 + L]); //Annual_Ts_Min 28-47
//		for (L = 0; L < 20; L++) fprintf(fpt_Out_All_Sites, "%.2lf ", AllSites_Output[i + 60 + L]); //Annual_Ts_Max 48-67

//		for (L = Out_Layer_AllSites[0]; L <= Out_Layer_AllSites[1]; L++) {
//			fprintf(fpt_Out_All_Sites, "%.2lf ", AllSites_Output[i + 80 + L]);
//		}

		for (L = 0; L < 13; L++) fprintf(fpt_Out_All_Sites, "%.1lf ", AllSites_Output[i + 32 + L]);  //SNow depth in April 5 to 17, 13 days

		fprintf(fpt_Out_All_Sites, "\n");

		i += AllSites_1Year_Output_Num;
	}  //Y

	return 1;
}

int Check_Range(char *Name, double X, double Xmin, double Xmax)
{
	if (X<Xmin || X>Xmax) {
		printf("%s: %lf\n Should be from %lf to %lf", Name, X, Xmin, Xmax);
		return -1;
	}
	else return 0;
}

int Check_Range_Int(char *Name, int X, int Xmin, int Xmax)
{
	if (X<Xmin || X>Xmax) {
		printf("%s: %d\n Should be from %d to %d", Name, X, Xmin, Xmax);
		return -1;
	}
	else return 0;
}


int Pick_Name(char* A0, char* A1)
{
	int i, N;
	int n1, n2, n;

	N = strlen(A0);
	//locate 1st occuring of a char before space or tab at the beginning
	n1 = N;
	for (i = 0; i < N; i++) {
		if (A0[i] != ' ' && A0[i] != '	') {
			n1 = i - 1;
			break;
		}
	}
	//locate the 2nd occuring of a space or tab at the beginning
	n2 = N - 1;
	for (i = n1 + 1; i < N; i++) {
		if (A0[i] == ' ' || A0[i] == '	') {
			n2 = i;
			break;
		}
	}
	if (n2 < n1) {
		printf("%s\n", A0);
		printf("The input line is empty ! \n");
		exit(0);
	}

	if (n2 - n1 + 1 > 100) {
		printf("%s", A0);
		printf("The selected string too long %d\n", n2 - n1 + 1);
		exit(0);
	}
	else {
		n = 0;
		for (i = n1 + 1; i < n2; i++) {
			A1[n] = A0[i];
			n++;
		}
		A1[n] = '\0';
		n++;
	}
	return n;
}

int Checking_Landcover_Parameters(int vegYear, int vegType, double vegLAI, double vegH, double vegDrift,
	double vegAlbedo, double veg_dPeat)
{
	int Input_Error = 0;

	if (vegType < 1 || vegType>5) {
		printf("Year %d, vgetation types (%d) should be 1 to 5", vegYear, vegType);
		Input_Error += -1;
	}
	if (vegLAI < 0) {
		printf("Year %d, Leaf area index (%lf) should >=0", vegYear, vegLAI);
		Input_Error += -1;
	}

	if (vegH < 0) {
		printf("Year %d, vegetation height (%lf) should >=0", vegYear, vegH);
		Input_Error += -1;
	}
	if (vegDrift >= 1) {
		printf("Year %d, Snow_drifting factor (%lf) should be < 1", vegYear, vegDrift);
		Input_Error += -1;
	}
	if (vegAlbedo < 0 || vegAlbedo>0.9) {
		printf("Year %d, vgetation albedo (%lf) should be 0 to 0.9", vegYear, vegAlbedo);
		Input_Error += -1;
	}
	//    if(veg_dPeat < -50 || veg_dPeat > 5) {
	//		printf("Year %d, variation of peat seems too big (%lf)", vegYear, veg_dPeat);
	//	}

	return Input_Error;
}


double Interpolate(int IF_Interp, int Last_L, int Current_L, int L0, double* Y)
{
	//linear interpolate based on depth
	int L;
	double X1, X2, XX, YY;

	if (IF_Interp == 0) YY = Y[1];
	else {
		X1 = 0;
		X2 = 0.5 * Depth[Last_L];
		for (L = Last_L + 1; L < Current_L; L++) X2 += Depth[L];
		X2 += 0.5 * Depth[Current_L];

		XX = 0.5 * Depth[Last_L];
		for (L = Last_L + 1; L < L0; L++) XX += Depth[L];
		XX += 0.5 * Depth[L0];

		YY = Y[0] + (Y[1] - Y[0]) / (X2 - X1) * (XX - X1);
	}

	return YY;
}

double Interp_Years(int XX, int* X, double* Y)
{
	double YY;

	YY = Y[0] + (Y[1] - Y[0]) / (X[1] - X[0]) * (XX - X[0]);

	return YY;
}


int Input_Soil_Profle(FILE* fpt_site)
{
	char A[205];
	int Return_Value = 0;
	int N, L;
	double F, Last_Depth;

	if (Landcover <= 3) {  //tree, 1, 2, 3
		Root_Frac[0] = 0.3;  Root_Frac[1] = 0.3; 	Root_Frac[2] = 0.3; 	Root_Frac[3] = 0.1; 	Root_Frac[4] = 0.0; 	Root_Frac[5] = 0.0;   //yellowknife
	}
	else {  //shrubs/grass 4, 5
		Root_Frac[0] = 0.5;  Root_Frac[1] = 0.3; 	Root_Frac[2] = 0.2; 	Root_Frac[3] = 0.0; 	Root_Frac[4] = 0.0; 	Root_Frac[5] = 0.0;
	}

	fgets(A, 200, fpt_site); //one note line
	Last_Depth = 0;
	N = 0;
	do {
		fscanf(fpt_site, "%lf", &F);
		if (F < 0) fgets(A, 200, fpt_site); //end of the profile
		else {
			xDepth[N] = F;

			fscanf(fpt_site, "%lf%lf%lf%lf%lf",
				&xMTexture[N], &xForganicW[N], &xOTexture[N], &xSoil_Fxice[N], &xSoil_Fstone[N]);
			fgets(A, 200, fpt_site);

			//              checking
			if (xDepth[N] < Last_Depth) {
				printf("Input layer %d: the depth %.1lf should be deeper than the previous layer %.1lf", N + 1, xDepth[N], Last_Depth);
				Return_Value += -1;
			}
			Last_Depth = xDepth[N];

			sprintf(A, "Depth %.1lf: mineral soil texture \n", xDepth[N]);
			Return_Value += Check_Range(A, xMTexture[N], 3, 14);

			sprintf(A, "Depth %.1lf: organic matter texture \n", xDepth[N]);
			Return_Value += Check_Range(A, xOTexture[N], 0, 3);

			sprintf(A, "Depth %.1lf: Soil organic matter fraction\n", xDepth[N]); //dry weight fraction excluding stone
			Return_Value += Check_Range(A, xForganicW[N], 0.0, 100.0);

			sprintf(A, "Depth %.1lf: volumetric excess ice fraction\n", xDepth[N]);
			Return_Value += Check_Range(A, xSoil_Fxice[N], 0, 100.0);

			sprintf(A, "Depth %.1lf: volumetric stone fraction\n", xDepth[N]);
			Return_Value += Check_Range(A, xSoil_Fstone[N], 0, 100.0);

			sprintf(A, "Depth %.1lf: The total volumetric farction of stone and excess ice\n", xDepth[N]);
			Return_Value += Check_Range(A, xSoil_Fxice[N] + xSoil_Fstone[N], 0, 100.0);

			xForganicW[N] *= 0.01;  //fraction
			xSoil_Fstone[N] *= 0.01;
			xSoil_Fxice[N] *= 0.01;

			N++;
			if (N >= MaxInputL - 1) {
				printf("Input layers > allowed layers (%d)\n", MaxInputL - 1);
				return -1;
			}
		}  //else
	} while (F > 0); //do

	if (N == 0) {
		printf("The input layer number is %d. At least onw soil layer is required.\n", N);
		return -2;
	}
	Input_LayerN = N;

	// the layer below a pure rock is assumed as bedrock
	for (L = 1; L < Input_LayerN; L++) {
		if (xSoil_Fstone[L - 1] >= 0.999 && xSoil_Fstone[L] < 0.99) {
			printf("Layer depth %.0lf is below a pure rock layer. The model requires the layers below a pure rock layer as bedrock (stone_fraction=100%%)", xDepth[L]);
			return -4;
		}
	}
	//The last layer should be bedrock if it is shallower than the allowed model layer (119.70m)
	L = Input_LayerN - 1;
	if (xDepth[L] < Total_Depth && xSoil_Fstone[L] < 0.9999) {
		printf("If the last depth (%.0lf) is shallower than the modelled depth (%.0lfcm), it should be the beginning of bedrock (stone_fraction=100%%)", xDepth[L], Total_Depth);
		return -3;
	}

	//make sure no pure stone layer above bedrock layer
	N = Input_LayerN - 1;
	for (L = 0; L < Input_LayerN; L++) {
		if (xSoil_Fstone[L] >= 0.999) {
			N = L;
			break;
		}
	}
	Input_LayerN = N + 1;

	for (L = 0; L < 2; L++) {
		PreFire_xDepth[L] = xDepth[L];
		PreFire_xOTexture[L] = xOTexture[L];
	}
	//-------------------------------
	//thawing/freezing parameters
	Para_a = 20.0; //70,    20,  >0, usually 1 to 100, the large the faster of Fw with T when T is approaching to 0;
	Para_b = 0.9;  //0.8,   0.9,   0.5 to 0.99, general fraction of Fice when T is several degrees below 0C; Ottawa 0.98
	Para_c = 0.005;//0.0005,0.005,  0 to 0.01: Increasing rate of Fw with T when T is much less than 0oC;
//    Para_a = 5.25;
//    Para_b = 0.7857;
//    Para_c = 0.002;

	double B;
	Para_AA = -Para_a * Para_c;
	B = Para_a * Para_b - Para_a + Para_c;

	if (Para_AA > -0.0000001) Para_AA = -0.0000001;
	Para_Tm = (-B + sqrt(B * B - 4 * Para_AA)) / (2 * Para_AA);

	return Return_Value;
}


void Interpolate_To_Model_Layers_From_Input_Layers(int KK)
{
	int L;

	double X, X_top, X_bottom, X0[8], X1[8];
	double xPorosity[MaxInputL], xField_Capacity[MaxInputL], xWilting_Point[MaxInputL],
		xWater_Fs[MaxInputL], xWater_Ks[MaxInputL], xWater_b[MaxInputL],
		xMineral[MaxInputL];
	double V1[maxSoilL], V2[maxSoilL], V3[maxSoilL], V4[maxSoilL];
	double F, DD0;

	if (KK == 1) Input_LayerN -= 1;

	//profiles
	for (L = 0; L < Input_LayerN; L++) {
		Peat_Hydro_Parameters(xOTexture[L], X0);
		Hydro_Parameters((int)xMTexture[L], X1);

		//		F =  xForganicW[L]*X1[6]/((1-xForganicW[L])*X0[6] + xForganicW[L]*X1[6]); //valumetric fraction of SOM, calculated from weight fraction
		F = 0.075 + 1.301 * exp(-6.0 * xForganicW[L]);        //effects of SOM on BD
		F = 1.0 - (F - 0.0782245 * xForganicW[L]) / 1.376; //estimated effects of SOM on soil property

		xPorosity[L] = F * X0[0] + (1 - F) * X1[0];
		xField_Capacity[L] = F * X0[1] + (1 - F) * X1[1];
		xWilting_Point[L] = F * X0[2] + (1 - F) * X1[2];
		xWater_Fs[L] = F * X0[3] + (1 - F) * X1[3];
		xWater_Ks[L] = F * X0[4] + (1 - F) * X1[4];
		xWater_b[L] = F * X0[5] + (1 - F) * X1[5];

		xMineral[L] = (1 - F) * (1 - X1[0]);
		xForganicV[L] = F * (1 - X0[0]);
	}

	L = Input_LayerN;  //the last layer
	xPorosity[L] = xPorosity[L - 1];
	xField_Capacity[L] = xField_Capacity[L - 1];
	xWilting_Point[L] = xWilting_Point[L - 1];
	xWater_Fs[L] = xWater_Fs[L - 1];
	xWater_Ks[L] = xWater_Ks[L - 1];
	xWater_b[L] = xWater_b[L - 1];

	xMineral[L] = xMineral[L - 1];


	DD0 = xDepth[Input_LayerN - 1];  //input soil layer depth
	if (DD0 < Total_Depth) { //the last layer is bedrock
		xDepth[L] = xDepth[L - 1] + 1000;   //rock

		xForganicV[L] = 0;
		xSoil_Fxice[L] = 0;
		xSoil_Fstone[L] = 1;
	}
	else {
		xDepth[L] = xDepth[L - 1] + 1000;

		xForganicV[L] = xForganicV[L - 1];
		xSoil_Fxice[L] = xSoil_Fxice[L - 1];
		xSoil_Fstone[L] = xSoil_Fstone[L - 1];
	}
	//Find Soil Layer
	Input_LayerN += 1;  //add a layer

	F = 0;
	Soil_Layers = -1;
	for (L = 0; L < Total_Layers; L++) {
		F += Depth[L];
		if (F >= DD0) {
			Soil_Layers = L + 1;
			if (Soil_Layers > Total_Layers - 1) Soil_Layers = Total_Layers - 1;
			break;
		}
	}
	if (Soil_Layers == -1) Soil_Layers = Total_Layers - 1;

	//assigning layer values based on input profile
	if (KK == 1) {
		for (L = 0; L < Soil_Layers; L++) {
			V1[L] = Soil_Water[L] / Porosity[L];
			F = Depth[L] * Soil_Fxice[L];
			if (F > 0) {
				V2[L] = DxIce[L] / F;
				V3[L] = DxWater[L] / F;
				V4[L] = DxAir[L] / F;
			}
			else {
				V2[L] = 0;
				V3[L] = 0;
				V4[L] = 0;
			}
		}  //for
	}  //ifKK

	X_top = 0;
	X = 0.5 * Depth[0];  //in cm
	X_bottom = Depth[0];
	for (L = 0; L < Soil_Layers; L++) {
		Porosity[L] = Layer_Weighted_Avg(Input_LayerN, xDepth, X_top, X_bottom, xPorosity);
		Field_Capacity[L] = Layer_Weighted_Avg(Input_LayerN, xDepth, X_top, X_bottom, xField_Capacity);
		Wilting_Point[L] = Layer_Weighted_Avg(Input_LayerN, xDepth, X_top, X_bottom, xWilting_Point);
		Water_Fs[L] = Layer_Weighted_Avg(Input_LayerN, xDepth, X_top, X_bottom, xWater_Fs);
		Water_Ks[L] = Layer_Weighted_Avg(Input_LayerN, xDepth, X_top, X_bottom, xWater_Ks);
		Water_b[L] = Layer_Weighted_Avg(Input_LayerN, xDepth, X_top, X_bottom, xWater_b);
		Organic[L] = Layer_Weighted_Avg(Input_LayerN, xDepth, X_top, X_bottom, xForganicV);
		Mineral[L] = Layer_Weighted_Avg(Input_LayerN, xDepth, X_top, X_bottom, xMineral);
		Soil_Fxice[L] = Layer_Weighted_Avg(Input_LayerN, xDepth, X_top, X_bottom, xSoil_Fxice);
		Soil_Fstone[L] = Layer_Weighted_Avg(Input_LayerN, xDepth, X_top, X_bottom, xSoil_Fstone);

		//			Porosity[L]       = Interpolate1(Input_LayerN, xDepth, X, xPorosity); 
		//			Field_Capacity[L] = Interpolate1(Input_LayerN, xDepth, X, xField_Capacity);
		//			Wilting_Point[L]  = Interpolate1(Input_LayerN, xDepth, X, xWilting_Point);
		//			Water_Fs[L]       = Interpolate1(Input_LayerN, xDepth, X, xWater_Fs);
		//			Water_Ks[L]       = Interpolate1(Input_LayerN, xDepth, X, xWater_Ks);
		//			Water_b[L]        = Interpolate1(Input_LayerN, xDepth, X, xWater_b);  
		//			Organic[L]        = Interpolate1(Input_LayerN, xDepth, X, xForganicV); 
		//			Mineral[L]        = Interpolate1(Input_LayerN, xDepth, X, xMineral);
		//			Soil_Fxice[L]     = Interpolate1(Input_LayerN, xDepth, X, xSoil_Fxice);
		//			Soil_Fstone[L]    = Interpolate1(Input_LayerN, xDepth, X, xSoil_Fstone);

		X_top += Depth[L];
		X += Depth[L] * 0.5 + Depth[L + 1] * 0.5;
		X_bottom += Depth[L + 1];
	}

	if (KK == 1) {
		for (L = 0; L < Soil_Layers; L++) {
			Soil_Water[L] = V1[L] * Porosity[L];

			F = Depth[L] * Soil_Fxice[L];
			DxIce[L] = V2[L] * F;
			DxWater[L] = V3[L] * F;
			DxAir[L] = V4[L] * F;
		}
	}

	Total_Soil_Depth = 0;
	for (L = 0; L < Soil_Layers; L++) Total_Soil_Depth += Depth[L];

	if (Soil_Layers > 3) RootZone_Layers = 3;
	else RootZone_Layers = Soil_Layers;

	if (Soil_Layers > 5) Show_SW_Layers = 5;
	else  Show_SW_Layers = Soil_Layers;
}


double Layer_Weighted_Avg(int Input_LayerN, double* xDepth, double X_top, double X_bottom, double* YY)
{
	//assigne based on weighed values
	int L, L1, L2;
	double Y;
	double F, P;

	L1 = 0;
	for (L = 0; L < Input_LayerN; L++) {
		if (X_top <= xDepth[L]) {
			L1 = L;
			break;
		}
	}

	L2 = 0;
	for (L = 0; L < Input_LayerN; L++) {
		if (X_bottom <= xDepth[L]) {
			L2 = L;
			break;
		}
	}

	if (L1 == L2) Y = YY[L1]; //the same layer
	else {
		F = xDepth[L1] - X_top;
		P = YY[L1] * (xDepth[L1] - X_top);
		for (L = L1 + 1; L < L2; L++) {
			F += xDepth[L] - xDepth[L - 1];
			P += YY[L] * (xDepth[L] - xDepth[L - 1]);
		}
		F += X_bottom - xDepth[L2 - 1];
		P += YY[L2] * (X_bottom - xDepth[L2 - 1]);

		Y = P / F;
	}
	return Y;
}

double Interpolate1(int N, double* DD, double X, double* YY)
{
	int L;
	double Y;

	if (X <= DD[0])  Y = YY[0];
	else {
		Y = -1;
		for (L = 0; L < N - 1; L++) {
			if (X >= DD[L] && X <= DD[L + 1]) {
				Y = YY[L] + (YY[L + 1] - YY[L]) / (DD[L + 1] - DD[L]) * (X - DD[L]);
				break;
			}
		}
		if (Y < 0)
			Y = YY[N - 1];  //beyond the depth
	}
	return Y;
}


void Estimate_Initials(double Tamplitude_Year)
{
	int i, L;
	double Tair, Peat_Thick, F;

	//------------------------------
	Peat_Thick = 0;
	for (L = 0; L < Total_Layers; L++) {
		if (Mineral[L] < 0.1) Peat_Thick += Depth[L];
		else break;
	}

	if (SurfOutflow_Depth < 0) SurfOutflow_Layer = 0;
	else {
		SurfOutflow_Layer = Soil_Layers;
		F = 0;
		for (L = 0; L < Soil_Layers; L++) {
			F += Depth[L];
			if (F >= SurfOutflow_Depth) {
				SurfOutflow_Layer = L + 1;
				break;
			}
		}
	}
	if (SurfOutflow_Layer > Soil_Layers) SurfOutflow_Layer = Soil_Layers;


	Total_Soil_Depth = 0;
	for (L = 0; L < Soil_Layers; L++) {
		Total_Soil_Depth += Depth[L];
		Soil_Water[L] = Porosity[L];
	}

	Snow_Depth = 0.0;  //Initial snow depth cm (1mm water==1cm snow)
	for (i = 180; i < 365; i++) {
		if (Daily_Tair[i] < 0.0) Snow_Depth += Daily_Precip[i]; // *(1 - SnowDrift_F);
	}
	Last_Snow_Depth = Snow_Depth;

	//initial temperature
	double DD = 400.0, dSoil_Air_Temp;
	dSoil_Air_Temp = 5.0 * Snow_Depth / (30 + Snow_Depth) - 2.0 * Daily_LAI[200] / (3 + Daily_LAI[200])
		- 2.0 * Peat_Thick / (20 + Peat_Thick) -3;

	F = 0.0;
	for (L = 0; L < Total_Layers; L++) {
		F += Depth[L];
		Tsoil[L] = dSoil_Air_Temp + Ta_Year + F * 0.0001 * Geothermal_Flux0 / K_Minerals[Total_Layers - 1] +
			0.6 * Tamplitude_Year * exp(-F / DD) * cos(2.0 * 3.14159 * (0 - 200) / 365.0 - F / DD);
	}

	Daily_Thaw_Depth[0] = 0;
	for (L = 0; L < Total_Layers; L++) {
		if (Tsoil[L] > 0)  Daily_Thaw_Depth[0] += Depth[L];
		else break;
	}
	Daily_Thaw_Depth[1] = Daily_Thaw_Depth[0];

	Daily_Freeze_Depth[0] = 0;
	for (L = 0; L < Total_Layers; L++) {
		if (Tsoil[L] <= 0)  Daily_Freeze_Depth[0] += Depth[L];
		else break;
	}
	Daily_Freeze_Depth[1] = Daily_Freeze_Depth[0];
	Daily_Freeze_Depth[2] = Daily_Freeze_Depth[0];


	Tair = Daily_Tair[0];
	Tsunset_Yesterday = Daily_Tair[0] + Daily_DTair[0] * sin(Pi * (Day_Length[0] + 0.18) / (Day_Length[0] + 2.0 * 1.6));

	Tcanopy = Tair; //initial canopy temperature
	Tsurface = Tair;
	Intercept_Rain = 0;      //initial plant intercepted water
	Intercept_Snow = 0;      //initial plant intercepted snow (in water mm)
	Pond = 0;
	SnowPond_Evap = 0;
	Daily_Snow_Melt = 0;
	Ground_Rainfall = 0;

	LE_Surface_Dew = 0;

	//gradual freezing
	for (L = 0; L < Soil_Layers; L++) {
		if (Tsoil[L] < 0) {
			if (Tsoil[L] <= Para_Tm) Fice[L] = 1.0;
			else Fice[L] = Para_b - Para_c * Tsoil[L] - Para_b / (1 - Para_a * Tsoil[L]);

			DxAir[L] = 0; //cm
			DxWater[L] = 0; //cm
			DxWater_Last[L] = DxWater[L];
			DxIce[L] = Soil_Fxice[L] * Depth[L];
		}
		else {
			Fice[L] = 0;

			DxAir[L] = 0; //cm
			DxWater[L] = Soil_Fxice[L] * Depth[L]; //cm  //filled with water
			DxWater_Last[L] = DxWater[L];
			DxIce[L] = 0;
		}

		Fwater[L] = 1 - Fice[L];
	}

	//determing Thaw depth 
	if (Tsoil[0] < 0.0) Thaw_Layers = 0;
	else {
		Thaw_Layers = Total_Layers;
		for (L = 1; L < Total_Layers; L++) {
			if (Tsoil[L - 1] >= 0.0 && Tsoil[L] < 0.0) {
				Thaw_Layers = L;
				break;
			}
		} //for loop
		if (Thaw_Layers > Soil_Layers) Thaw_Layers = Soil_Layers;
	}     // end of else 

   //initial snow layers, snow density
	Lu = 0;
	if (Snow_Depth > 0.0) {
		if (Snow_Depth > 3 && Snow_Depth < 10.0) {
			Lu = 1;
			Lu_Depth = Snow_Depth;
		}
		else if (Snow_Depth >= 10.0) {
			Lu = (int)(Snow_Depth / 10.0 + 1);
			if (Lu > MaxLu) Lu = MaxLu;
			Lu_Depth = Snow_Depth / Lu;
		}

		for (L = 0; L < MaxLu; L++) {
			Tsnow[L] = Tair;
			Snow_Density[L] = 0.2;
			Snow_Water[L] = 0.0;
			dLu_Depth[L] = 0.0;
		}
	}   //end of if snow_Depth>0.0
	Last_Lu = Lu;
	Last_Lu_Depth = Lu_Depth;
	Last_Snow_Depth = Snow_Depth;
}

double Soil_Temp_Profile(int IF_Ini, double* AllSites_Output)
{
	int Precip_Time, L, I, J, i, k;
	int ET_Layers, WT_Layers, Wflux_Layers;
	int IF_Output_31Days = 1;

	double Sun_Set, Sun_Rise, Tair_Hr[N_Steps];

	double Soil_Temp[MaxL], HC[MaxL], K[MaxL], LAI_All, ExpLAI, ExpLAI_Sun, sin_Alt, Canopy_Wind,
		Lu_A0, Lu_A[MaxLu], A0, A[MaxL], Teq, Lu_Fac0, Lu_Fac[MaxLu], Fac[MaxL],
		BB, F, F1, F2, P, P1, P2, P3, P4, P5, Mid[4];

	double Lu_HC[MaxLu], Lu_K[MaxLu], Snow_Temp[MaxLu], Fresh_Snow_Depth,
		Snow_Melt, Snow_Melt_To_Soil, Lamda, Snow_Compaction_Factor;

	double Height1;
	double Time, LHeat, SHeat_Canopy, SHeat_Surface, dT, Tdew,
		ET_Soil, ETplant, Evap;

	double Daily_Tsoil[MaxL], Daily_Tcanopy, Daily_Tsurface, Daily_Tsnow[MaxLu],
		Daily_Rainfall, Daily_Snowfall, Daily_ETplant, Daily_ET_Soil, Daily_Evap, Daily_Outflow;

	static double Last_Tair, Last_Solar_Rad, Last_ETplant, Last_Evap, Last_WT,
		Last_SoilWater[5], Last_Tsoil[5];  //for show 

	double Geothermal_Flux, Tair, Tmin0, Tmin1, Tmax,
		Rad_avg, Rad_S, Rad_Direct, Rad_Diffuse, Surf_Albedo, VPD, DayF_Diffuse;

	double SWrootzone, FCrootzone, ra_canopy, ra_surface, Emissivity_Canopy, Emissivity_Air,
		G_Flux, LE_Canopy, LE_Surface,
		Rn_Surface, Rn_Canopy, LN2_Canopy, LN2_Surface;

	double Monthly_Evap[12], Monthly_Transp[12], Monthly_Outflow[12], Monthly_Ts[MaxL];

	double Thaw_Max = 0.0, Freeze_Max = 0.0, PermBase_Min = 1.0e10, Annual_Outflow = 0.0,
		Annual_Evap = 0, Annual_ETplant = 0.0, Annual_Precip = 0.0, Annual_Snowfall = 0, Soil_Water_0, Tb0;

	double TDDa = 0, FDDa = 0, TDDs = 0, FDDs = 0, nt, nf, Offset_Surf, Offset_Thermal, Ratio_K;
	double  nf_hy_output, Snowfall_hy_output, FDDa_hy_output;  //end in July 31

	double g_a, g_c, F_a, F_s, F_i, x_a, x_i, x_w, x_s, lam_a, lam_w, lam_s, lam_i, Solid;
	double Annual_Ts_Min[20], Annual_Ts_Max[20];  //for estimate max thaw/frezzing depths

	int FreezeBackDOY1 = 200, FreezeBackDOY2 = 367;
	double Annual_SnowCoverDays = 0, Annual_SnowDepth = 0;
	double Monthly_Snowfall[12], Monthly_SnowCoverDays[12], Monthly_SWE[12];
	double Snow_Depth_April[30];  //for output
	double Daily_Snow_Depth[366];
	int Snowcover_Period, Snowcover_Start[2], Snowcover_End[2];

//	double Ymin, Ymax;                     // Actual Y value range
//	int PointNum, Xleft, Xright, Ytop, Ybottom;

	//if(IF_Ini>0) srand(1);
	//------------------Initializing -------------
	Daily_Snowfall = 0.0;
	Daily_Rainfall = 0.0;
	Daily_Evap = 0.0;
	SnowPond_Evap = 0.0;
	G_Flux = 0.0;

	for (L = 0; L < Total_Layers; L++) Annual_ProfileT[L] = 0.0;
	for (L = 0; L < 20; L++) {
		Annual_Ts_Min[L] = 999.99;
		Annual_Ts_Max[L] = -999.99;
	}

	if (IF_Ini > 0) {    //initialization
		Geothermal_Flux = Geothermal_Flux0;
		//    for(L=0; L<Soil_Layers; L++) Soil_Water[L] = Porosity[L];  //assuming saturated on DOY=0 in initilization years
	}
	else {
		P = K_Minerals[Total_Layers - 1];          //K
		F = 0;
		for (L = Total_Layers - 2; L < Total_Layers; L++) F += Depth[L] * 0.5;
		Geothermal_Flux = 0.9 * P * (Tsoil[Total_Layers - 1] - Tsoil[Total_Layers - 2]) / F * 10000 + 0.1 * Geothermal_Flux0; //W/m2
  //    Geothermal_Flux = 0.0*P*(Tsoil[Total_Layers-1]-Tsoil[Total_Layers-2])/F*10000 + 1.0*Geothermal_Flux0; //W/m2
  //    if(Total_Layers == 81) Geothermal_Flux = Geothermal_Flux0;
	}  //end else

	for (L = 0; L < Soil_Layers; L++) {
		//	Soil_Fxice[L] -= DxWater[L]/Depth[L];

		Fsoil[L] = 1 - Soil_Fstone[L] - (DxIce[L] + DxWater[L]) / Depth[L];
		if (Fsoil[L] < 1.0e-10) Fsoil[L] = 1.0e-10;

		DepthFsoil[L] = Depth[L] * Fsoil[L];
		KsFsoil[L] = Water_Ks[L] * pow(Fsoil[L], -2.6);
	}
	Tb0 = Tsoil[Total_Layers - 1];

	Soil_Water_0 = Pond;
	for (L = 0; L < Soil_Layers; L++) Soil_Water_0 += Soil_Water[L] * Fsoil[L] * Depth[L] * 10.0;

	//---------------- simulation --Daily loop
	DOY = 0;
	for (Month = 0; Month < 12; Month++) {

		for (L = 0; L < Total_Layers; L++) Monthly_Ts[L] = 0;   //for output

		Monthly_Soil_Temp[Month] = 0.0;  //20cm soil temperature
		Monthly_Soil_Water[Month] = 0.0;
		Monthly_Water_Table[Month] = 0.0;
		Monthly_Snow_Pack[Month] = 0.0;
		Monthly_Thaw_Depth[Month] = 0.0;
		Monthly_Freeze_Depth[Month] = 0.0;
		Monthly_Precip[Month] = 0.0;
		Monthly_Snowfall[Month] = 0.0;
		Monthly_SnowCoverDays[Month] = 0.0;
		Monthly_SWE[Month] = 0.0;

		Monthly_Evap[Month] = 0.0;
		Monthly_Transp[Month] = 0.0;
		Monthly_AET[Month] = 0.0;
		Monthly_Outflow[Month] = 0.0;
		//--------------------Day loop
		for (Day = 0; Day < DAYS[Month]; Day++) {
			for (L = 0; L < Total_Layers; L++) Daily_Tsoil[L] = 0.0;
			for (L = 0; L < MaxLu; L++)        Daily_Tsnow[L] = 0;
			Daily_Outflow = 0.0;

			//  for(L=0; L<Soil_Layers; L++) 	Soil_Water[L] = Porosity[L];  //forcing saturated every day  from layer 2 down   ********************

			 // if(IF_Ini == 0 && Calendar_Year == 2000 && DOY >= 0) //IF_Ini == 0 && 
			//	  DOY = DOY;

			//--- fire ----
			if (DOY == Fire_DOY) {
				//	   Height = 0.1;
				//	   Land_Albedo *= 0.5;
				//	   for(i=DOY; i<366; i++) Daily_LAI[i] = 0.0;  
				//	   Woody_AI = 0.1;

				for (L = 0; L < 2; L++) {
					PreFire_xDepth[L] = xDepth[L];
					PreFire_xOTexture[L] = xOTexture[L];
				}

				F = -dPeat;
				if (F > PreFire_xDepth[1]) {
					printf("Year %d: The burned depth more than the top two layers. Reducing burned depth or deepening the 2nd input peat layer !", Calendar_Year);
					exit(0);
				}
				//assuming fire only consume maximum 2 layers
				if (F < xDepth[0]) {  //consume part of the first layer
					Peat_Build_Layer = 0;
					for (L = 0; L < Input_LayerN; L++) xDepth[L] -= F;

					xOTexture[0] *= 1 + F / PreFire_xDepth[0];
					if (xOTexture[0] > 1.5) xOTexture[0] = 1.5;
				}
				else { //1st layer consumed, 2nd layer partially consumed  
					Peat_Build_Layer = 1;
					Input_LayerN -= 1;
					F = -dPeat;
					for (L = 0; L < Input_LayerN; L++) {
						xDepth[L] = xDepth[L + 1] - F;

						xMTexture[L] = xMTexture[L + 1];
						xOTexture[L] = xOTexture[L + 1];
						xForganicW[L] = xForganicW[L + 1];
						xSoil_Fstone[L] = xSoil_Fstone[L + 1];
					}  //for
					xOTexture[0] *= 1 + F / PreFire_xDepth[1] * 0.5;
					if (xOTexture[0] > 1.5) xOTexture[0] = 1.5;
				}  //else
				dPeat = 0;
				Interpolate_To_Model_Layers_From_Input_Layers(1);
			}
			//-----------------
			LAI_All = Daily_LAI[DOY] + Woody_AI;  //Branch and bole effects:Biomass/50

			if (Landcover == 5) {
				Height1 = 0.5 * Daily_LAI[DOY];    //m
				if (Height1 > 1.0) Height1 = 1.0;
			}
			else Height1 = Height;
			if (Height1 < 0.2) Height1 = 0.2;

			if (Height1 > 15) Height1 = 15;  //15m as the reference height of wind

			F = (0.23 * Height1 + 10.0) / (0.06 * Height1);  //+10
			F = log(F);
			LN2_Canopy = F * F;  //used for canopy resistance calculation

			if (Snow_Depth > 5.0) LN2_Surface = 28.07;    //z=1m, z0=0.005m, z/z0=200, ln(200)*ln(200)=28.07 
			else               LN2_Surface = 9.0;	   //z=1m, z0=0.05m, z/z0=20, ln(20)*ln(20)=8.97 

			F1 = Height1 - Snow_Depth * 0.01;   //height of the canopy in m
			if (F1 < 0.1 * Height1) {
				F1 = 0.01;
				F2 = F1 * 0.01;  //roughtness length
			}
			else  F2 = 0.1 * F1;  //roughtness length

			P1 = 0.667 * F1; //Zero plane displacement  
			P2 = log((F1 - P1) / F2) / log((15.0 - P1) / F2);  //15.0 m
			Canopy_Wind = Daily_Wind[DOY] * P2;

			//glass/shrubs: Wapusk
			//    P2 = 0.01 + 0.5*exp(-0.2 * LAI_All); 
			//    P2 = 0.1 + exp(-0.8*pow(LAI_All, 0.333)); 

			ExpLAI = exp(-Ext_K * LAI_All * Omega);

			//---------snow processes
			F = SnowPond_Evap;            //in mm water
			if (Snow_Depth > 0.0) {    //sublimation and evaporation from snow
				for (L = 0; L < Lu; L++) {  //consume water from snow layers
					F1 = Snow_Water[L] * Lu_Depth * 10.0;  //water in a snow layer (mm)
					if (F > F1) {
						Snow_Water[L] = 0.0;
						F -= F1;
					}
					else {
						Snow_Water[L] -= F / (Lu_Depth * 10.0);
						F = 0.0;
						break;
					}
				}
				F1 = F * 0.1 / Snow_Density[0];  //snow depth for sublimation
				if (F1 > Snow_Depth) {
					Snow_Depth = 0.0;
					Lu = 0;
				}
				else {
					Snow_Depth -= F1;
					dLu_Depth[0] += F1;
					if (dLu_Depth[0] > Lu_Depth) dLu_Depth[0] = Lu_Depth;
				}
			}
			else if (Pond > 0) { //consumer water from pond when there is no snow
				if (F > Pond) {
					F -= Pond;  //pond: in mm
					Pond = 0.0;
				}
				else {
					Pond -= F;
					F = 0.0;
				}
			}

			if (Daily_Snow_Melt > 0.0) {
				F = Daily_Snow_Melt;  //water in cm
				if (Lu == 0) Snow_Melt_To_Soil = Daily_Snow_Melt;
				else {
					for (L = 0; L < Lu; L++) {
						Snow_Water[L] += F / Lu_Depth;          //water in fraction

						if (Snow_Density[L] > 0.2) F1 = 0.0;    //0.2 Irreduciable water saturation (not considered now)
						else F1 = 0.1 * (0.2 - Snow_Density[L]) / (0.2 - 0.05);  //saturation water fraction;

						if (Snow_Water[L] > F1) {
							Snow_Melt_To_Soil = (Snow_Water[L] - F1) * Lu_Depth;
							Snow_Water[L] = F1;
						}
						else {
							Snow_Melt_To_Soil = 0.0;
							break;
						}
					}  //end of for loop
				}  //else
			}      //end of if
			else Snow_Melt_To_Soil = 0.0;

			//infiltration from pond/precipitation/snow-melt (including surface lateral inflow)
			F1 = Ground_Rainfall + Snow_Melt_To_Soil * 10.0 + Pond; //mm avalibale water for infiltration
			F1 += (Ground_Rainfall + Snow_Melt_To_Soil * 10.0 / (1 - SnowDrift_F)) * F_SurfInflow;  //add surface input
			Ground_Rainfall = 0;

			//add water in the soil
			for (L = 0; L < Soil_Layers; L++) {
				if (F1 > 0.0 && Fwater[L] > 0.0) {
					F2 = (Porosity[L] - Soil_Water[L]) * Depth[L] * Fsoil[L] * 10.0;   //mm 
					if (F1 > F2) {  //saturating this layer
						Fice[L] = Fice[L] * Soil_Water[L] / Porosity[L];  //no thaw/freezing
						Fwater[L] = 1 - Fice[L];
						if (Fice[L] > 0) {
							BB = Para_c + Para_a * (Para_b - Fice[L]);
							Tsoil[L] = (-BB + sqrt(BB * BB - 4 * Para_AA * Fice[L])) / (2 * Para_AA);
						}
						Soil_Water[L] = Porosity[L];
						F1 -= F2;
					}
					else {
						Soil_Water[L] += F1 / (10.0 * Depth[L] * Fsoil[L]);
						F1 = 0.0;
						break;
					}   //else (F1>F2)
				}  //end     if Temp[]>0
				else break;
			}  //end for loop
			Pond = F1;

			//   Soil_Daily_Gravity_Drainage();

			//----lateral flow --------- 
			//..surface outflow from pond
			if (Pond > Snow_Depth * 10 * 0.2 && Pond > 0) {     //pond in mm
		  //pond is in mm, Pond lost only when Pond is deeper than 20% of snow depth
				if (Lu > 0) P1 = 0.5;  //snow effects on surfcae runoff
				else P1 = 1;

				F1 = Pond - Snow_Depth * 10 * 0.2;
				if (F1 < Pond + SurfOutflow_Depth * 10) F1 = Pond + SurfOutflow_Depth * 10;

				P = F1 * F_SurfOutflow * P1;  //mm water
				if (P < 0) P = 0;
				if (P > Pond) P = Pond;

				Pond -= P;
				Daily_Outflow += P;
			}

			//.. surface outflow from soil
			if (F_SurfOutflow > 0) {
				if (Daily_Thaw_Depth[0] > SurfOutflow_Depth) i = SurfOutflow_Layer;
				else i = Thaw_Layers;

				F1 = 0;
				F2 = Pond;
				for (L = 0; L < i; L++) {
					if (Soil_Water[L] / Porosity[L] > 0.9999) {
						F1 += Depth[L];
						F2 += (Soil_Water[L] - Field_Capacity[L]) * Depth[L] * Fsoil[L] * 10;  //mm water avaialble
					}
					else break;
				}
				P = F1 * F_SurfOutflow * 10; //mm water
				if (P > F2) P = F2;

				Daily_Outflow += P;

				if (P <= Pond) Pond -= P;  //mm water
				else {
					P -= Pond;
					Pond = 0;
					for (L = 0; L < i; L++) {
						F = (Soil_Water[L] - Field_Capacity[L]) * Depth[L] * Fsoil[L] * 10;  //mm
						if (F > P) {
							Soil_Water[L] -= P * 0.1 / (Depth[L] * Fsoil[L]);
							break;
						}
						else {
							P -= F;
							Soil_Water[L] = Field_Capacity[L];
						}
					}  //for
				}  //else
			}

			// find water table
			Water_Table = Total_Soil_Depth;
			WT_Layers = Soil_Layers;
			for (L = Soil_Layers - 1; L >= 0; L--) {
				if (Soil_Water[L] / Porosity[L] > 0.9999) {
					Water_Table -= Depth[L];
					WT_Layers -= 1;
				}
				else {
					if (Soil_Water[L] < Field_Capacity[L]) break;  //water table at bottom of the layer
					else {
						F = (Porosity[L] - Soil_Water[L]) / (Porosity[L] - Field_Capacity[L]);
						if (F > 1) F = 1;
						Water_Table -= Depth[L] * (1 - F);
						WT_Layers -= 1;
						break;
					}
				}
			}  //for

		  //  if(Water_Table<Depth[0]*0.5) {
		  //	  Water_Table -= Pond*0.1; 
		  //	  WT_Layers = 0;
		  //  }

		  //ground inflow
			P = (Water_Table - GrndInflow_Depth) * F_GrndInflow;  //water table change in cm
			if (P > 0) {
				F = Water_Table - P;  //new water table position 
				F1 = Total_Soil_Depth;
				WT_Layers = Soil_Layers;
				for (L = Soil_Layers - 1; L >= 0; L--) {
					F1 -= Depth[L];
					if (F < F1) {
						WT_Layers -= 1;
						if (Tsoil[L] > 0) Soil_Water[L] = Porosity[L]; //inflow only when Ts>0, assume saturated if Ts<0
					}
					else { //Water table will be in this layer
						if (Tsoil[L] > 0) {
							F2 = Porosity[L] - (Porosity[L] - Field_Capacity[L]) * (F - F1) / Depth[L];  //inflow only when Ts>0
							if (Soil_Water[L] < F2) Soil_Water[L] = F2;
						}
						break;
					}
				}
				Water_Table = F;
			}  //if ground inflow

		  // ground outflow
			if (Daily_Freeze_Depth[2] > 2000) { //Has permafrost in 20m depth, outflow occurs above thaw depth 
				if (Daily_Thaw_Depth[0] > GrndOutflow_Depth) P = GrndOutflow_Depth - Water_Table;
				else P = Daily_Thaw_Depth[0] - Water_Table;
			}
			else { //No surface permafrost. Outflow occurs below frozen depth 
				if (Water_Table < Daily_Freeze_Depth[0])  P = GrndOutflow_Depth - Daily_Freeze_Depth[0];
				else P = GrndOutflow_Depth - Water_Table;
			}

			if (P > 0) {
				P = P * F_GrndOutflow; //cm

				F2 = Pond * 0.1;  //cm
				F1 = Water_Table;
				for (L = WT_Layers; L < Soil_Layers; L++) {
					F1 += Depth[L];
					if (F1 < GrndOutflow_Depth) F2 = (Soil_Water[L] - Field_Capacity[L]) * Depth[L] * Fsoil[L];  //water avaialble
					else break;
				}
				if (P > F2) P = F2;

				Daily_Outflow += P * 10;  //mm

				if (Pond > P * 10) {
					Pond -= P * 10;
					P = 0;
				}
				else {
					P -= Pond * 0.1;
					Pond = 0;

					for (L = WT_Layers; L < Soil_Layers; L++) {
						if (Tsoil[L] > 0) {
							F = (Soil_Water[L] - Field_Capacity[L]) * Depth[L] * Fsoil[L];
							if (F > P) {
								Soil_Water[L] -= P / (Depth[L] * Fsoil[L]);
								P = 0;
								break;
							}
							else {
								P -= F;
								Soil_Water[L] = Field_Capacity[L];
							}  //else
						}
					}  //for
				}  //else
			}

			Wflux_Layers = WT_Layers + 2;
			if (Wflux_Layers > Soil_Layers - 1) Wflux_Layers = Soil_Layers - 1;

			//-----------------------
			if (Daily_Snowfall > 0.0) {
//				F1 = log(Daily_Vap[DOY] / 6.11);
//				F = 237.3 * F1 / (17.27 - F1);   //dew point
		 	    F = Daily_Tair[DOY];
				if (F > -15)      Fresh_Snow_Density = Fresh_Snow_Density_Min + 1.7 * pow((F + 15), 1.5);
				else              Fresh_Snow_Density = Fresh_Snow_Density_Min;
//	   else if(F>-30) Fresh_Snow_Density = Fresh_Snow_Density_Min * exp(0.05*(15.0+F));
//	   else           Fresh_Snow_Density = Fresh_Snow_Density_Min * 0.472367 * exp(0.02*(30.0+F));    //exp(0.05*(15.0-30)) = 0.472367

//				F = pow(Daily_Vap[DOY] / 1.5, 0.5);
//				Fresh_Snow_Density = Fresh_Snow_Density * F;     

				Fresh_Snow_Density = Fresh_Snow_Density / 1000.0;  //snow density in g/cm3
				if (Fresh_Snow_Density > 0.45) Fresh_Snow_Density = 0.45;

				Fresh_Snow_Depth = Daily_Snowfall * 0.1 / Fresh_Snow_Density;
			}
			else {
				Fresh_Snow_Density = Fresh_Snow_Density_Min / 1000.0;
				Fresh_Snow_Depth = 0.0;
			}

			if (Last_Lu == 0) {
				if(Snow_Depth + Fresh_Snow_Depth>0) Snow_Density[0] = (Snow_Density[0] * Snow_Depth + Fresh_Snow_Depth * Fresh_Snow_Density) / (Snow_Depth + Fresh_Snow_Depth);
				else Snow_Density[0] = Fresh_Snow_Density;
			}
			Snow_Depth += Fresh_Snow_Depth;

			P = Daily_Rainfall + Daily_Snow_Melt * 10.0;  //mm

			if (P > 0) Snow_Compaction_Factor = 1.0;
			else Snow_Compaction_Factor = Snow_Compaction_Factor0;

			if (Snow_Depth > 0 && P > 0.0) {  	//rainfall (mm/day) related compaction
				k = 1;        // + (int)(P*0.1);
		  //	  if(k>Lu) k=Lu;
				for (L = 0; L < k; L++) {
					F = Snow_Density[L] + P * 0.1;  //snow density
					if (F > 0.40) F = 0.40;

					P = (Lu_Depth - dLu_Depth[L]) * (1.0 - Snow_Density[L] / F);
					dLu_Depth[L] += P;
					Snow_Depth -= P;
					Snow_Density[L] = F;
				}
				if (Snow_Depth < 0.0) Snow_Depth = 0.0;
			} //if

		 //wind effects on the top snow layer
			if (Lu > 0) {
				F = 0.003 * Canopy_Wind * exp(15 * (-Snow_Density[0] + 0.04));
				F1 = 1.0 - Snow_Density[0] / (Snow_Density[0] + F);
				P = Lu_Depth * F1;
				dLu_Depth[0] += P;
				Snow_Depth -= P;
				Snow_Density[0] /= (1 - F1);
			}

			if (Snow_Depth > 2 && Snow_Depth < 10.0) {
				Lu = 1;
				Lu_Depth = Snow_Depth;
			}
			else if (Snow_Depth >= 10.0) {
				Lu = (int)(Snow_Depth / 10.0 + 1);
				if (Lu > MaxLu) Lu = MaxLu;
				Lu_Depth = Snow_Depth / Lu;
			}
			else {
				Lu = 0;       //no above ground layers
				Lu_Depth = Snow_Depth;
			}

			if (Lu > 0) {
				if (Last_Lu == 0 && Last_Snow_Depth > 0) {
					Last_Lu = 1;
					dLu_Depth[0] = 0.0;
					Last_Lu_Depth = Last_Snow_Depth;
				}
				Update_Snow_Layers(Lu, Fresh_Snow_Depth);
			}

			Last_Lu = Lu;
			Last_Lu_Depth = Lu_Depth;
			Last_Snow_Depth = Snow_Depth;
			for (L = 0; L < MaxLu; L++) dLu_Depth[L] = 0.0;

			//--------------------------
			for (L = 0; L < Lu; L++) {
				//	   Lu_K[L]=0.000358+0.0264*Snow_Density[L]*Snow_Density[L];  //Yin and Arp, Can.J.Forest Res., 23: 2521-36, 1993
				//	   Lu_K[L]=0.00074 +0.02576*Snow_Density[L]*Snow_Density[L]; //Mellor, 1976.  w/cm/k

				//     Lu_K[L]= (0.024 + 3.0*Snow_Density[L]*Snow_Density[L] - 0.106*Snow_Density[L])/100.0; //Riche & Schneebeli, 2013. The Cryosphere, doi:10.5194/tc-7-217-2013
					   Lu_K[L]= (0.024 + 2.5 * Snow_Density[L] * Snow_Density[L] - 0.123 * Snow_Density[L]) / 100.0; //Calonne et al., 2011, GRL, DOI: 10.1029/2011GL049234

				if (Lu_K[L] < Snow_K_Min) Lu_K[L] = Snow_K_Min;  //0.001:Resolute:0.001, 0.002: MouldBay,Saskatoon, Ottawa, 0.003:GooseA

				Lu_HC[L] = HC_Ice * Snow_Density[L] + Snow_Water[L] * HC_Water;
			}
			if (Lu_K[0] < 0.001)  Lu_K[0] = 0.001;


			//Wapusk: test for Lichen
			// if(Snow_Depth<0.1 && Daily_Precip[DOY]<0.001) {
			//	 Soil_Water[0] *= 0.75;
			//	 if(Soil_Water[0]<Wilting_Point[0]) Soil_Water[0]=Wilting_Point[0];
			// }

			for (L = 0; L < Soil_Layers; L++) {
				HC[L] = Fsoil[L] * (HC_Organic * Organic[L] + HC_Minerals * Mineral[L] + Soil_Water[L] * (HC_Water * Fwater[L] + HC_Ice * Fice[L])) +
					HC_Minerals * Soil_Fstone[L] + (HC_Ice * DxIce[L] + HC_Water * DxWater[L]) / Depth[L];

				/*
				//--YZ original
					if(Soil_Type[L]==0) {   //organic/peat
						F1 = 0.001626 *exp(2.75*Soil_Water[L]);  //frozen
						F2 = 0.000993 *exp(2.15*Soil_Water[L]);  //unfrozen

				//		F1 = 0.001626 *exp(2.75*Soil_Water[L]);  //frozen
				//		F2 = 0.000805 *exp(2.48*Soil_Water[L]);  //unfrozen

						K[L] = (F1*Fice[L] + F2*Fwater[L]) * Fsoil[L] +
								K_Minerals[L]*Soil_Fstone[L] + K_Ice*Soil_Fxice[L];

					}
					else {
					   K[L] = pow(K_Organic,Organic[L]*Fsoil[L]) * pow(K_Minerals[L], Mineral[L]*Fsoil[L]+Soil_Fstone[L]) *
							  pow(K_Water,Soil_Water[L]*Fwater[L]*Fsoil[L]) *
							  pow(K_Ice, Soil_Water[L]*Fice[L]*Ice_Specific_Volume*Fsoil[L]+Soil_Fxice[L]) *
							  pow(K_Air, (Porosity[L]- Soil_Water[L])*Fsoil[L]);
					}
				*/
				//from Yinsuo, case_4: De Vries as Farouki, 1986
				x_a = Porosity[L] - Soil_Water[L];
				x_w = Soil_Water[L] * Fwater[L];
				x_s = (1.0 - Porosity[L]) * Fsoil[L];
				x_i = Soil_Water[L] * (1 - Fwater[L]);
				Solid = Organic[L] + Mineral[L];

				if (Solid > 0) lam_s = pow(K_Organic * 100, Organic[L] * Fsoil[L] / Solid) * pow(K_Minerals[L] * 100, (Mineral[L] * Fsoil[L] + Soil_Fstone[L]) / Solid);
				else lam_s = 0;

				lam_i = K_Ice * 100;
				lam_w = K_Water * 100;

				lam_a = K_Air * 100 + 0.0238 * exp(0.0536 * Tsoil[L]); //estimated from fig. 91 in Farouki 1986.
				g_a = 0.333 - x_a / (Porosity[L] + 1.0e-10) * (0.333 - 0.035);

				//      if (x_w>0.09) {
				//			lam_a =  K_Air*100+0.0238*exp(0.0536*Tsoil[L]); //estimated from fig. 91 in Farouki 1986.
				//			g_a = 0.333- x_a/(Porosity[L]+1.0e-10)*(0.333-0.035);	
				//	   }
				//       else  {
				//			lam_a = (0.0615+1.96*x_w)*0.418; //from Farouki 1986 pp110.
				//			g_a = 0.013+0.944*x_w;
				//	   }

				g_c = 1.0 - 2.0 * g_a;
				F_a = 1.0 / 3.0 * (2.0 / (1.0 + (lam_a / lam_w - 1.0) * g_a) + 1.0 / (1.0 + (lam_a / lam_w - 1.0) * g_c));
				F_s = 1.0 / 3.0 * (2.0 / (1.0 + (lam_s / lam_w - 1.0) * 0.125) + 1.0 / (1.0 + (lam_s / lam_w - 1.0) * 0.75));
				F_i = 1.0 / 3.0 * (2.0 / (1.0 + (lam_i / lam_w - 1.0) * 0.125) + 1.0 / (1.0 + (lam_i / lam_w - 1.0) * 0.75));

				F = (x_w * lam_w + F_a * x_a * lam_a + F_s * x_s * lam_s + F_i * x_i * lam_i) / (x_w + F_a * x_a + F_s * x_s + F_i * x_i) * 0.01;

				K[L] = pow(F, Fsoil[L]) * pow(K_Water, DxWater[L] / Depth[L]) * pow(K_Ice, DxIce[L] / Depth[L]) * pow(K_Minerals[L], Soil_Fstone[L]) * pow(K_Air, DxAir[L] / Depth[L]);
			}

			for (L = Soil_Layers; L < Total_Layers; L++) {
				HC[L] = HC_Minerals;
				K[L] = K_Minerals[L];
			}

			if (Lu == 1) {
				Lu_A0 = 2 * Lu_K[0] / Lu_Depth;
				Lu_A[0] = (Lu_K[0] + K[0]) / (Lu_Depth + Depth[0]);

				Lu_Fac0 = (Lu_A0 + Lu_A[0]) / (Lu_HC[0] * Lu_Depth) * TimeStep;
				if (Lu_Fac0 > 1) Lu_Fac0 = 1;
			}
			else if (Lu == 2) {
				Lu_A0 = 2 * Lu_K[0] / Lu_Depth;
				Lu_A[0] = (Lu_K[0] + Lu_K[1]) / (2 * Lu_Depth);
				Lu_A[1] = (Lu_K[1] + K[0]) / (Lu_Depth + Depth[0]);

				Lu_Fac0 = (Lu_A0 + Lu_A[0]) / (Lu_HC[0] * Lu_Depth) * TimeStep;
				if (Lu_Fac0 > 1) Lu_Fac0 = 1;
				Lu_Fac[0] = (Lu_A[0] + Lu_A[1]) / (Lu_HC[0] * Lu_Depth) * TimeStep;
				if (Lu_Fac[0] > 1) Lu_Fac[0] = 1;
			}
			else if (Lu > 2) {
				Lu_A0 = 2 * Lu_K[0] / Lu_Depth;
				for (L = 0; L < Lu - 1; L++)  Lu_A[L] = (Lu_K[L] + Lu_K[L + 1]) / (Lu_Depth * 2);
				Lu_A[Lu - 1] = (Lu_K[Lu - 1] + K[0]) / (Lu_Depth + Depth[0]);

				Lu_Fac0 = (Lu_A0 + Lu_A[0]) / (Lu_HC[0] * Lu_Depth) * TimeStep;
				if (Lu_Fac0 > 1) Lu_Fac0 = 1;

				for (L = 0; L < Lu - 1; L++) {
					Lu_Fac[L] = (Lu_A[L] + Lu_A[L + 1]) / (Lu_HC[L] * Lu_Depth) * TimeStep;
					if (Lu_Fac[L] > 1) Lu_Fac[L] = 1;
				}
			}

			//soil layers
			A0 = 2 * K[0] / Depth[0];
			J = Total_Layers - 1;
			for (L = 0; L < J; L++) A[L] = (K[L] + K[L + 1]) / (Depth[L] + Depth[L + 1]);

			if (Lu > 0) Fac[0] = (Lu_A[Lu - 1] + A[0]) / (HC[0] * Depth[0]) * TimeStep;
			else     Fac[0] = (A0 + A[0]) / (HC[0] * Depth[0]) * TimeStep;

			if (Fac[0] > 1) Fac[0] = 1;

			for (L = 1; L < Soil_Layers; L++) {
				Fac[L] = (A[L - 1] + A[L]) / (HC[L] * Depth[L]) * TimeStep;
				if (Fac[L] > 1) Fac[L] = 1;
			}

			J = Total_Layers - 1;
			for (L = Soil_Layers; L < J; L++) {
				Fac[L] = (A[L - 1] + A[L]) / (HC[L] * Depth[L]) * TimeStep;
				if (Fac[L] > 1) Fac[L] = 1;
			}
			Fac[L] = A[L - 1] / (HC[L] * Depth[L]) * TimeStep;  //Total_Layers-1
			if (Fac[L] > 1) Fac[L] = 1;


			Daily_ETplant = 0.0;
			Daily_ET_Soil = 0;
			Daily_Evap = 0.0;
			SnowPond_Evap = 0.0;
			Daily_Tcanopy = 0.0;
			Daily_Tsurface = 0.0;
			Daily_Snow_Melt = 0.0;            //daily
			Daily_Rainfall = 0.0;
			Daily_Snowfall = 0.0;
			LHeat = 0.0;  //latentheat MJ/day

			SWrootzone = 0.0;
			FCrootzone = 0.0;
			for (L = 0; L < 5; L++) {
				if (Tsoil[L] > 0.0) {
					if (Soil_Water[L] > Wilting_Point[L]) SWrootzone += (Soil_Water[L] - Wilting_Point[L]) * Depth[L] * Fsoil[L] * Root_Frac[L];
					FCrootzone += (Field_Capacity[L] - Wilting_Point[L]) * Depth[L] * Root_Frac[L];  //not *Fsoil[L]
				}
			}

			ET_Layers = Thaw_Layers;
			if (ET_Layers > 5) ET_Layers = 5;  //Root layer 
			if (Wflux_Layers < ET_Layers)     Wflux_Layers = ET_Layers;

			if (Snow_Depth < 5) {
				F = Snow_Depth * 0.2;  //snowcover fraction
				Surf_Albedo = (1.0 - F) * Land_Albedo + F * 0.2; //0.2: snow albedo when depth is 5cm    
			}
			else {
				F = 0.16 + 110 * pow(Snow_Density[0], 4);  //grain size diameter (mm)
				Surf_Albedo = 1 - 0.206 * 1.77 * sqrt(F);
				if (Surf_Albedo < 0.15) Surf_Albedo = 0.15;
			}

			//  Precip_Time = int((double)rand()/RAND_MAX*86400.0/TimeStep)*TimeStep;
			Precip_Time = 0;  //No effects, Using Daily average Tair check snow or rainfall
			Tmin0 = Daily_Tair[DOY] - 0.5 * Daily_DTair[DOY];
			Tmax = Daily_Tair[DOY] + 0.5 * Daily_DTair[DOY];
			if (DOY == 364 + DAYS[1] - 28) Tmin1 = Tmin0;
			else Tmin1 = Daily_Tair[DOY + 1] - 0.5 * Daily_DTair[DOY + 1];

			Sun_Rise = 12 - Day_Length[DOY] / 2.0;
			Sun_Set = 12 + Day_Length[DOY] / 2.0;

			Rad_avg = Daily_Solar_Rad[DOY] * 1000000.0 / (Day_Length[DOY] * 60.0 * 60.0);  //daily mean solar radiation
			F = Rad_avg / sFactors[DOY][3];  //ratio between daily solar radiation on a horizontal surface to at top of Atm
			if (F > 0.9)    F = 0.9;
			else if (F < 0) F = 0.0;

			if (F <= 0.175) DayF_Diffuse = 1.0;
			else {
				DayF_Diffuse = 1.05 / (1 + exp(-4.5 + 8.6 * F));
				if (DayF_Diffuse > 0.99) DayF_Diffuse = 0.99;
			}

			Emissivity_Air = Daily_Emissivity_Air[DOY]; //for down-ward longwave radiation based on Tair
			Emissivity_Canopy = Emissivity_Veg;   //*ExpLAI+(1-ExpLAI)*Emissivity_Air;   

			Tsunset_Yesterday = Temperature_Hourly(Day_Length[DOY], Tsunset_Yesterday, Tmax, Tmin0, Tmin1, Tair_Hr);

			F = log(Daily_Vap[DOY] / 6.11);
			Tdew = 237.3 * F / (17.27 - F);

//  if (IF_Ini==0 && Calendar_Year == 2010 && Month==2 && Day >= 18)
//	   F = F;
			 //-----------------Begin time loop --------------------------------
			for (I = 0; I < N_Steps; I++) {    //24*60=1440, Noon: 720

				Time = I * TimeStep / 3600.0;

				Tair = Tair_Hr[I];
				//no slope and viewshed effects
				//  Rad_S = 0;
				//  if(Day_Length[DOY]>0) {
				//	  if(Time>=Sun_Rise && Time<= Sun_Set) {
				//		  Rad_S = Daily_Solar_Rad[DOY]*1000000.0/Day_Length[DOY]/(60*60.0)*0.5*Pi    //J/m2/s
				//				  * sin(Pi/Day_Length[DOY]*(Time-12+0.5*Day_Length[DOY]) );  
				//	  }
				//  }
				//  Rad_S1 = Rad_S;

				Rad_Direct = 0;
				Rad_Diffuse = 0;
				ExpLAI_Sun = 0;
				if (Day_Length[DOY] > 0) {
					if (Time >= Sun_Rise && Time <= Sun_Set) {

						Slope_Solar_Rad(DOY, Time, sin_Lat, cos_Lat, cos_Slope, sin_Slope, cos_sFactors0[DOY],
							DayF_Diffuse, Rad_avg, Slope, Aspect, ViewShed, Diffuse_Rad_F, sFactors[DOY], Mid);

						Rad_Direct = Mid[0];
						Rad_Diffuse = Mid[1];
						sin_Alt = Mid[2];
						if (sin_Alt < 0.001) ExpLAI_Sun = 0;
						else              ExpLAI_Sun = exp(-Ext_K * LAI_All * Omega / sin_Alt);
					}  //if
					else {
						Rad_Direct = 0;
						Rad_Diffuse = 0;
						ExpLAI_Sun = 0;
					}
				}  //if
				Rad_S = Rad_Direct + Rad_Diffuse;

				Rn_Canopy = (1.0 - Albedo) * (Rad_Direct * (1.0 - ExpLAI_Sun) + Rad_Diffuse * (1.0 - ExpLAI));
				Rn_Surface = (1.0 - Surf_Albedo) * (Rad_Direct * ExpLAI_Sun + Rad_Diffuse * ExpLAI);

				F = 6.11 * exp(17.27 * Tair / (Tair + 237.3));
				if (Tair < 0.0) F = F * (1.0 + 0.00972 * Tair + 0.000042 * Tair * Tair); //Monthly weather reviw, vol.92, No.11, p508	  
				VPD = (F - Daily_Vap[DOY]);         //mb  *0.3 added for test
				if (VPD < 0.0) VPD = 0.0;

				if (Daily_Precip[DOY] > 0.0 && I == Precip_Time) {
					//--- rainfall 
					F1 = Daily_Precip[DOY] * (1 - Daily_Snowfall_Frac[DOY]);
					Daily_Rainfall += F1;
					Intercept_Rain += F1;   //mm
					F = Daily_LAI[DOY] * 0.3;
					if (Intercept_Rain > F) {
						Ground_Rainfall += Intercept_Rain - F;  //water falling to ground
						Intercept_Rain = F;
						//rainfall related snow melt
						if (Lu > 0) {
							P1 = Tmax;
							if (P1 < 1) P1 = 1.0;
							P = 0.0125 * F1 * 0.1 / Snow_Density[0] * P1;  //cm
							if (P > Last_Lu_Depth - dLu_Depth[0]) P = Last_Lu_Depth - dLu_Depth[0];

							dLu_Depth[0] += P;
							Snow_Depth -= P;
							Daily_Snow_Melt += 0.0125 * F1 * 0.1; //cm water
						}
					}
					else Ground_Rainfall += 0.0;  //no rain falling to ground

			//snowfall
					Intercept_Snow += Daily_Precip[DOY] * Daily_Snowfall_Frac[DOY] * (1 - SnowDrift_F);
					if (Intercept_Snow > F) {
						Daily_Snowfall += (Intercept_Snow - F);  //mm on the ground
						Intercept_Snow = F;
					}
				}
				else Ground_Rainfall += 0.0;

				//aerodynamic resistance (surface)

				ra_surface = LN2_Surface / (0.16 * Canopy_Wind);
				P = 5.0 * 9.8 * 0.5 * (Tsurface - Tcanopy) / ((Tcanopy + 273.15) * Canopy_Wind * Canopy_Wind);
				if (P < -0.8) P = -0.8;

				if (P > 0) F = pow((1.0 + P), -0.75);
				else    F = pow((1.0 + P), -2.0);
				if (F > 2) F = 2;
				else if (F < 0.2) F = 0.2;

				ra_surface = ra_surface * F;  //resistance s/m

			   //aerodynamic resistance (canopy)
				ra_canopy = LN2_Canopy / (0.16 * Daily_Wind[DOY]);
				P = 5.0 * 9.8 * 0.5 * Height1 * (Tcanopy - Tair) / ((Tair + 273.15) * Daily_Wind[DOY] * Daily_Wind[DOY]);
				if (P < -0.8) P = -0.8;

				if (P > 0) F = pow((1.0 + P), -0.75);
				else    F = pow((1.0 + P), -2.0);              //resistance s/m
				if (F > 2) F = 2;
				else if (F < 0.2) F = 0.2;
				ra_canopy = ra_canopy * F;

				//canopy ET
				F = Intercept_Rain + Intercept_Snow;
				rc(Landcover, C_Leaf_max, Daily_LAI[DOY], LE_Surface_Dew, F, Snow_Depth, Pond, Tsoil[0],
					Soil_Water[0] * Fsoil[0], Porosity[0], SWrootzone, FCrootzone,
					Rad_S, Tair, Tmin0, VPD, Mid);      //Mid[0]:stomata resistance, Mid[1]:soil-surface resistance

				if (Rn_Canopy > 0.01) LE_Canopy = Penman_ET(Tcanopy, VPD, Rn_Canopy, ra_canopy, Mid[0]);
				else LE_Canopy = 0.0;
				if (Intercept_Snow > 0) Lamda = Lamda2;
				else Lamda = Lamda1;

				ETplant = LE_Canopy * TimeStep / Lamda;      //kg/m2 = mm, 
				ET_Soil = ETplant;

				LHeat += LE_Canopy * TimeStep * 1.0e-6;  //MJ

				if (Intercept_Snow > ET_Soil) {
					Intercept_Snow -= ET_Soil;
					ET_Soil = 0.0;
				}
				else {
					ET_Soil -= Intercept_Snow;
					Intercept_Snow = 0.0;

					if (Intercept_Rain > ET_Soil) {
						Intercept_Rain -= ET_Soil;
						ET_Soil = 0.0;
					}
					else {
						ET_Soil -= Intercept_Rain;
						Intercept_Rain = 0.0;
					}
				}

				if (ET_Soil > Pond) {
					ET_Soil -= Pond;
					Pond = 0;
				}
				else {
					Pond -= ET_Soil;
					ET_Soil = 0;
				}

				//checking water avaliability
				if (ET_Soil > 0) {
					F = 0;
					for (i = 0; i < ET_Layers; i++) {
						if (Soil_Water[i] > Wilting_Point[i]) F += (Soil_Water[i] - Wilting_Point[i]) * Depth[i] * Fsoil[i] * 10.0 * 0.5 * Root_Frac[i]; //mm, assuming only half can be absorbed in a time interval
					}
					if (ET_Soil > F) {  //soil water limits
						F = ET_Soil - F;
						LE_Canopy -= F * Lamda1 / TimeStep;
						LHeat -= F * Lamda1 * 1.0e-6; //mj
						ET_Soil -= F;
						ETplant -= F;
					}
				}

				//surface evaporation
				F = Rn_Surface - G_Flux;
				if (Rn_Surface > 0) {   //no ET when Rn_Surface=0 
					LE_Surface = Penman_ET(Tsurface, VPD, F, ra_surface, Mid[1]);       //W/s
					if (LE_Surface < 0.0) LE_Surface = 0.0;

					if (Snow_Depth < 0.1 || Pond < 1.0) LE_Surface *= Fsoil[0];
				}
				else  LE_Surface = 0.0;

				//	 if (LE_Surface_Dew > LE_Surface) {
				//		   LE_Surface_Dew -= LE_Surface;
				//		   LE_Surface = 0;
				//	 }
				//	 else {
				//		   LE_Surface -= LE_Surface_Dew;
				//		   LE_Surface_Dew = 0;
				//	 }

				if (Snow_Depth > 0) Lamda = Lamda2;
				else Lamda = Lamda1;

				Evap = LE_Surface * TimeStep / Lamda;   //Evp from surface kg/m2 = mm
				LHeat += LE_Surface * TimeStep * 1.0e-6;

				//checking water avaliability
				P = Snow_Depth * Snow_Density[0] * 10.0 + Pond;
				if (P < Evap) {
					F = Pond;
					for (i = 0; i < 1; i++) {
						if (Soil_Water[i] > Wilting_Point[i]) F += (Soil_Water[i] - Wilting_Point[i]) * Depth[i] * Fsoil[i] * 10.0 * 0.5; //mm Only half can be lost in a time interval
					}
					if (Evap > F) {  //soil water limits
						F = Evap - F;
						LE_Surface -= F * Lamda / TimeStep;
						Evap -= F;
						LHeat -= F * Lamda * 1.0e-6; //mj
					}
					SnowPond_Evap = P;  //all evaporated
					Evap = Evap - P;
				}
				else {  //Snow and pond > Evap
					SnowPond_Evap += Evap;
					Evap = 0.0;    //no evap from soil layers
				}

				Daily_ETplant += ETplant;
				Daily_ET_Soil += ET_Soil;
				Daily_Evap += Evap;

				if (Daily_LAI[DOY] < 0.01) Tcanopy = Tair;  //no canopy
				else {
					P5 = Tsurface + 273.15;
					if (Snow_Depth > 0) P5 = Stefen_Boltzmann * Emissivity_Snow * P5 * P5 * P5 * P5;   //surface long wave radiation
					else  P5 = Stefen_Boltzmann * Emissivity_Soil * P5 * P5 * P5 * P5;

					P4 = Tair + 273.15;
					P4 = Stefen_Boltzmann * Emissivity_Air * P4 * P4 * P4 * P4;  //sky long wave radiation

					F1 = Rn_Canopy - LE_Canopy + (P5 + P4) * (1 - ExpLAI);

					P3 = Tcanopy + 273.15;
					P3 = Stefen_Boltzmann * Emissivity_Canopy * P3 * P3 * P3 * P3;  //canopy long wave radiation
					F2 = 1020.0 / ra_surface * (Tcanopy - Tsurface) + 1020.0 / ra_canopy * (Tcanopy - Tair) + 2 * P3 * (1 - ExpLAI);
					i = 1;
					while (i < 30 && fabs(F2 - F1)> 1.0) {
						dT = 0.01 * (F2 - F1);
						if (dT > 1) dT = 1;
						else if (dT < -1) dT = -1;
						Tcanopy -= dT;

						P3 = Tcanopy + 273.15;
						P3 = Stefen_Boltzmann * Emissivity_Canopy * P3 * P3 * P3 * P3;
						F2 = 1020.0 / ra_surface * (Tcanopy - Tsurface) + 1020.0 / ra_canopy * (Tcanopy - Tair) + 2 * P3 * (1 - ExpLAI);
						i++;
					}
					if (Tcanopy < Tdew && Tdew > 0) {
						Tcanopy = (Tcanopy + Tdew) / 2.0;
						if (Tcanopy < 0) Tcanopy = 0;
					}
				}  //else

				if (Lu > 0) {   //snow surface temperature
					P2 = Lu_K[0] / (0.5 * Lu_Depth) * 1.0e4;

					P3 = Tcanopy + 273.15;
					P3 = Stefen_Boltzmann * Emissivity_Canopy * P3 * P3 * P3 * P3;  //canopy long wave radiation

					P4 = Tair + 273.15;
					P4 = Stefen_Boltzmann * Emissivity_Air * P4 * P4 * P4 * P4;  //sky long wave radiation

					F1 = Rn_Surface - LE_Surface + P3 * (1.0 - ExpLAI) + P4 * ExpLAI;

					P5 = Tsurface + 273.15;
					P5 = Stefen_Boltzmann * Emissivity_Snow * P5 * P5 * P5 * P5;   //surface long wave radiation
				 // F2=1020.0/ra_surface*(Tsurface-Tcanopy) + P2*(Tsurface-Tsnow[0]) + P5;
					F2 = 1020.0 * ((Tsurface - Tcanopy) / ra_surface * (1 - ExpLAI) + (Tsurface - Tair) / ra_canopy * ExpLAI)
						+ P2 * (Tsurface - Tsnow[0]) + P5;
					i = 1;
					while (i < 30 && fabs(F2 - F1)> 1.0) {
						dT = (F2 - F1) * 0.01;
						if (dT > 1) dT = 1;
						else if (dT < -1) dT = -1;
						Tsurface -= dT;

						P5 = Tsurface + 273.15;
						P5 = Stefen_Boltzmann * Emissivity_Snow * P5 * P5 * P5 * P5;
						F2 = 1020.0 * ((Tsurface - Tcanopy) / ra_surface * (1 - ExpLAI) + (Tsurface - Tair) / ra_canopy * ExpLAI)
							+ P2 * (Tsurface - Tsnow[0]) + P5;
						//	   F2=1020/ra_surface*(Tsurface-Tcanopy) + P2*(Tsurface-Tsnow[0]) + P5;
						i++;
					}

					if (Tsurface < Tdew && Tdew > 0) {
						Tsurface = (Tsurface + Tdew) / 2.0;
						if (Tsurface < 0) Tsurface = 0;
						//	   P5 = Tsurface + 273.15;
						//	   P5 = Stefen_Boltzmann * Emissivity_Soil * P5 * P5 * P5 * P5;   //surface long wave radiation
						//	   F2 = 1020.0 * ((Tsurface - Tcanopy) / ra_surface * (1 - ExpLAI) + (Tsurface - Tair) / ra_canopy * ExpLAI)
						//		   + P2 * (Tsurface - Tsnow[0]) + P5;
						//	   LE_Surface_Dew += F2 - F1;           //in W/m2
					}

					//   P5= Tsurface + 273.15;
					//   P5=Stefen_Boltzmann*Emissivity_Snow*P5*P5*P5*P5;
					//   RLongWave=P3*(1.0-ExpLAI)+P5*ExpLAI-P4;   //above canopy

					//-------Method 1 
					if (Lu == 1) {    //only one snow layer
						Teq = (Lu_A0 * Tsurface + Lu_A[0] * Tsoil[0]) / (Lu_A0 + Lu_A[0]);
						//	   Fac = (Lu_A0+Lu_A[0])/(Lu_HC[0]*Lu_Depth)*TimeStep;
						Snow_Temp[0] = Lu_Fac0 * Teq + (1 - Lu_Fac0) * Tsnow[0];
					}
					else {  //more than one snow layer
						Teq = (Lu_A0 * Tsurface + Lu_A[0] * Tsnow[1]) / (Lu_A0 + Lu_A[0]);
						//	   Fac = (Lu_A0+Lu_A[0])/(Lu_HC[0]*Lu_Depth)*TimeStep;
						Snow_Temp[0] = Lu_Fac0 * Teq + (1 - Lu_Fac0) * Tsnow[0];
					}

					if (Snow_Depth > 0.0 && Tsnow[0] > 0.0) {
						P = Snow_Temp[0] * Lu_HC[0] * Lu_Depth;
						F1 = Water_Ice * Snow_Depth * Snow_Density[0];    //heat need for melting
						if (P > F1) {                  //all melted
							Snow_Melt = Snow_Depth * Snow_Density[0];  //cm
						}
						else {   //not all melt
							Snow_Melt = P / Water_Ice;  //snow_melt in cm water
						}
						Snow_Temp[0] = 0.0;

						P = Snow_Melt / Snow_Density[0];
						if (P > Snow_Depth) P = Snow_Depth;

						dLu_Depth[0] += P;
						Snow_Depth -= P;
						Daily_Snow_Melt += P * Snow_Density[0];
					}
					else {
						Snow_Melt = 0.0;
					}

					F1 = Lu_K[0] * (Tsurface - 0.5 * (Snow_Temp[0] + Tsnow[0])) / (0.5 * Lu_Depth);
					G_Flux = F1 * 10000.0;  //W/m2
				 //---------------
					if (Lu >= 3) {             //above soil layers
						for (L = 1; L < Lu - 1; L++) {
							Teq = (Lu_A[L - 1] * Tsnow[L - 1] + Lu_A[L] * Tsnow[L + 1]) / (Lu_A[L - 1] + Lu_A[L]);
							//		 Fac = (Lu_A[L-1]+Lu_A[L])/(Lu_HC[L]*Lu_Depth)*TimeStep;
							Snow_Temp[L] = Lu_Fac[L - 1] * Teq + (1 - Lu_Fac[L - 1]) * Tsnow[L];
						}
					}

					if (Lu >= 2) {     //The last snow layer (two snow layers, one soil layer)
						L = Lu - 1;
						Teq = (Lu_A[L - 1] * Tsnow[L - 1] + Lu_A[L] * Tsoil[0]) / (Lu_A[L - 1] + Lu_A[L]);
						//		Fac = (Lu_A[L-1]+Lu_A[L])/(Lu_HC[L]*Lu_Depth)*TimeStep;
						Snow_Temp[L] = Lu_Fac[L - 1] * Teq + (1 - Lu_Fac[L - 1]) * Tsnow[L];
					}

					//snow melt from the bottom
					if (Snow_Temp[Lu - 1] > 0.0) {
						P = Snow_Temp[Lu - 1] * Lu_HC[Lu - 1] * Lu_Depth;  //
						F1 = Water_Ice * Snow_Depth * Snow_Density[Lu - 1];    //heat need for melting
						if (P > F1) F2 = Snow_Depth * Snow_Density[Lu - 1];  //all melted
						else F2 = P / Water_Ice;  //not all melt, snow_melt in cm water
						Snow_Temp[Lu - 1] = 0.0;

						P = F2 / Snow_Density[Lu - 1];
						if (P > Snow_Depth) P = Snow_Depth;
						Snow_Depth -= P;
						dLu_Depth[Lu - 1] += P;
						Daily_Snow_Melt += P * Snow_Density[Lu - 1];
					}
					else if (Lu_Fac[Lu - 1] > 1) Snow_Temp[Lu - 1] = Teq;

					if (Lu > 0) {
						//One snow layer, two soil layers
						Teq = (Lu_A[Lu - 1] * Tsnow[Lu - 1] + A[0] * Tsoil[1]) / (Lu_A[Lu - 1] + A[0]);

						//		Fac = (Lu_A[Lu-1]+A[0])/(HC[0]*Depth[0])*TimeStep;
						Soil_Temp[0] = Fac[0] * Teq + (1 - Fac[0]) * Tsoil[0];

						if (Fice[0] > 0 || Soil_Temp[0] < 0) {
							APT_Thawing_Freezing(Soil_Temp[0], Tsoil[0], Fice[0], Soil_Water[0] * Fsoil[0], HC[0], Mid);
							Soil_Temp[0] = Mid[0];
							Fice[0] = Mid[1];
							Fwater[0] = 1.0 - Mid[1];
						}
						//		T[0]=Soil_Temp[0];
					}

					//********************************  Snow Density Change
					F = 0;                   //top layer
					for (L = 0; L < Lu; L++) {               //snow density change  
						F1 = 0.01 * exp(-0.08 * (-Tsnow[L])) * F * exp(-21.0 * Snow_Density[L]); //0.01, 0.08, 21 compaction
						F2 = 0.001 * exp(-0.04 * (-Tsnow[L]));                 //0.001, -0.04, destructive metamorphism
						if (Snow_Density[L] > 0.2) F2 *= exp(-46.0 * (Snow_Density[L] - 0.2)); //0.2 threshold snow density for destructive metamophism decreases
						P2 = Snow_Compaction_Factor * (F1 + F2) * TimeStep / 3600.0;                               //fraction increase
						Snow_Density[L] += Snow_Density[L] * P2;

						P4 = (Lu_Depth - dLu_Depth[L]) * P2 / (1.0 + P2);  //reduction in layer thickness cm
						dLu_Depth[L] += P4;
						Snow_Depth -= P4;

						F += Lu_Depth * Snow_Density[L] * 10.0;  //mm water or g
					}
				}        //if Lu>0
				else {        //Lu==0, No snow, soil surface temperature ******************
					P2 = K[0] / (0.5 * Depth[0]) * 1.0e4;

					P3 = Tcanopy + 273.15;
					P3 = Stefen_Boltzmann * Emissivity_Canopy * P3 * P3 * P3 * P3;   //canopy long wave

					P4 = Tair + 273.15;
					P4 = Stefen_Boltzmann * Emissivity_Air * P4 * P4 * P4 * P4;     //sky long wave

					F1 = Rn_Surface - LE_Surface + P3 * (1.0 - ExpLAI) + P4 * ExpLAI;

					P5 = Tsurface + 273.15;
					P5 = Stefen_Boltzmann * Emissivity_Soil * P5 * P5 * P5 * P5;   //surface long wave radiation
				 //   F2=1020.0/ra_surface*(Tsurface-Tcanopy) + P2*(Tsurface-T[0]) + P5;
					F2 = 1020.0 * ((Tsurface - Tcanopy) / ra_surface * (1 - ExpLAI) + (Tsurface - Tair) / ra_canopy * ExpLAI)
						+ P2 * (Tsurface - Tsoil[0]) + P5;

					i = 1;
					while (i < 30 && fabs(F2 - F1)> 1.0) {
						dT = (F2 - F1) * 0.01;
						if (dT > 1) dT = 1;
						else if (dT < -1) dT = -1;
						Tsurface -= dT;

						P5 = Tsurface + 273.15;
						P5 = Stefen_Boltzmann * Emissivity_Soil * P5 * P5 * P5 * P5;   //surface long wave radiation
						F2 = 1020.0 * ((Tsurface - Tcanopy) / ra_surface * (1 - ExpLAI) + (Tsurface - Tair) / ra_canopy * ExpLAI)
							+ P2 * (Tsurface - Tsoil[0]) + P5;
						//	   F2=1020.0/ra_surface*(Tsurface-Tcanopy) + P2*(Tsurface-T[0]) + P5;
						i++;
					}

					if (Tsurface < Tdew && Tdew > 0) {
						Tsurface = (Tsurface + Tdew) / 2.0;
						if (Tsurface < 0) Tsurface = 0;
						//	   P5 = Tsurface + 273.15;
						//	   P5 = Stefen_Boltzmann * Emissivity_Soil * P5 * P5 * P5 * P5;   //surface long wave radiation
						//	   F2 = 1020.0 * ((Tsurface - Tcanopy) / ra_surface * (1 - ExpLAI) + (Tsurface - Tair) / ra_canopy * ExpLAI)
						//		   + P2 * (Tsurface - Tsnow[0]) + P5;
						//	   LE_Surface_Dew += F2 - F1;           //in W/m2
					}

					//   P5= Tsurface + 273.15;
					//   P5=Stefen_Boltzmann*Emissivity_Soil*P5*P5*P5*P5;   //surface long wave radiation
					//   RLongWave=P3*(1.0-ExpLAI)+P5*ExpLAI-P4;   //above canopy

					Teq = (A0 * Tsurface + A[0] * Tsoil[1]) / (A0 + A[0]);
					//   Fac = (A0+A[0])/(HC[0]*Depth[0])*TimeStep;  
					Soil_Temp[0] = Fac[0] * Teq + (1 - Fac[0]) * Tsoil[0];

					if (Snow_Depth > 0.0 && Soil_Temp[0] > 0) {  //snow melt
						F = HC[0] * Depth[0] * (Soil_Temp[0] - 0);   //heat available
						F2 = Water_Ice * (Snow_Depth * Snow_Density[0] - Daily_Snow_Melt);
						if (F2 > 0) {
							if (F > F2) {  //all melted
								Snow_Melt = F2 / Water_Ice;
								Soil_Temp[0] -= F2 / (HC[0] * Depth[0]);
							}
							else {   //not all melt
								Snow_Melt = F / Water_Ice;   //cm
								Soil_Temp[0] = 0.0;
							}
						}
						P = Snow_Melt / Snow_Density[0];
						if (P > Snow_Depth) P = Snow_Depth;
						dLu_Depth[0] += P;
						Snow_Depth -= P;
						Daily_Snow_Melt += P * Snow_Density[0];
					}

					if (Fice[0] > 0 || Soil_Temp[0] < 0) {
						APT_Thawing_Freezing(Soil_Temp[0], Tsoil[0], Fice[0], Soil_Water[0] * Fsoil[0], HC[0], Mid);
						Soil_Temp[0] = Mid[0];
						Fice[0] = Mid[1];
						Fwater[0] = 1.0 - Mid[1];
					}
				}      //end of else (Lu<=0) ****************************

				SHeat_Canopy = 1020.0 / ra_canopy * (Tcanopy - Tair) * 0.5;
				SHeat_Surface = 1020.0 / (ra_surface + ra_canopy) * (Tsurface - Tair);

				//soil layers
				for (L = 1; L < Soil_Layers; L++) {  //Total_Layer-1
					Teq = (A[L - 1] * Tsoil[L - 1] + A[L] * Tsoil[L + 1]) / (A[L - 1] + A[L]);
					//     Fac = (A[L-1]+A[L])/(HC[L]*Depth[L])*TimeStep;  
					Soil_Temp[L] = Fac[L] * Teq + (1 - Fac[L]) * Tsoil[L];

					//	 if(Soil_Fxice[L]>0) {
					F = Soil_Temp[L] * HC[L] * Depth[L];
					if (F > 0 && DxIce[L] > 0) {         //metling excess ice
						if (F >= DxIce[L] * Water_Ice) {   //all melt
							Soil_Temp[L] = (F - DxIce[L] * Water_Ice) / (HC[L] * Depth[L]);
							DxWater[L] += DxIce[L];
							DxIce[L] = 0;
						}
						else {  //melt a part
							Soil_Temp[L] = 0;
							DxWater[L] += F / Water_Ice;
							DxIce[L] -= F / Water_Ice;
						}  //else
					}  //if
					if (F < 0 && DxWater[L]>0) {   //freezing excess water
						if (-F >= DxWater[L] * Water_Ice) {   //all frozen
							Soil_Temp[L] = (F + DxWater[L] * Water_Ice) / (HC[L] * Depth[L]);
							DxIce[L] += DxWater[L];
							DxWater[L] = 0;
						}
						else {  //frozen a part
							Soil_Temp[L] = 0;
							DxWater[L] += F / Water_Ice;  //F<0
							DxIce[L] -= F / Water_Ice;
						}  //else
					}  //if
		   //	 }  //if Soil_Fxice[L]>0

					if (Fice[L] > 0 || Soil_Temp[L] < 0) {
						APT_Thawing_Freezing(Soil_Temp[L], Tsoil[L], Fice[L], Soil_Water[L] * Fsoil[L], HC[L], Mid);
						Soil_Temp[L] = Mid[0];
						Fice[L] = Mid[1];
						Fwater[L] = 1.0 - Mid[1];
						//--------------------------------
						F = Soil_Temp[L] * HC[L] * Depth[L];
						if (F > 0 && DxIce[L] > 0) {
							if (F >= DxIce[L] * Water_Ice) {   //all melt
								Soil_Temp[L] = (F - DxIce[L] * Water_Ice) / (HC[L] * Depth[L]);
								DxWater[L] += DxIce[L];
								DxIce[L] = 0;
							}
							else {  //melt a part
								Soil_Temp[L] = 0;
								DxWater[L] += F / Water_Ice;
								DxIce[L] -= F / Water_Ice;
							}  //else
						}

						if (F < 0 && DxWater[L]>0) {   //freezing excess water
							if (-F >= DxWater[L] * Water_Ice) {   //all frozen
								Tsoil[L] = Soil_Temp[L];

								Soil_Temp[L] = (F + DxWater[L] * Water_Ice) / (HC[L] * Depth[L]);
								DxIce[L] += DxWater[L];
								DxWater[L] = 0;

								APT_Thawing_Freezing(Soil_Temp[L], Tsoil[L], Fice[L], Soil_Water[L] * Fsoil[L], HC[L], Mid);
								Soil_Temp[L] = Mid[0];
								Fice[L] = Mid[1];
								Fwater[L] = 1.0 - Mid[1];
							}
							/*
							else {  //frozen some of the excess water, Fice still > 0, complicated
								Soil_Temp[L] = 0;
								DxWater[L] += F / Water_Ice;  //F<0
								DxIce[L] -= F / Water_Ice;
							}  //else
							*/
						}  //if
					}  //IF
				} // end of soil layer loop L

				if (IF_Ini > 0) {
					J = 40;
					if (J > Soil_Layers) {
						for (L = Soil_Layers; L < J; L++) {  //Soill_Layer to J
							Teq = (A[L - 1] * Tsoil[L - 1] + A[L] * Tsoil[L + 1]) / (A[L - 1] + A[L]);
							//		 Fac = (A[L-1]+A[L])/(HC[L]*Depth[L])*TimeStep;  
							Soil_Temp[L] = Fac[L] * Teq + (1 - Fac[L]) * Tsoil[L];
						} //
					}
					else J = Soil_Layers;

					for (L = J; L < Total_Layers; L++) {  //J to Total_Layers-1
						Teq = (Geothermal_Flux * 0.0001 + A[L - 1] * Tsoil[L - 1]) / A[L - 1];
						F = A[L - 1] / (HC[L] * Depth[L]) * TimeStep;
						if (F > 1) Soil_Temp[L] = Teq;
						else Soil_Temp[L] = F * Teq + (1 - F) * Tsoil[L];
					} //for
				}  //if IF_Ini>0
				else {
					for (L = Soil_Layers; L < Total_Layers - 1; L++) {  //Total_Layer-1
						Teq = (A[L - 1] * Tsoil[L - 1] + A[L] * Tsoil[L + 1]) / (A[L - 1] + A[L]);
						//		 Fac = (A[L-1]+A[L])/(HC[L]*Depth[L])*TimeStep;  
						Soil_Temp[L] = Fac[L] * Teq + (1 - Fac[L]) * Tsoil[L];
					} // end of rock layer loop L

			  //Bottom layer
					L = Total_Layers - 1;
					Teq = (Geothermal_Flux * 0.0001 + A[L - 1] * Tsoil[L - 1]) / A[L - 1];
					//	  Fac = A[L-1]/(HC[L]*Depth[L])*TimeStep;
					Soil_Temp[L] = Fac[L] * Teq + (1 - Fac[L]) * Tsoil[L];
				}  //end of else

			  //end layer loop --------------
				for (L = 0; L < Lu; L++) Tsnow[L] = Snow_Temp[L];
				for (L = 0; L < Total_Layers; L++) Tsoil[L] = Soil_Temp[L];

				Daily_Tcanopy += Tcanopy / (86400.0 / TimeStep);
				Daily_Tsurface += Tsurface / (86400.0 / TimeStep);
				for (L = 0; L < Total_Layers; L++)  Daily_Tsoil[L] += Soil_Temp[L] / (86400.0 / TimeStep);
				for (L = 0; L < Lu; L++)            Daily_Tsnow[L] += Snow_Temp[L] / (86400.0 / TimeStep);

				Soil_Water_Distrib(ET_Layers, Wflux_Layers, ET_Soil, Evap);

			}  //loop I
			//------------------------------------

			//--- subsidence
			if (IF_Ini == 0) {    // && Calendar_Year>=2000) {
				k = 0;
				for (L = 1; L < Soil_Layers; L++) {  //considering only the top layer with excess water
					if (DxWater[L] > 0.0001) {
						k = L;
						break;
					}
				}
				if (k > 0) {   //movement of excess water in the top layer with excess water to upper layers
					double	Daily_Subs = 0;
					P = 1.0;
					for (L = k; L >= 0; L--) {
						if (Soil_Temp[L] > 0.1 && DxWater[k] > 0) {
							if (Soil_Water[L] < Porosity[L]) {  //put excess water in the pores of the same layer
								F1 = (Porosity[L] - Soil_Water[L]) * DepthFsoil[L];
								if (F1 > DxWater[k] * P) F2 = DxWater[k] * P;   //all excess water in layer k moved up to layer L
								else F2 = F1;

								Daily_Subs += F2;

								Soil_Water[L] += F2 / DepthFsoil[L];
								DxWater[k] -= F2;
							}  //if
							P *= 0.2;   //the rate of water release reduced 
						}  //if
						else break;   //Soil_Temp[L]<=0;
					}	//for loop

					if (Daily_Subs > 0) {
						Subsidence += Daily_Subs;

						if (Subs_Layer + Daily_Subs < Depth[k + 1]) {
							DxWater[k] += DxWater[k + 1] / Depth[k + 1] * Daily_Subs;  //the layer shifts down, adding excess water and ice from next layer to this layer
							DxIce[k] += DxIce[k + 1] / Depth[k + 1] * Daily_Subs;

							Subs_Layer += Daily_Subs;

							F1 = (Depth[k + 1] - Daily_Subs) / Depth[k + 1];  //update DxIce and DxWater for layer k+1. other conditions assume no change
							F2 = Daily_Subs / Depth[k + 2];
							DxWater[k + 1] = DxWater[k + 1] * F1 + DxWater[k + 2] * F2;
							DxIce[k + 1] = DxIce[k + 1] * F1 + DxIce[k + 2] * F2;
						}
						else {  //Sub_Layer+F2 > Depth[k+1]
							F1 = Depth[k + 1] - Subs_Layer;
							F2 = Subs_Layer + Daily_Subs - Depth[k + 1];
							DxWater[k] += DxWater[k + 1] / Depth[k + 1] * F1 + DxWater[k + 2] / Depth[k + 2] * F2;
							DxIce[k] += DxIce[k + 1] / Depth[k + 1] * F1 + DxIce[k + 2] / Depth[k + 2] * F2;
							Subs_Layer = F2;

							F1 = (Depth[k + 1] - Subs_Layer) / Depth[k + 1];  //update DxIce and DxWater for layer k+1. other conditions assume no change
							F2 = Subs_Layer / Depth[k + 2];
							DxWater[k + 1] = DxWater[k + 1] * F1 + DxWater[k + 2] * F2;
							DxIce[k + 1] = DxIce[k + 1] * F1 + DxIce[k + 2] * F2;
							// simple averae fornthe property of layer k
							Porosity[k] = (Porosity[k] + Porosity[k + 1]) / 2;
							Field_Capacity[k] = (Field_Capacity[k] + Field_Capacity[k + 1]) / 2;
							Wilting_Point[k] = (Wilting_Point[k] + Wilting_Point[k + 1]) / 2;
							Water_Fs[k] = (Water_Fs[k] + Water_Fs[k + 1]) / 2;
							Water_Ks[k] = (Water_Ks[k] + Water_Ks[k + 1]) / 2;
							Water_b[k] = (Water_b[k] + Water_b[k + 1]) / 2;
							Organic[k] = (Organic[k] + Organic[k + 1]) / 2;
							Mineral[k] = (Mineral[k] + Mineral[k + 1]) / 2;
							Soil_Fstone[k] = (Soil_Fstone[k] + Soil_Fstone[k + 1]) / 2;

							Fsoil[k] = 1 - Soil_Fstone[k] - (DxIce[k] + DxWater[k]) / Depth[k];
							if (Fsoil[k] < 1.0e-10) Fsoil[k] = 1.0e-10;

							Soil_Water[k] = (Soil_Water[k] + Soil_Water[k + 1]) / 2;
							if (Soil_Water[k] > Porosity[k]) Soil_Water[k] = Porosity[k];

							Soil_Temp[k] = (Soil_Temp[k] + Soil_Temp[k + 1]) / 2;
							Tsoil[k] = Soil_Temp[k];

							if (Tsoil[k] < 0) {
								if (Tsoil[k] <= Para_Tm) Fice[k] = 1.0;
								else Fice[k] = Para_b - Para_c * Tsoil[k] - Para_b / (1 - Para_a * Tsoil[k]);
							}
							Fwater[k] = 1.0 - Fice[k];

							//shifting all layer up 1 layer, fill the bottom with bedrock
							Soil_Layers -= 1;
							Total_Soil_Depth -= Depth[Soil_Layers - 1];
							for (i = k + 1; i < Soil_Layers; i++) {
								Porosity[i] = Porosity[i + 1];
								Field_Capacity[i] = Field_Capacity[i + 1];
								Wilting_Point[i] = Wilting_Point[i + 1];
								Water_Fs[i] = Water_Fs[i + 1];
								Water_Ks[i] = Water_Ks[i + 1];
								Water_b[i] = Water_b[i + 1];
								Organic[i] = Organic[i + 1];
								Mineral[i] = Mineral[i + 1];
								Soil_Fstone[i] = Soil_Fstone[i + 1];

								Soil_Water[i] = Soil_Water[i + 1];
								Soil_Temp[i] = Soil_Temp[i + 1];

								Tsoil[i] = Soil_Temp[i];
								Fice[i] = Fice[i + 1];
								Fwater[i] = Fwater[i + 1];
							}

							Fsoil[k + 1] = 1 - Soil_Fstone[k + 1] - (DxIce[k + 1] + DxWater[k + 1]) / Depth[k + 1];
							if (Fsoil[k + 1] < 1.0e-10) Fsoil[k + 1] = 1.0e-10;

							for (i = k + 2; i < Soil_Layers; i++) {
								DxWater[i] = DxWater[i + 1] / Depth[i + 1] * Depth[i];
								DxIce[i] = DxIce[i + 1] / Depth[i + 1] * Depth[i];
								Fsoil[i] = 1 - Soil_Fstone[i] - (DxIce[i] + DxWater[i]) / Depth[i];
								if (Fsoil[i] < 1.0e-10) Fsoil[i] = 1.0e-10;
							}
						}

						F1 = Soil_Temp[k] * HC[k] * Depth[k];   //heat available
						if (F1 <= DxIce[k] * Water_Ice) {  //part of Xice melt, Ts to 0, 
							F2 = F1 / Water_Ice;
							DxIce[k] -= F2;
							DxWater[k] += F2;
							Soil_Temp[k] = 0.0;
						}
						else {				//all Xice melt, Ts reduced but not 0
							F2 = (F1 - DxIce[k] * Water_Ice) / (HC[k] * Depth[k]);
							Soil_Temp[k] -= F2;   //should be >0 C

							DxWater[k] += DxIce[k];
							DxIce[k] = 0.0;
						}
						Soil_Fxice[k] = DxIce[k] / Depth[k];

						Fsoil[k] = 1 - Soil_Fstone[k] - (DxIce[k] + DxWater[k]) / Depth[k];
						if (Fsoil[k] < 1.0e-10) Fsoil[k] = 1.0e-10;

						Tsoil[k] = Soil_Temp[k];
					}  //if Daily_subs > 0

				}    //if k>0
			}   //if(IF_Ini ==0)



			//determin thaw/freeze depths based on temperature (two)
			//frozen base
			if (Daily_Tsoil[Total_Layers - 1] < 0)
				Daily_Freeze_Depth[2] = Total_Depth - Depth[Total_Layers - 1] * 0.5 +
				(0 - Daily_Tsoil[Total_Layers - 1]) / Geothermal_Flux0 * K_Minerals[Total_Layers - 1] * 10000;
			else {
				Daily_Freeze_Depth[2] = 0;
				F1 = Total_Depth - Depth[Total_Layers - 1] * 0.5;
				for (L = Total_Layers - 1; L > 0; L--) {
					F2 = F1;
					F1 -= (Depth[L] + Depth[L - 1]) * 0.5;
					if (Daily_Tsoil[L - 1] < 0 && Daily_Tsoil[L] >= 0.0) {
						Daily_Freeze_Depth[2] = F1 + (F2 - F1) / (Daily_Tsoil[L] - Daily_Tsoil[L - 1]) * (0 - Daily_Tsoil[L - 1]);
						break;
					}
				}
			}

			//seasonal thaw/freeze at the top layers
			if (Daily_Tsoil[0] < 0) {
				Thaw_Layers = 0;
				Daily_Thaw_Depth[0] = 0;  //top freezing

				F2 = Depth[0] * 0.5;
				J = Total_Layers - 1;
				for (L = 1; L < Total_Layers; L++) {
					F1 = F2;
					F2 += (Depth[L - 1] + Depth[L]) * 0.5;
					if (Daily_Tsoil[L - 1] < 0 && Daily_Tsoil[L] >= 0.0) { //freeze depth
						Daily_Freeze_Depth[0] = F1 + (F2 - F1) / (Daily_Tsoil[L] - Daily_Tsoil[L - 1]) * (0 - Daily_Tsoil[L - 1]);
						J = L;
						break;
					}  //if
				}  //for

				if (J == Total_Layers - 1) {  //Frozen to bottom
					Daily_Freeze_Depth[0] = Daily_Freeze_Depth[2];
					Daily_Freeze_Depth[1] = Daily_Freeze_Depth[2];
					Daily_Thaw_Depth[1] = 0;
				}
				else {
					Daily_Freeze_Depth[1] = Daily_Freeze_Depth[0];
					// checking if there is talik below layer J
					Daily_Thaw_Depth[1] = 0;
					k = Total_Layers - 1;
					for (L = J + 1; L < Total_Layers; L++) {
						F1 = F2;
						F2 += (Depth[L - 1] + Depth[L]) * 0.5;
						if (Daily_Tsoil[L - 1] >= 0 && Daily_Tsoil[L] < 0.0) { //thaw depth, permafrost table
							Daily_Thaw_Depth[1] = F1 + (F2 - F1) / (Daily_Tsoil[L] - Daily_Tsoil[L - 1]) * (0 - Daily_Tsoil[L - 1]);
							k = L;
							break;
						}
					}  //for
				}  //end of else
			}  //end of if
			else {  //Daily_Tsoil[0]>=0
				Daily_Freeze_Depth[0] = 0;

				F2 = Depth[0] * 0.5;
				J = Total_Layers - 1;
				for (L = 1; L < Total_Layers; L++) {
					F1 = F2;
					F2 += (Depth[L - 1] + Depth[L]) * 0.5;
					if (Daily_Tsoil[L - 1] >= 0 && Daily_Tsoil[L] < 0.0) { //thaw depth
						Daily_Thaw_Depth[0] = F1 + (F2 - F1) / (Daily_Tsoil[L] - Daily_Tsoil[L - 1]) * (0 - Daily_Tsoil[L - 1]);
						Thaw_Layers = L;
						if (Thaw_Layers > Soil_Layers) Thaw_Layers = Soil_Layers;
						J = L;
						break;
					}
				}  //for
				if (J == Total_Layers - 1) {  //thaw to bottom, no permafrost
					Thaw_Layers = Soil_Layers;
					Daily_Thaw_Depth[0] = Total_Depth;
					Daily_Thaw_Depth[1] = Total_Depth;
					Daily_Freeze_Depth[1] = 0;
				}
				else {  //check the depth of freezing tongue/permafrost table
					k = Total_Layers - 1;
					for (L = J + 1; L < Total_Layers; L++) { //freeze tongue
						F1 = F2;
						F2 += (Depth[L - 1] + Depth[L]) * 0.5;
						if (Daily_Tsoil[L - 1] < 0 && Daily_Tsoil[L] >= 0.0) { //freeze depth
							Daily_Freeze_Depth[1] = F1 + (F2 - F1) / (Daily_Tsoil[L] - Daily_Tsoil[L - 1]) * (0 - Daily_Tsoil[L - 1]);
							k = L;
							break;
						}  //if
					}  //for
					if (k == Total_Layers - 1 || Daily_Freeze_Depth[1] + 5 > Daily_Freeze_Depth[2]) { //to the perm.base
						Daily_Freeze_Depth[1] = Daily_Freeze_Depth[0];
						Daily_Thaw_Depth[1] = Daily_Thaw_Depth[0];
					}
					else { //frozen tongue exists
						for (L = k + 1; L < Total_Layers; L++) {
							F1 = F2;
							F2 += (Depth[L - 1] + Depth[L]) * 0.5;
							if (Daily_Tsoil[L - 1] >= 0 && Daily_Tsoil[L] < 0.0) { //thaw depth, permafrost table
								Daily_Thaw_Depth[1] = F1 + (F2 - F1) / (Daily_Tsoil[L] - Daily_Tsoil[L - 1]) * (0 - Daily_Tsoil[L - 1]);
								break;
							}
						}  //for
					}  //else
				}  //else
			}  //end of else

			F = Daily_Thaw_Depth[0];
			if(Daily_Thaw_Depth[1] > F) F = Daily_Thaw_Depth[1];  


			if (F > Monthly_Thaw_Depth[Month]) Monthly_Thaw_Depth[Month] = F;
			if (F > Thaw_Max) Thaw_Max = F;  //perm. table.

			F = Daily_Freeze_Depth[0];
			if(Daily_Freeze_Depth[1]>F) F = Daily_Freeze_Depth[1];  

			if (F > Monthly_Freeze_Depth[Month]) Monthly_Freeze_Depth[Month] = F;
			if (F > Freeze_Max) Freeze_Max = F;

			if (Daily_Freeze_Depth[2] < PermBase_Min) PermBase_Min = Daily_Freeze_Depth[2];

			Annual_Evap += Daily_Evap;
			Annual_ETplant += Daily_ETplant;

			Annual_Precip += Daily_Precip[DOY];
			Annual_Snowfall += Daily_Precip[DOY] * Daily_Snowfall_Frac[DOY];
			Annual_Snowfall_hy += Daily_Precip[DOY] * Daily_Snowfall_Frac[DOY];

			if (DOY == 200) {     //
				Snowfall_hy_output = Annual_Snowfall_hy;
				nf_hy_output = FDDs_hy / FDDa_hy;
				FDDa_hy_output = FDDa_hy;
				FDDa_hy = 0;
				FDDs_hy = 0;
				Annual_Snowfall_hy = 0;
			}

			Annual_Outflow += Daily_Outflow;

			if (DOY < 200 && Soil_Temp[0] < 0 && FreezeBackDOY1 == 200) {   //first time in a year frozen completely
				i = 0;
				for (L = 0; L < Soil_Layers; L++) {
					if (Soil_Temp[L] > -0.2) i++;
				}
				if (i == 0) FreezeBackDOY1 = DOY + 1;
			}

			if (DOY > 200 && FreezeBackDOY2 == 367) {   //first time in a year frozen completely
				i = 0;
				for (L = 0; L < Soil_Layers; L++) {
					if (Soil_Temp[L] > -0.2) i++;
				}
				if (i == 0) FreezeBackDOY2 = DOY + 1;
			}

			if (DOY == 210) {        // at least one no snow 
				Snow_Depth = 0.0;
				Last_Snow_Depth = 0;
				Last_Lu = 0;
				Lu = 0;
				Last_Lu_Depth = 10.0;
				Lu_Depth = 10.0;
			}

			Soil_Water_0 = Pond;
			for (L = 0; L < Soil_Layers; L++) Soil_Water_0 += Soil_Water[L] * Fsoil[L] * Depth[L] * 10.0;

			Monthly_Precip[Month] += Daily_Precip[DOY];
			Monthly_Snowfall[Month] += Daily_Precip[DOY] * Daily_Snowfall_Frac[DOY];

			Monthly_AET[Month] += Daily_Evap + Daily_ETplant;

			Monthly_Evap[Month] += Daily_Evap;
			Monthly_Transp[Month] += Daily_ETplant;
			Monthly_Outflow[Month] += Daily_Outflow;

			Monthly_Snow_Pack[Month] += Snow_Depth / DAYS[Month];
			if (Snow_Depth > 0) Monthly_SnowCoverDays[Month] += 1;

			Daily_Snow_Depth[DOY] = Snow_Depth;

			if (Month == 3) Snow_Depth_April[Day] = Snow_Depth;  //snow depth in April for output

			F = 0;
			for(L=0; L<Lu; L++) F += Lu_Depth * Snow_Density[L] * 10.0;  //mm water or g
			Monthly_SWE[Month] += F / DAYS[Month];

			for (L = 0; L < Total_Layers; L++) Annual_ProfileT[L] += Daily_Tsoil[L] / (365.0 + Current_Leap_Year_Flag);
			for (L = 0; L < 20; L++) {
				if (Daily_Tsoil[L] < Annual_Ts_Min[L]) Annual_Ts_Min[L] = Daily_Tsoil[L];
				if (Daily_Tsoil[L] > Annual_Ts_Max[L]) Annual_Ts_Max[L] = Daily_Tsoil[L];
			}

			F = 0;
			for (L = 0; L < Show_SW_Layers; L++) {
				F += Soil_Water[L] * Depth[L] * Fsoil[L];  //Soil water in cm, 0-50cm
			}
			Monthly_Soil_Water[Month] += F / DAYS[Month];


			Monthly_Water_Table[Month] -= Water_Table / DAYS[Month];

			Monthly_Soil_Temp[Month] += Daily_Tsoil[1] / DAYS[Month];

			for (L = 0; L < Total_Layers; L++) Monthly_Ts[L] += Daily_Tsoil[L] / DAYS[Month];

			if (Snow_Depth > 0) Annual_SnowCoverDays += 1;
			Annual_SnowDepth += Snow_Depth;

			//-----------Daily output
			if (Daily_Tair[DOY] > 0)  TDDa += Daily_Tair[DOY];
			else {
				FDDa += Daily_Tair[DOY];
				FDDa_hy += Daily_Tair[DOY];
			}
			if (Daily_Tsoil[0] > 0)	TDDs += Daily_Tsoil[0];
			else {
				FDDs += Daily_Tsoil[0];
				FDDs_hy += Daily_Tsoil[0];
			}

			// Soil temperature
			if (IF_Ini == 0 && Calendar_Year >= Out_Day[0] && Calendar_Year <= Out_Day[1]) {
				fprintf(fpt_Out_Day, "%.4lf,%d-%d-%d, %d, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, ",
					Calendar_Year + DOY / 366.0, Calendar_Year, Month + 1, Day + 1, DOY+1, Daily_Tair[DOY], Daily_Precip[DOY], Snow_Depth, Water_Table * 0.01,       //Total_1,   
					Daily_Thaw_Depth[0] * 0.01, Daily_Thaw_Depth[1] * 0.01,
					Daily_Freeze_Depth[0] * 0.01, Daily_Freeze_Depth[1] * 0.01, Daily_Freeze_Depth[2] * 0.01);  // Pond

			 //    for(L=Out_Layer[0]; L<=Out_Layer[1]; L++) fprintf(fpt_Out_Day, "%.2lf, ", Daily_Tsoil[L]); 

			 //    for(L=Out_Layer[0]; L<=Out_Layer[1]; L++) fprintf(fpt_Out_Day, "%.4lf, ", Soil_Water[L]); //Suction[L]);  %.4lf Soil_Water[L]*Fwater[L]
			 //  for(L=Out_Layer[0]; L<=Out_Layer[1]; L++) fprintf(fpt_Out_Day, "%.2lf %.2lf ", Daily_Tsoil[L], Soil_Water[L]);   //Ts and SW for Ottawa

			 //5cm, 10cm, 20cm, 50cm, 100cm, 150cm
			 //   fprintf(fpt_Out_Day, "%.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf", Daily_Tsoil[0], (Daily_Tsoil[0]+Daily_Tsoil[1])/2, (Daily_Tsoil[1]+Daily_Tsoil[2])/2, 
			 //	       (Daily_Tsoil[4]+Daily_Tsoil[5])/2, (Daily_Tsoil[9]+Daily_Tsoil[10])/2, Daily_Tsoil[13] ); 

			//5cm, 10cm, 12.5cm, 15cm, 20cm, 25cm
			//	fprintf(fpt_Out_Day, "%.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, ", Daily_Tsoil[0], (Daily_Tsoil[0] + Daily_Tsoil[1]) / 2,
			//		((Daily_Tsoil[0] + Daily_Tsoil[1]) / 2 + Daily_Tsoil[1]) / 2, Daily_Tsoil[1], (Daily_Tsoil[1] + Daily_Tsoil[2]) / 2, Daily_Tsoil[2]);

				for (L = Out_Layer[0]; L <= Out_Layer[1]; L++) fprintf(fpt_Out_Day, "%.2lf, ", Daily_Tsoil[L]);

				fprintf(fpt_Out_Day, "\n");
			}
			/*
			if (IF_Ini == 0 && Calendar_Year == 1900 && DOY == 150) {
				for (L = 0; L < Total_Layers; L++) {
					if(L<Soil_Layers)   fprintf(fpt_Out_All_Sites, "%d %lf %lf %lf\n", L, K[L], HC[L], Porosity[L]);
					else                fprintf(fpt_Out_All_Sites, "%d %lf %lf 0.0\n", L, K[L], HC[L]);
				}
			}
			*/

			Last_Tair = Daily_Tair[DOY];
			Last_Solar_Rad = Daily_Solar_Rad[DOY];
			Last_WT = Water_Table;
			Last_ETplant = Daily_ETplant;
			Last_Evap = Daily_Evap;
			for (i = 0; i < Show_SW_Layers; i++) Last_SoilWater[i] = Soil_Water[i];
			for (i = 0; i < 5; i++) Last_Tsoil[i] = Daily_Tsoil[i];

			DOY++;
		}  //====================== end of Day loop

		if (Wflux_Layers > Total_Layers - 3) {
			Soil_Water[Soil_Layers] = Porosity[Soil_Layers]; //assuming the soil is saturated if the soil is deeper than modelled layers
		}

		if (IF_Ini == 0 && Calendar_Year >= Out_Month[0] && Calendar_Year <= Out_Month[1]) {

			fprintf(fpt_Out_Month, "%.4lf, %d, %d, %.3lf, %.1lf, %.1lf, %.2lf, %.0lf, %.3lf, %.1lf,",
				Calendar_Year + Month / 12.0, Calendar_Year, Month + 1, Monthly_Tair[Month], Monthly_Precip[Month], Monthly_Snowfall[Month],
				Monthly_Snow_Pack[Month], Monthly_SnowCoverDays[Month], -Monthly_Water_Table[Month] * 0.01, Monthly_Soil_Water[Month]);

			for (L = Out_Layer[0]; L <= Out_Layer[1]; L++) fprintf(fpt_Out_Month, "%.2lf, ", Monthly_Ts[L]);

			//5cm, 10cm, 20cm, 50cm, 100cm, 150cm
			//   fprintf(fpt_Out_Month, "%.3lf, %.3lf, %.3lf, %.3lf, %.3lf, %.3lf", Monthly_Ts[0], (Monthly_Ts[0]+Monthly_Ts[1])/2, (Monthly_Ts[1]+Monthly_Ts[2])/2, 
			//	       (Monthly_Ts[4]+Monthly_Ts[5])/2, (Monthly_Ts[9]+Monthly_Ts[10])/2, Monthly_Ts[13] ); 

			fprintf(fpt_Out_Month, "\n");
		}

	} //******************** End of Monthly Loop ***********************

	//Annual values for output
	Annual_Values[0] = Thaw_Max;
	Annual_Values[1] = Freeze_Max;
	Annual_Values[2] = PermBase_Min;

	Annual_Values[3] = Annual_ProfileT[2];  //20 cm
	Annual_Values[4] = Annual_ProfileT[5];  //50 cm
	Annual_Values[5] = Annual_ProfileT[10]; //1 m
	Annual_Values[6] = Annual_ProfileT[16]; //2 m
	Annual_Values[7] = Annual_ProfileT[22];  //5 m
	Annual_Values[8] = Annual_ProfileT[27];  //10 m
	Annual_Values[9] = Annual_ProfileT[33];  //20 m
	Annual_Values[10] = Annual_ProfileT[42];  //40 m
	Annual_Values[11] = Annual_ProfileT[51];  //70 m
	Annual_Values[12] = Annual_ProfileT[Total_Layers - 1];  //120 m

	nt = TDDs / TDDa;
	nf = FDDs / FDDa;

	double EdFac_t, EdFac_f, ThawFreeze_Depth, TopPerm_T;
	int ThawFreeze_layer, TopPerm_Layer;

	if (Thaw_Max < Freeze_Max && Thaw_Max < 119700.0 - 1) {  //permafrost exists and no talik
		ThawFreeze_Depth = Thaw_Max;
		EdFac_t = Thaw_Max / sqrt(TDDa * nt);
		EdFac_f = -999.9;
	}
	else {
		ThawFreeze_Depth = Freeze_Max;

		EdFac_t = -999.9;
		EdFac_f = ThawFreeze_Depth / sqrt(-FDDa_hy_output * nf_hy_output);
	}

	ThawFreeze_layer = -1;
	F = 0;
	for (L = 0; L < Total_Layers; L++) {
		F += Depth[L];
		if (F >= ThawFreeze_Depth) {
			ThawFreeze_layer = L;
			break;
		}
	}
	if (ThawFreeze_layer < 0) {
		printf("Year: %d Thaw_max = %.2lf Freeze_max = %.2lf  Thawing_freezing layer did not found ! (useing 1m)\n", Calendar_Year, Thaw_Max, Freeze_Max);
		ThawFreeze_layer = 10;
	}

	Offset_Surf = Annual_ProfileT[0] - Ta_Year;
	Offset_Thermal = Annual_ProfileT[0] - Annual_ProfileT[ThawFreeze_layer];
	Ratio_K = (Annual_ProfileT[ThawFreeze_layer] * (365.0 + Current_Leap_Year_Flag) - FDDs) / TDDs;

	TopPerm_Layer = -1;
	F = 0;
	for (L = 0; L < Total_Layers; L++) {
		F += Depth[L];
		if (F >= Thaw_Max) {
			TopPerm_Layer = L;
			break;
		}
	}
	if (TopPerm_Layer > -1)  TopPerm_T = Annual_ProfileT[TopPerm_Layer];
	else  TopPerm_T = -999;

	//determine snow end doy for max two snow periods
	if (IF_Ini == 0) {
		double SnowDepth_Min = 5.0;

		Snowcover_Start[0] = -1;  Snowcover_End[0] = -1;
		Snowcover_Start[1] = -1;  Snowcover_End[1] = -1;

		Snowcover_Period = 0;
		i = 5;  //1st period no need to check accumulation days>5 
		L = 0;
		for (DOY = 0; DOY < 210; DOY++) {
			if (i >= 5 && Snowcover_Start[Snowcover_Period] < 0 && Daily_Snow_Depth[DOY] > SnowDepth_Min) {  //5.0cm
				Snowcover_Start[Snowcover_Period] = DOY + 1;
				if (Snowcover_Period == 1) Snowcover_Start[Snowcover_Period] -= i + 1;
			}

			if (Daily_Snow_Depth[DOY] > SnowDepth_Min) {  //5.0cm
				i++;
				L = 0;
				if (i >= 5 && Snowcover_Start[Snowcover_Period] > 0) Snowcover_End[Snowcover_Period] = DOY + 1;  //
			}
			else {  //no snow
				L++;   //Consective no-snowcover days 
				if (L >= 3) {  //a new snow cover period
					i = 0;
					Snowcover_Period = 1;
				}  //if
			}
		}  //for

		if (Snowcover_Start[1] > 0) {
			L = 0;
			for (DOY = Snowcover_Start[1] - 1; DOY > Snowcover_End[0] - 3; DOY--) {
				if (Daily_Snow_Depth[DOY] > SnowDepth_Min) {                        //5.0cm
					if (L < 3) Snowcover_Start[Snowcover_Period] = DOY + 1;
					else break;
					L = 0;
				}
				else {
					L++;
				}
			}  //for
		}  //if
	}  //if(IF_Ini==0

	// annual output
	if (IF_Ini == 0 && Calendar_Year >= Out_Year[0] && Calendar_Year <= Out_Year[1]) {
		fprintf(fpt_Out_Year, "%d, %.2lf, %.1lf, %.1lf, %.1lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.0lf, %.1lf, ",
			Calendar_Year, Ta_Year, Annual_Precip, Annual_Snowfall, Snowfall_hy_output, ThawFreeze_Depth * 0.01, Thaw_Max * 0.01, Freeze_Max * 0.01, PermBase_Min * 0.01, 
			Monthly_Snow_Pack[0], Annual_SnowDepth / (365. + Current_Leap_Year_Flag), Annual_SnowCoverDays, Subsidence);  //Subsidence, FreezeBackDOY1, FreezeBackDOY2,

	 //5cm, 10cm, 20cm, 50cm, 100cm, TTOP
		fprintf(fpt_Out_Year, "%.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.4lf, ", Annual_ProfileT[0], (Annual_ProfileT[0] + Annual_ProfileT[1]) / 2, (Annual_ProfileT[1] + Annual_ProfileT[2]) / 2,
			(Annual_ProfileT[4] + Annual_ProfileT[5]) / 2, (Annual_ProfileT[9] + Annual_ProfileT[10]) / 2, Annual_ProfileT[ThawFreeze_layer]);

		fprintf(fpt_Out_Year, "%.1lf, %.1lf, %.1lf, %.4lf, %.4lf, %.4lf, %.4lf, %.2lf, %.2lf, ", TDDa, FDDa, FDDa_hy_output, nt, nf, nf_hy_output, Ratio_K, Offset_Surf, Offset_Thermal);
		if (EdFac_t < 0) fprintf(fpt_Out_Year, "-1, %.4lf", EdFac_f);
		else fprintf(fpt_Out_Year, "%.4lf,-1", EdFac_t);

		fprintf(fpt_Out_Year, ", %.4lf, ", TopPerm_T);

		for (L = Out_Layer[0]; L <= Out_Layer[1]; L++) fprintf(fpt_Out_Year, "%.3lf, ", Annual_ProfileT[L]);

		fprintf(fpt_Out_Year, "\n");
	}

	if (IF_Ini == 0 && Calendar_Year >= Out_Year_AllSites[0] && Calendar_Year <= Out_Year_AllSites[1]) {

		//     fprintf(fpt_Out_All_Sites, "%d %d %.2lf %.1lf %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %.0lf %.1lf ", Site_Sequence_Num,
		//		 Calendar_Year, Ta_Year, Annual_Precip, Annual_Snowfall, Thaw_Max*0.01, Freeze_Max*0.01, PermBase_Min*0.01, Monthly_Snow_Pack[0], 
		//	     Annual_SnowDepth/(337+DAYS[1]), Annual_SnowCoverDays, Subsidence);  //

		// fprintf(fpt_Out_All_Sites, "%d %d %d %d", Snowcover_Start[0], Snowcover_End[0], Snowcover_Start[1], Snowcover_End[1]);

		//	 	 for(L=Out_Layer_AllSites[0]; L<=Out_Layer_AllSites[1]; L++) fprintf(fpt_Out_All_Sites, "%7.3lf ", Annual_ProfileT[L]); 
		//		 fprintf(fpt_Out_All_Sites, "\n");


		i = AllSites_1Year_Output_Num * (Calendar_Year - Out_Year_AllSites[0]);

		AllSites_Output[i + 0] = Calendar_Year;
		AllSites_Output[i + 1] = Thaw_Max * 0.01;
		AllSites_Output[i + 2] = Freeze_Max * 0.01;
		AllSites_Output[i + 3] = PermBase_Min * 0.01;

		AllSites_Output[i + 4] = Snowcover_Start[0];
		AllSites_Output[i + 5] = Snowcover_End[0];
		AllSites_Output[i + 6] = Snowcover_Start[1];
		AllSites_Output[i + 7] = Snowcover_End[1];

		for (Month = 0; Month < 12; Month++) AllSites_Output[i + 8 + Month] = Monthly_Snow_Pack[Month];    //8-19 
		for (Month = 0; Month < 12; Month++) AllSites_Output[i + 20 + Month] = Monthly_SWE[Month];    //20-31 

		for (Day = 4; Day <= 16; Day++)  AllSites_Output[i + 32 + Day-4] = Snow_Depth_April[Day];  //April 5 to 17, 13 days

//		for (L = 0; L < 20; L++) AllSites_Output[i + 40 + L] = Annual_Ts_Min[L];   //40-59
//		for (L = 0; L < 20; L++) AllSites_Output[i + 60 + L] = Annual_Ts_Max[L];   //60-79 

//		for (L = Out_Layer_AllSites[0]; L <= Out_Layer_AllSites[1]; L++) {
//			AllSites_Output[i + 80 + L] = Annual_ProfileT[L];
//		}
	}

	//show some results
	//if (IF_Ini > 0) printf("%d ", IF_Ini);
	//else printf("%d ", Calendar_Year);

//	printf("Ini:%d  Year: %d Ta:%.2lf T1:%.2lf dTb: %.4lf  AMTD: %.1lf AMFD: %.1lf Snow1: %.1lf  ",
//		IF_Ini, Calendar_Year, Ta_Year, Annual_ProfileT[1], Annual_ProfileT[Total_Layers - 1] - Tb0,
//		Thaw_Max, Freeze_Max, Monthly_Snow_Pack[0]);

	//  F= 0;
	//  for(L=0; L<Soil_Layers; L++) F += Soil_Water[L]*Depth[L]*10.0;
	//  F1 = Annual_Precip-(Annual_AET+Annual_Outflow)-(F-Soil_Water_0);  //check annual balance
	//  String.Format("dSoil_Water: %.1lf  Precip: %.1lf  EVP: %.1lf Outflow: %.1lf  Balance: %.2lf  ", 
	//	         F-Soil_Water_0, Annual_Precip, Annual_AET, Annual_Outflow, F1);
	//  pDC->TextOut(400, 60, String);

	return Annual_ProfileT[Total_Layers - 1] - Tb0;
}

//#define Earth_R0 6371.0   //earth radius, km
//#define Earth_H0  20.0    //effective thickness of the atmopthere for scattering
//#define Earth_Diff_KK 0.01  //reduction rate, calubrated by based diffuse fraction at 90 and 0
//===========================================
void Slope_Solar_Rad(int DOY, double Time, double sin_Lat, double cos_Lat, double cos_Slope, double sin_Slope, double cos_sFactors0,
	double DayF_Diffuse, double Rad0, double Slope, double Aspect, double ViewShed[], double Diffuse_Rad_F, double sFactors[], double Mid[])
{
	//sFactors[0]: solar zenith angle at noon, in radiant
	//sFactors[1]: the ratio between direct radiation at noon on a horizontal and the daily mean direct radiation on a horizontal surface 
	//sFactors[2]: the ratio between diffuse radiation at noon and the daily mean diffuse radiation on a horizontal surface 
	//sFactors[3]: average solar radiation on the top of the atmosphere

	int k;
	double Rad_Direct, Rad_Diffuse;
	double Declination, sin_D, cos_D, S, C;
	double Hour_Angle, Altitude0, sin_Alt, sin_Altitude0, cos_Altitude0, Zenith, Azimuth;
	double F, P0;

	Declination = 23.45 * RAD * sin(2 * Pi * (DOY + 284) / 365.25);

	sin_D = sin(Declination);
	cos_D = cos(Declination);

	S = sin_Lat * sin_D;
	C = cos_Lat * cos_D;

	Hour_Angle = 2 * Pi * (Time - 12.0) / 24.0;
	Altitude0 = asin(S + C * cos(Hour_Angle));
	Zenith = 0.5 * Pi - Altitude0;

	sin_Altitude0 = sin(Altitude0);
	cos_Altitude0 = cos(Altitude0);
	if (cos_Altitude0 <= 0.0001) cos_Altitude0 = 0.0001; //avoiding divided by 0

 //Max direct fraction
 //    double H, Min_Diff
 //   H = (-2*Earth_R0*sin_Altitude0 + sqrt(4*Earth_R0*Earth_R0*sin_Altitude0*sin_Altitude0 + 
 //	    4*Earth_H0*Earth_H0 + 8*Earth_R0*Earth_H0))/2;
 //   Min_Diff = 1 - exp(-Earth_Diff_KK*H);
 //   if(F_Diffuse < Min_Diff) F_Diffuse = Min_Diff;

	P0 = cos(0.5 * Pi * (Zenith - sFactors[0]) / (0.5 * Pi - sFactors[0]));

	//Azimuth: North:0, clockwise
	F = (sin_Altitude0 * sin_Lat - sin_D) / (cos_Altitude0 * cos_Lat);
	if (F > 0.99999 || F < -0.99999) F = 0.99999;

	if (Time < 12.0) Azimuth = Pi - acos(F);
	else Azimuth = Pi + acos(F);

	k = int(Azimuth / (22.5 * RAD) + 0.5);

	if (Altitude0 > ViewShed[k]) {  //un-blocked
		sin_Alt = cos_Slope * sin_Altitude0 + sin_Slope * cos_Altitude0 * cos(Azimuth - Aspect);   //sin_Altitude0
		if (sin_Alt < 0) sin_Alt = 0;
	}  //if
	else sin_Alt = 0;

	Rad_Direct = Rad0 * (1 - DayF_Diffuse) * sFactors[1] * P0 * sin_Alt / cos_sFactors0;
	Rad_Diffuse = Rad0 * DayF_Diffuse * sFactors[2] * P0 * Diffuse_Rad_F;

	//	Direct_Rad_Flat  = Rad0 * (1-DayF_Diffuse) * sFactors[1] * P0 * Direct_Rad_Flat;

	Mid[0] = Rad_Direct;
	Mid[1] = Rad_Diffuse;
	Mid[2] = sin_Alt;
}

void Update_Snow_Layers(int Lu, double Fresh_Snow_Depth)
{
	int NN, k, i;
	double XX[MaxLu + 1], YY[MaxLu + 1], TT[MaxLu + 1], WW[MaxLu + 1];
	double Depth, F;
	double Balance1, Balance2, Balance;

	Balance1 = Fresh_Snow_Depth * Fresh_Snow_Density;
	for (i = 0; i < Last_Lu; i++) Balance1 += (Last_Lu_Depth - dLu_Depth[i]) * Snow_Density[i];

	NN = 0;
	XX[0] = 0;
	if (Fresh_Snow_Depth > 0) {         //fresh snow layers
		XX[NN] = Fresh_Snow_Depth * 0.5;       //whole frsh layer
		YY[NN] = Fresh_Snow_Density;
		TT[NN] = Daily_Tair[DOY];
		WW[NN] = 0.0;
		NN = 1;
	}

	XX[NN] = Fresh_Snow_Depth + (Last_Lu_Depth - dLu_Depth[0]) * 0.5;  //mid layer
	YY[NN] = Snow_Density[0];
	TT[NN] = Tsnow[0];
	WW[NN] = Snow_Water[0];

	for (i = NN + 1; i < Last_Lu + NN; i++) {
		XX[i] = XX[i - 1] + (Last_Lu_Depth - dLu_Depth[i - NN] + Last_Lu_Depth - dLu_Depth[i - NN - 1]) * 0.5;
		YY[i] = Snow_Density[i - NN];
		TT[i] = Tsnow[i - NN];
		WW[i] = Snow_Water[i - NN];
	}

	Depth = Lu_Depth * 0.5;
	for (i = 0; i < Lu; i++) {
		k = Find_Snow_Layer(Depth, Last_Lu + NN, XX);
		if (k == 0) {
			Snow_Density[i] = YY[0];
			Tsnow[i] = TT[0];
			Snow_Water[i] = WW[0];
		}
		else if (k == Last_Lu + NN) {
			Snow_Density[i] = YY[Last_Lu + NN - 1];
			Tsnow[i] = TT[Last_Lu + NN - 1];
			Snow_Water[i] = WW[Last_Lu + NN - 1];
		}
		else {
			Snow_Density[i] = YY[k - 1] + (YY[k] - YY[k - 1]) / (XX[k] - XX[k - 1]) * (Depth - XX[k - 1]);
			Tsnow[i] = TT[k - 1] + (TT[k] - TT[k - 1]) / (XX[k] - XX[k - 1]) * (Depth - XX[k - 1]);
			Snow_Water[i] = WW[k - 1] + (WW[k] - WW[k - 1]) / (XX[k] - XX[k - 1]) * (Depth - XX[k - 1]);
		}
		Depth += Lu_Depth;

		if (Snow_Density[i] > 0.55) Snow_Density[i] = 0.55;
	}  //for loop

	Balance2 = 0;
	for (i = 0; i < Lu; i++) Balance2 += Lu_Depth * Snow_Density[i];
	Balance = Balance1 - Balance2;          //mass conservation

	if (Balance < -0.001) {  //reducing propotional to density
		F = 0;
		for (i = 0; i < Lu; i++) F += Snow_Density[i];
		F = Balance / (F * Lu_Depth);

		for (i = 0; i < Lu; i++) Snow_Density[i] += Snow_Density[i] * F;
	}
	else if (Balance > 0.001) {		//increasing propotional to 0.55-Density
		F = 0;
		for (i = 0; i < Lu; i++) F += 0.55 - Snow_Density[i];

		if (F > 0) F = Balance / (F * Lu_Depth);
		else F = 0;

		for (i = 0; i < Lu; i++) Snow_Density[i] += (0.55 - Snow_Density[i]) * F;
	}
}

int Find_Snow_Layer(double Depth, int Layers, double* XX)
{
	int i, L;
	double d = 0.001;

	if (Depth <= XX[0] + d) L = 0;
	else if (Depth >= XX[Layers - 1] + d) L = Layers;  //last layer
	else {
		for (i = 0; i < Layers - 1; i++)
			if (Depth > XX[i] && Depth <= XX[i + 1] + d) {
				L = i + 1;
				break;
			}
	}
	return L;
}


/*
int NEST::Find_Soil_Layer(double DD)
{
   int i, L;
   double F;

   if(DD<=Depth[0]) L=0;
   else {
	   L = Total_Layers;
	   F = 0;
	   for (i=0; i<Total_Layers-1; i++) {
		   if(DD>=F && DD<=F+Depth[i+1]) {
			   L = i+1;
			   break;
		   }
		   F += Depth[i];
	   }
   }

   return L;
}

*/

void APT_Thawing_Freezing(double apT, double LastT, double LastFice, double SW, double HC, double Mid[])
{
	//Using apparent temprature determining T 
	double T, T1, Fice;
	double F, Q1, Q2, X, Y, Z, D, E;

	//checking Fice and the corresponding T
	//   if(LastT>Para_Tm && LastT<=0) {
	//	   double B, TT;
	//	   B = Para_c + Para_a*(Para_b - LastFice);
	//	   TT = (-B+sqrt(B*B - 4*Para_AA * LastFice))/(2*Para_AA);
	//	   if (fabs(TT-LastT)>0.00001) 
	//		   TT = TT;
	//	}

	F = Water_Ice * SW;
	Q1 = F * LastFice;        //heat required
	Q2 = F * (1 - LastFice);  //heat required
	if (apT >= Q1 / HC) {  //all thawing
		Fice = 0.0;
		T = apT - Q1 / HC;
	}
	else if (apT <= Para_Tm - Q2 / HC) {  //all frozen
		Fice = 1;
		T = apT + Q2 / HC;
	}
	else if (LastT <= Para_Tm && apT <= Para_Tm) {  //no thawing/freezing
		T = apT;
		Fice = 1.0;
	}
	else {
		if (LastT > 0)            T1 = 0;
		else if (LastT > Para_Tm) T1 = LastT;
		else 			         T1 = Para_Tm;

		X = HC / (Water_Ice * SW);
		Y = Para_c * T1 + X * apT + Para_b / (1 - Para_a * T1);
		Z = Para_c + X;

		D = Para_a * Z;
		E = -Z - Para_a * Y;
		F = Y - Para_b;

		T = (-E - sqrt(E * E - 4 * D * F)) / (2 * D);
		Fice = Para_b - Para_c * T - Para_b / (1 - Para_a * T);
	}
	Mid[0] = T;
	Mid[1] = Fice;
}

#define  Temperature_Hourly_A 1.6
//#define  Temperature_Hourly_B 2.1
#define  Temperature_Hourly_C -0.18
double Temperature_Hourly(double Day_Length, double Tsunset_Yesterday, double Tmax_Today, double Tmin_Today, double Tmin_Nextday, double* Tair_Hr)
// Calculating diurnal variation of temperature
//For using actual daily data: we may need to consider next day's Tmin for T declining period (from Tmax to Tmin)
{
	int I;
	double Time;
	double Sun_Rise, Sun_Set, Tsunset;  //A = 1.6, B = 2.1, C = -0.18,
	double BBD, Temp;

	// Tsunset: Temperature at sun set.
	Sun_Rise = 12.0 - Day_Length / 2.0 + Temperature_Hourly_C;
	Sun_Set = 12.0 + Day_Length / 2.0;

	BBD = Day_Length - Temperature_Hourly_C;
	Tsunset = Tmin_Nextday + (Tmax_Today - Tmin_Nextday) * sin(Pi * BBD / (Day_Length + 2.0 * Temperature_Hourly_A));             //T at sunset today

	for (I = 0; I < N_Steps; I++) {    //N_NEST_Steps: 48, NEST_Step: 30 minutes
		Time = I * TimeStep / 3600.0;

		if (Time >= Sun_Rise && Time <= 12) {
			BBD = Time - Sun_Rise;
			Temp = Tmin_Today + (Tmax_Today - Tmin_Today) * sin(Pi * BBD / (Day_Length + 2.0 * Temperature_Hourly_A));
		}
		else if (Time > 12 && Time <= Sun_Set) {
			BBD = Time - Sun_Rise;
			Temp = Tmin_Nextday + (Tmax_Today - Tmin_Nextday) * sin(Pi * BBD / (Day_Length + 2.0 * Temperature_Hourly_A));
		}
		else if (Time > Sun_Set) {
			BBD = Time - Sun_Set;
			//	   Temp = Tmin_Nextday+(Tsunset-Tmin_Nextday) * exp(-B*BBD/(24.0-Day_Length) );
			Temp = Tsunset + (Tmin_Nextday - Tsunset) / (24 - Day_Length) * BBD;
		}
		else if (Time < Sun_Rise) {
			BBD = Time + 24.0 - Sun_Set;
			Temp = Tsunset_Yesterday + (Tmin_Today - Tsunset_Yesterday) / (24 - Day_Length) * BBD;
			//	   Temp= Tmin_Lastday+(Tsunset-Tmin_Yesterday) * exp(-B*BBD/(24.0-Day_Length) );
		}
		Tair_Hr[I] = Temp;
	}
	return Tsunset;
}


void rc(int Landcover, double C_Leaf_max, double LAI, double LE_Surface_Dew, double Intercept, double Snow, double Pond, double Soil_Temp0,
	double  SW0, double  Poro0, double  SWrootzone, double  FCrootzone,
	double  Radiation, double  Temp, double  Tmin, double  VPD, double R[])
	//canopy resistance/conductance, based on BEPS
	//R[0]: canopy and soil resistance
	//R[1]: soil surface resistance
{
	double  r_soil, C_stoma, C_canopy;
	double  f_rad, f_Temp, f_Tmin, f_lwp, f_vpd;
	double P_open, P_close, F;

	if (Intercept > 1.0e-5) {   //free water avaliable
		C_canopy = C_Leaf_Boundary * LAI * 0.1;
		r_soil = 1.0e10;     //almost no evaporation from soil
	}
	else {
		//surface resistance
		//   F = Poro0-SW0;               //Kondo et al., 1990
		//   if(F<=0.0) r_soil=1.0e-10;              //=0
		//   else       r_soil=3.0e10*pow(F, 16.6);
		if (Snow + Pond > 0.0 || Water_Table < 0 || LE_Surface_Dew>0.1) r_soil = 10.0;
		else if (Soil_Temp0 < 0.0)  r_soil = 1.0e10;
		else if (Water_Table < 10)  r_soil = 3.5 * Water_Table + 10.0;
		//	 else  r_soil= 3.5*Poro0/SW0 * Poro0/SW0 + 33.5;  //Shu Fen Sun 3.5*pow(P/SW, 2.3) +33.5, top 3cm soil layer
		else  r_soil = 3.5 * pow(Poro0 / SW0, 11.5) + 33.5;  //2.3*5=11.5 assuming F_3cm = pow(F_10cm, 5),  

   //radiation effects 
		F = 0.01 * 0.5 * 4.55 * Radiation;
		f_rad = F / (1.0 + F);

		//Temperature effects
		if (Temp < 0.0 || Temp>40) f_Temp = 0.0;
		else if (Temp < 20.0)      f_Temp = log(Temp + 1) / 3.0445;  //log(20+1)=3.0445
		else                    f_Temp = cos(Pi / 2 * (Temp - 20) / 20);             //Tmax-Topt=20

	 //Freezing effects
		if (Tmin > 0.0) f_Tmin = 1;
		else if (Tmin > -8) f_Tmin = 1 + 0.125 * Tmin;
		else f_Tmin = 0.0;

		//leaf water potential
		P_open = -1.0;
		if (Landcover == 1) P_close = -8;  //conifer
		else P_close = -4;

		F = -1.0 / (SWrootzone / FCrootzone + 0.0001);  //1-0.2
		f_lwp = (P_close - F) / (P_close - P_open);

		if (f_lwp > 1.0) f_lwp = 1.0;
		else if (f_lwp < 0.0) f_lwp = 0.0;

		//VDP effects
		if (Landcover == 1) f_vpd = 1.0 / (1.0 + 0.076 * VPD);         //conifer
		else if (Landcover == 2) f_vpd = 1.0 / (1.0 + 0.076 * 3 * VPD);  //decidous  //0.429
		else f_vpd = 0.5 * (1.0 / (1.0 + 0.076 * VPD) + 1.0 / (1.0 + 0.429 * VPD)); //mix

		C_stoma = C_Leaf_max * f_rad * f_Temp * f_Tmin * f_lwp * f_vpd;  //stoma conductance

	 //   if(Landcover==6) C_stoma *=0.1;  //Wapusk, bog
		if (Pond > 0) C_canopy = C_Leaf_Boundary * LAI;
		else {
			if (SWrootzone < 0.1) C_canopy = 0;
			else C_canopy = LAI * C_Leaf_Boundary * (C_stoma + C_Leaf_cuticular) /
				(C_Leaf_Boundary + C_stoma + C_Leaf_cuticular);
		}
	}
	R[0] = 1.0 / (C_canopy + 1.0e-6);
	R[1] = r_soil;
	return;
}


double Penman_ET(double Tair, double VPD, double Rn_G, double ra, double rc)
/*  penmon-monteith function */
{
	double	c1, c2;
	double 	t1, t2, gamma, slope, L;
	double	pa, penmon;

	gamma = 0.646 + 0.0006 * Tair;
	pa = 1.292 - 0.00428 * Tair;

	L = (2.501 - 0.0024 * Tair) * 1.0e+6;      //latent heat: J/kg

	t1 = Tair + 0.5;
	t2 = Tair - 0.5;

	c1 = (17.269 * t1) / (237.0 + t1);
	c2 = (17.269 * t2) / (237.0 + t2);

	slope = (exp(c1) - exp(c2)) * 6.1078;

	penmon = (slope * Rn_G + 1010 * pa * VPD / ra) / (slope + gamma * (1.0 + rc / ra)); //J/m2/s

	return penmon;           //J/m2/s
}


void Soil_Water_Distrib(int ET_Layers, int Wflux_Layers, double ET_Soil, double Evap)
//water uptake and redistribution
{
	//Porosity :       Hydro_P[0];
	//Field_Capacity:  Hydro_P[1];
	//Wilting_Point :  Hydro_P[2];
	//F_sat            Hydro_P[3]    //cm
	//K_sat            Hydro_P[4]    //cm/min
	//b	               Hydro_P[5]

	int  L, L1, kk;
	int IF_Thaw[maxSoilL];
	double  K[maxSoilL], avg_K, Fi, m, n, B, Fi1, m1, n1, B1; //Suction[maxSoilL],
	double  ET_x, F, F1;
	double Water[maxSoilL];
	double Delta, DD, Suc, Suc1, dSuc0, dSuc, Flow, Flow1,
		W, SW, SW_Last, SW1, SW1_Last, Time_Remain, TT, Factor;
	//  double Total_SW1, Total_SW2, Diff;

	for (L = 0; L < Wflux_Layers; L++) {
		if (Tsoil[L] >= 0) IF_Thaw[L] = 1;
		else IF_Thaw[L] = 0;
	}
	//---ET uptake from soil profile
	if (Evap > 0.0) {
		F1 = 0.0;
		for (L = 0; L < 1; L++) {
			if (Soil_Water[L] > Wilting_Point[L]) F1 += (Soil_Water[L] - Wilting_Point[L]) * DepthFsoil[L];
		}
		if (F1 > 0.0) {
			F = Evap * 0.1 / F1;
			if (F > 1.0) F = 1.0;

			for (L = 0; L < 1; L++) {
				F1 = (Soil_Water[L] - Wilting_Point[L]) * DepthFsoil[L];
				if (F1 > 0) Soil_Water[L] -= F1 * F / DepthFsoil[L];
			}
		}
	}    //end of Evap>0.0

	if (ET_Soil > 0.0) {
		//ET from top layer with Soil_Water > Field_Capacity without considering root fraction
		ET_x = ET_Soil * 0.1;  //in cm
		for (L = 0; L < 3; L++) {
			if (ET_x > 0.0 && Soil_Water[L] > Field_Capacity[L]) {
				F = (Soil_Water[L] - Field_Capacity[L]) * DepthFsoil[L];
				if (F > ET_x) {
					Soil_Water[L] -= ET_x / DepthFsoil[L];
					ET_x = 0.0;
				}
				else {
					Soil_Water[L] -= F / DepthFsoil[L];
					ET_x -= F;
				}
			}
		}

		// uptake based on root fraction
		if (ET_x > 0) {
			F1 = 0.0;
			for (L = 0; L < ET_Layers; L++) {
				if (Soil_Water[L] > Wilting_Point[L]) F1 += (Soil_Water[L] - Wilting_Point[L]) * DepthFsoil[L] * Root_Frac[L];
			}
			if (F1 > 0.0) {
				F = ET_x / F1;
				if (F > 1.0) F = 1.0;
				for (L = 0; L < ET_Layers; L++) {
					F1 = (Soil_Water[L] - Wilting_Point[L]) * DepthFsoil[L] * Root_Frac[L]; //*ET_Uptake_Frac[L];
					if (F1 > 0) Soil_Water[L] -= F1 * F / DepthFsoil[L];
				}
			}
		}    //if ET_x >0

	}    //end of ET_Soil>0.0

  //  Total_SW1= 0;
  //  for(L=0; L<Wflux_Layers; L++) Total_SW1 += Soil_Water[L]*DepthFsoil[L];

  //--- water flux: Campbell, 1974, Clapp & Hornberger, 1978
	for (L = 0; L < Wflux_Layers; L++) {

		Water[L] = Soil_Water[L];

		W = Soil_Water[L] / Porosity[L];
		if (W > 1.0) W = 1.0;
		F = pow(W, 2.0 * Water_b[L] + 3.0);
		//		  if(F<0.001) F = 0.001;
		if (Fwater[L] < 1.0) K[L] = 0.0;        //occurs when all thaw
		K[L] = F * KsFsoil[L];    //Water_Ks[L] * pow(Fsoil[L], -2.6);

		if (W < Soil_Water_Wi) Suction[L] = Water_Fs[L] * pow(W, -Water_b[L]);
		else {
			Fi = Water_Fs[L] * pow(Soil_Water_Wi, -Water_b[L]);
			m = Fi / 0.0064 - Fi * Water_b[L] / 0.0736;   //(1.0-Wi)**2=0.0064, Wi((1-Wi)=0.0736
			n = 2.0 * Soil_Water_Wi - Fi * Water_b[L] / (m * Soil_Water_Wi) - 1.0;
			Suction[L] = m * (W - n) * (1.0 - W);
		}  //else
	}  //for L

	kk = Wflux_Layers - 1;
	for (L = 0; L < kk; L++) {
		L1 = L + 1;
		if (IF_Thaw[L] + IF_Thaw[L1] == 2) {
			Suc = Suction[L];
			Suc1 = Suction[L1];
			DD = (Depth[L] + Depth[L1]) / 2.0;

			dSuc0 = Suction[L] - Suction[L1] - DD;

			if (dSuc0 > 1 || (dSuc0 < -1 && Suc1 > 0.0001)) {
				Delta = 0.01;

				avg_K = (K[L] + K[L1]) / 2;

				SW = Water[L];
				SW1 = Water[L1];

				Fi = Water_Fs[L] * pow(Soil_Water_Wi, -Water_b[L]);
				Fi1 = Water_Fs[L1] * pow(Soil_Water_Wi, -Water_b[L1]);

				m = Fi / 0.0064 - Fi * Water_b[L] / 0.0736;   //(1.0-Wi)**2=0.0064, Wi((1-Wi)=0.0736
				m1 = Fi1 / 0.0064 - Fi1 * Water_b[L1] / 0.0736;   //(1.0-Wi)**2=0.0064, Wi((1-Wi)=0.0736

				n = 2.0 * Soil_Water_Wi - Fi * Water_b[L] / (m * Soil_Water_Wi) - 1.0;
				n1 = 2.0 * Soil_Water_Wi - Fi1 * Water_b[L1] / (m1 * Soil_Water_Wi) - 1.0;

				B = m * (1 + n);
				B1 = m1 * (1 + n1);

				SW_Last = SW;
				SW1_Last = SW1;

				Time_Remain = TimeStep;

				if (dSuc0 > 1) {   //UP ward flow, using layer L to determine flow
					Factor = DepthFsoil[L1] / DepthFsoil[L] * 0.8;
					if (Factor > 1) Factor = 1;

					dSuc = dSuc0;
					while (dSuc > 1.0 && Time_Remain > 0) {
						SW_Last = SW;
						SW1_Last = SW1;

						Suc -= dSuc * Delta;

						W = pow(Suc / Water_Fs[L], -1.0 / Water_b[L]);
						if (W >= Soil_Water_Wi) W = (B + sqrt(B * B - 4 * m * (m * n + Suc))) / (2 * m);
						SW = W * Porosity[L];
						Flow = (SW - SW_Last) * DepthFsoil[L];

						if (Flow < 0) {
							Time_Remain = 0;
							SW = SW_Last;
						}
						else {
							SW1 = SW1_Last - Flow * Factor / DepthFsoil[L1];  //mofify the flow to make sure  
							Flow1 = (SW1_Last - SW1) * DepthFsoil[L1];  //same SW1 but justified using Fsoil 
							W = SW1 / Porosity[L1];
							if (W < Soil_Water_Wi) Suc1 = Water_Fs[L1] * pow(W, -Water_b[L1]);
							else     Suc1 = m1 * (W - n1) * (1.0 - W);

							if (Flow > Flow1) {  //adjust Flow
								Flow = Flow1;
								SW = SW_Last + Flow / DepthFsoil[L];
								W = SW / Porosity[L];
								if (W < Soil_Water_Wi) Suc = Water_Fs[L] * pow(W, -Water_b[L]);
								else     Suc = m * (W - n) * (1.0 - W);
							}

							F = (dSuc + (Suc - Suc1 - DD)) / 2.0;
							if (F < 0.5) F = 0.5;
							TT = Flow / (avg_K * F / DD) * 60.0;  //time (in sec) need to flow thay amount 

							if (TT > Time_Remain) {
								TT = TT - Time_Remain;  //reduce this extra flow
								Flow = TT / 60.0 * (avg_K * F / DD);
								SW -= Flow / DepthFsoil[L];
								SW1 += Flow / DepthFsoil[L1];

								Time_Remain = -999.9;  //for exit
							}
							else {
								Time_Remain -= TT;
								dSuc = Suc - Suc1 - DD;

								Delta = Time_Remain / TT * 0.1;

								if (Delta > 0.5) Delta = 0.5;

								SW_Last = SW;
								SW1_Last = SW1;
							}
						}  //else Flow>0
					}   //while
				}  //if dSuc0 > 1
				else if (dSuc0 < -1 && Suc1 > 0.0001) {   //downward flow. Layer L slightly drier than layer L1
					Factor = DepthFsoil[L] / DepthFsoil[L1] * 0.5;
					if (Factor > 1) Factor = 1;

					dSuc = -dSuc0;
					while (dSuc > 1 && Time_Remain > 0) {

						if (dSuc <= DD * 2) {     //DOWN ward, Suc1<DD*2, using layer L to determine flow
							Suc += dSuc * Delta;
							W = pow(Suc / Water_Fs[L], -1.0 / Water_b[L]);
							if (W >= Soil_Water_Wi) W = (B + sqrt(B * B - 4 * m * (m * n + Suc))) / (2 * m);

							SW = W * Porosity[L];
							Flow = (SW_Last - SW) * DepthFsoil[L];

							SW1 = SW1_Last + Flow / DepthFsoil[L1];
							if (SW1 >= Porosity[L1]) {
								Flow1 = (Porosity[L1] - SW1_Last) * DepthFsoil[L1];
								SW1 = Porosity[L1];
								Suc1 = 0;

								Flow = Flow1;
								SW = SW_Last - Flow / DepthFsoil[L];
								//						W = SW1/Porosity[L1];
								//						if(W<Soil_Water_Wi) Suc1 = Water_Fs[L1]*pow(W, -Water_b[L1]);
								//						else     Suc1 = m1*(W-n1)*(1.0-W);
								//                      ignor time check as usually time is not a limit 

								Time_Remain = -999.9;  //for exit, time out before saturation
							}  //if SW1 >= Porosity[L1]
							else {   //SW1<Porosity
								W = SW1 / Porosity[L1];
								if (W < Soil_Water_Wi) Suc1 = Water_Fs[L1] * pow(W, -Water_b[L1]);
								else     Suc1 = m1 * (W - n1) * (1.0 - W);

								F = (dSuc + (Suc1 + DD - Suc)) / 2.0;
								if (F < 0.5) F = 0.5;
								TT = Flow / (avg_K * F / DD) * 60.0;  //time (in sec) need to flow thay amount 

								if (TT > Time_Remain) {
									TT = TT - Time_Remain;  //reduce this extra flow
									Flow = TT / 60.0 * (avg_K * F / DD);
									SW += Flow / DepthFsoil[L];
									SW1 -= Flow / DepthFsoil[L1];

									Time_Remain = -999.9;  //for exit
								}
								else {
									Time_Remain -= TT;
									dSuc = Suc1 + DD - Suc;

									Delta = Time_Remain / TT * 0.1;

									if (Delta > 0.5) Delta = 0.5;

									SW_Last = SW;
									SW1_Last = SW1;
								}
							}  //else
						}  //else if(dSuc <= DD
						else {		//DOWN ward, dSuc1<-Depth[L]*2, Layer L is wetter than layer L1, using layer L1 to determine flow

							Suc1 -= (dSuc - DD) * Delta;

							W = pow(Suc1 / Water_Fs[L1], -1.0 / Water_b[L1]);
							if (W >= Soil_Water_Wi) W = (B1 + sqrt(B1 * B1 - 4 * m1 * (m1 * n1 + Suc1))) / (2 * m1);

							SW1 = W * Porosity[L1];
							Flow1 = (SW1 - SW1_Last) * DepthFsoil[L1];

							SW = SW_Last - Flow1 * Factor / DepthFsoil[L]; //modified to make sure  
							if (SW < 0.01) {
								SW = SW_Last;
								Time_Remain = 0;  //for exit
							}

							Flow = (SW_Last - SW) * DepthFsoil[L];  //same SW but justified using Fsoil 

							W = SW / Porosity[L];
							if (W < Soil_Water_Wi) Suc = Water_Fs[L] * pow(W, -Water_b[L]);
							else     Suc = m * (W - n) * (1.0 - W);

							if (Flow1 > Flow) {
								Flow1 = Flow;
								SW1 = SW1_Last + Flow1 / DepthFsoil[L1];

								W = SW1 / Porosity[L1];
								if (W < Soil_Water_Wi) Suc1 = Water_Fs[L1] * pow(W, -Water_b[L1]);
								else     Suc1 = m1 * (W - n1) * (1.0 - W);
							}

							F = (dSuc + (Suc1 + DD - Suc)) / 2.0;
							if (F < 0.1) F = 0.1;
							TT = Flow / (avg_K * F / DD) * 60.0;  //time (in sec) need to flow thay amount 

							if (TT >= Time_Remain) {
								TT = TT - Time_Remain;  //reduce this extral flow
								Flow = TT / 60.0 * (avg_K * F / DD);
								SW += Flow / DepthFsoil[L];
								SW1 -= Flow / DepthFsoil[L1];

								Time_Remain = -999.9;  //for exit
							}
							else {
								Time_Remain -= TT;
								dSuc = Suc1 + DD - Suc;

								Delta = Time_Remain / TT * 0.1;

								if (Delta > 0.5) Delta = 0.5;

								SW_Last = SW;
								SW1_Last = SW1;
							}
						}  //else if(dSuc < D
					}   //while
				}  //else if(dSuc0 < -1)
				Water[L] = SW;
				Water[L1] = SW1;

				W = Water[L1] / Porosity[L1];
				if (W < Soil_Water_Wi) Suction[L1] = Water_Fs[L1] * pow(W, -Water_b[L1]);
				else     Suction[L1] = m1 * (W - n1) * (1.0 - W);

			}  //if(dSuc>1 || dSuc<-1) 
		}  // if IF_Thaw
	}

	//  Total_SW2= 0;
	//  for(L=0; L<Wflux_Layers; L++) Total_SW2 += Water[L]*DepthFsoil[L];
	//  Diff = Total_SW2-Total_SW1;
	//  if(fabs(Diff)>0.0000001) 
	//	  F=Diff;

	for (L = 0; L < Wflux_Layers; L++) {
		Soil_Water[L] = Water[L];
		//		  if(Soil_Water[L]<Wilting_Point[L])   Soil_Water[L] =Wilting_Point[L];
	}
}

void Soil_Daily_Gravity_Drainage()
{
	int L, L1, Soil_Layers_1 = Soil_Layers - 1;
	double F, F1, F_Drain;

	F = 0;
	for (L = 0; L < Soil_Layers_1; L++) {
		L1 = L + 1;
		if (Tsoil[L] >= 0) {
			Soil_Water[L] += F / (Depth[L] * Fsoil[L]);
			F = 0;
			if (Soil_Water[L] > Field_Capacity[L]) {
				if (Soil_Water[L] > Porosity[L]) {
					F = (Soil_Water[L] - Porosity[L]) * Depth[L] * Fsoil[L];
					Soil_Water[L] = Porosity[L];
				}
				F_Drain = 0.5 * (Soil_Water[L] - Field_Capacity[L]) / (Porosity[L] - Field_Capacity[L]);  //daily draining rate
				F1 = (Soil_Water[L] - Field_Capacity[L]) * Depth[L] * Fsoil[L] * F_Drain; //drainage due to above field capacity

				if (Tsoil[L1] >= 0 && F1 > 0) {
					Soil_Water[L] -= F1 / (Depth[L] * Fsoil[L]);
					F += F1;  //total drain
				}  //if
				else break;
			}  //else
		}  //if Tsoil[L]>=0
	} //for Loop

  //fill back with the remaining water
	for (L = L1 - 1; L >= 0; L--) {
		F1 = (Porosity[L] - Soil_Water[L]) * Depth[L] * Fsoil[L];
		if (F1 > F) {
			Soil_Water[L] += F / (Depth[L] * Fsoil[L]);
			F = 0;
			break;
		}
		else {
			Soil_Water[L] = Porosity[L];
			F -= F1;
		}
	}  //for
}


void Interpolate_To_Daily(int* DAYS, int j, double* Monthly_Values[], double* Daily_Values)
//j is for year, Monthly value include all year's data
{
	int D, i, k;
	double Y[14], X[14];

	X[0] = -15.5;
	X[1] = 15.5;
	for (i = 2; i <= 12; i++) X[i] = X[i - 1] + (DAYS[i - 2] + DAYS[i - 1]) / 2.0;
	X[13] = X[12] + 31;

	if (j == 0) Y[0] = Monthly_Values[j][11];   //1st year 
	else     Y[0] = Monthly_Values[j - 1][11];    //Dec. value in last year

	for (i = 1; i <= 12; i++) Y[i] = Monthly_Values[j][i - 1];

	if (j == YEARS - 1) Y[13] = Monthly_Values[j][0];  //last year
	else Y[13] = Monthly_Values[j + 1][0];        //Jan. value in next year

	D = 365 + Current_Leap_Year_Flag;
	for (k = 0; k < D; k++) {
		for (i = 0; i < 13; i++) {
			if (k >= X[i] && k < X[i + 1]) {
				Daily_Values[k] = Y[i] + (Y[i + 1] - Y[i]) / (X[i + 1] - X[i]) * (k - X[i]);
				i = 13;  //exit for loop
			}
		}
	}

}

//-------------------
void Interpolate_To_Daily1(int* DAYS, double* Monthly_Values, double* Daily_Values)
{
	int D, i, k;
	double Y[14], X[14];

	X[0] = -15.5;
	X[1] = 15.5;
	for (i = 2; i <= 12; i++) X[i] = X[i - 1] + (DAYS[i - 2] + DAYS[i - 1]) / 2.0;
	X[13] = X[12] + 31;

	Y[0] = Monthly_Values[11];
	for (i = 1; i <= 12; i++) Y[i] = Monthly_Values[i - 1];
	Y[13] = Monthly_Values[0];

	D = 365 + Current_Leap_Year_Flag;
	for (k = 0; k < D; k++) {
		for (i = 0; i < 13; i++) {
			if (k >= X[i] && k < X[i + 1]) {
				Daily_Values[k] = Y[i] + (Y[i + 1] - Y[i]) / (X[i + 1] - X[i]) * (k - X[i]);
				i = 14;  //exit for loop
			}
		}
		if (Daily_Values[k] < -9000.0) Daily_Values[k] = -9999.0;
	}

}

int Leap_Year(int Year)
{
	int i;
	if (Year == 1900 || Year == 2100) i = 0;   //not a leap year
	else {
		if ((Year % 4) == 0) i = 1;
		else i = 0;
	}
	//  i=0;  //not considered
	return i;
}

void Veg_Parameters(int Landcover, double LAImax, double* XX)
{
	//0:Albedo, 1:extinct K, 2:C_leaf_max, 3:woody area index, 4:LAImin (in winter)  
	//5:GDD foliage start,   6:GDD folage end, 7: clumping index	
	switch (Landcover) {
	case 1:            //coniferous
		XX[0] = 0.1;  XX[1] = 0.5; XX[2] = 0.01; XX[3] = LAImax * 0.5;
		XX[4] = LAImax * 0.7; XX[5] = 200.0; XX[6] = 500; XX[7] = 0.585;  //Clumping index from Leblanc et al., Can.J. RS, 31(364-376),2005
		XX[8] = -8.0; XX[9] = 0.076;      //Potential for stamata closure, VPD effect parameter
		break;
	case 2:                //broad leaf
		XX[0] = 0.15;  XX[1] = 0.58; XX[2] = 0.05; XX[3] = LAImax * 0.5;
		XX[4] = 0;     XX[5] = 200.0; XX[6] = 400; XX[7] = 0.72;
		XX[8] = -4.0;  XX[9] = 0.429;
		break;
	case 3:                //mixed forest
		XX[0] = 0.12;  XX[1] = 0.54; XX[2] = 0.025; XX[3] = LAImax * 0.5;
		XX[4] = LAImax * 0.3; XX[5] = 300.0;  XX[6] = 500; XX[7] = 0.735;
		XX[8] = -6.0;  XX[9] = 0.2;
		break;
	case 4:                //shrub
		XX[0] = 0.20;  XX[1] = 0.58; XX[2] = 0.05; XX[3] = LAImax * 0.25;
		XX[4] = 0;     XX[5] = 200.0; XX[6] = 400; XX[7] = 0.735;
		XX[8] = -4.0;  XX[9] = 0.429;
		break;
	case 5:                //crop and grass
		XX[0] = 0.20;  XX[1] = 0.58; XX[2] = 0.05; XX[3] = 0;
		XX[4] = 0;     XX[5] = 100.0; XX[6] = 500; XX[7] = 0.895;
		XX[8] = -4.0;  XX[9] = 0.429;
		break;
	default:                               //others
		XX[0] = 0.12;  XX[1] = 0.54; XX[2] = 0.025; XX[3] = LAImax * 0.5;
		XX[4] = LAImax * 0.3;  XX[5] = 450.0;  XX[6] = 1150; XX[7] = 0.735;
		XX[8] = -6.0;  XX[9] = 0.2;
	}   //end of switch
}


void Peat_Hydro_Parameters(double Q, double* Hydro_P)
{
	//derive hydrolic parameters based on the type of peat, Interpolated from the data presented by Letts et al., 2000
	// TypeIndex: range from 0 to 3. 0.5: Fibric, 1.5: hemic, 2.5: Sapric (deccomposed increasingly)
	double X[5] = { 0,     0.5,   1.5,   2.5,   3 },    //peat type index. (<0.01 for ice)
		BD[5] = { 0.054, 0.084, 0.144, 0.204, 0.234 },   //Bulk density (g/cm3)  
//original
Poro[5] = { 0.955, 0.93,  0.88,  0.83,  0.805 },   //porosity (fraction), original, Letts etal., 2000    
FC[5] = { 0.25,  0.3,   0.4,   0.5,    0.55 },    //field capacity estimated from spacific yield, 
//		   WP[5]   = {0.2,   0.2,   0.21,  0.22,  0.255},   //wilting point  orginal

WP[5] = { 0.05,  0.10,   0.15,  0.22,  0.25 },   //YZ wilting point

Suct[5] = { 1.035, 1.03,  1.02,  1.01,  1.005 },  //suction at saturaion (cm) 
Ks[5] = { 20.0,  1.68,  0.012, 0.0006,0.0002 }, //conductivity at saturation (cm/min)
b[5] = { 1.75,  2.7,   6.1,  12.0,  17.5 };     //parameter b

	int i, k1, k2, Out;

	Out = 1;
	if (Q <= X[0])   i = 0;
	else if (Q >= X[4]) i = 4;
	else {
		Out = 0;
		for (i = 0; i < 4; i++) {
			if (Q >= X[i] && Q <= X[i + 1]) {
				k1 = i;
				k2 = i + 1;
				break;
			}
		}

		Hydro_P[0] = Poro[k1] + (Poro[k2] - Poro[k1]) / (X[k2] - X[k1]) * (Q - X[k1]);  //porosity
		Hydro_P[1] = FC[k1] + (FC[k2] - FC[k1]) / (X[k2] - X[k1]) * (Q - X[k1]);    //Field capacitys
		Hydro_P[2] = WP[k1] + (WP[k2] - WP[k1]) / (X[k2] - X[k1]) * (Q - X[k1]);    //Wilting point
		Hydro_P[3] = Suct[k1] + (Suct[k2] - Suct[k1]) / (X[k2] - X[k1]) * (Q - X[k1]);    //suction at saturation

		Hydro_P[4] = exp(log(Ks[k1]) + (log(Ks[k2]) - log(Ks[k1])) / (X[k2] - X[k1]) * (Q - X[k1]));    //conductivity at saturation
		Hydro_P[5] = exp(log(b[k1]) + (log(b[k2]) - log(b[k1])) / (X[k2] - X[k1]) * (Q - X[k1]));    //b

		Hydro_P[6] = BD[k1] + (BD[k2] - BD[k1]) / (X[k2] - X[k1]) * (Q - X[k1]);    //Bulk density
	}

	if (Out == 1) {
		Hydro_P[0] = Poro[i];
		Hydro_P[1] = FC[i];
		Hydro_P[2] = WP[i];
		Hydro_P[3] = Suct[i];
		Hydro_P[4] = Ks[i];
		Hydro_P[5] = b[i];
		Hydro_P[6] = BD[i];
	}
}


void Hydro_Parameters(int TextureID, double* Hydro_P)
//Porosity :       Hydro_P[0];
//Field_Capacity:  Hydro_P[1];
//Wilting_Point :  Hydro_P[2];
//F_sat            Hydro_P[3]    //cm
//K_sat            Hydro_P[4]    //cm/min
//b	               Hydro_P[5]
//Bulk density     Hydro_P[6]   //g/cm3
{
	switch (TextureID) {
	case 1:              //forest floor
		Hydro_P[0] = 0.93;	//Porosity, 0.95
		Hydro_P[1] = 0.35;	//Field_Capacity 0.35
		Hydro_P[2] = 0.04;	//Wilting_Point 0.08
		Hydro_P[3] = 1.03;    //Suction at saturation cm
		Hydro_P[4] = 0.28;    //Conductivity at saturation, cm/min
		Hydro_P[5] = 2.7;     //parameter b
		Hydro_P[6] = 0.05;	//Bulk density 
		break;
	case 2:		//peat layers, not used 
		Hydro_P[0] = 0.88;	//Porosity,
		Hydro_P[1] = 0.39;	//Field_Capacity
		Hydro_P[2] = 0.15;	//Wilting_Point
		Hydro_P[3] = 1.02;    //Suction at saturation cm
		Hydro_P[4] = 0.012;    //Conductivity as saturation, cm/min
		Hydro_P[5] = 6.1;     //parameter b
		Hydro_P[6] = 0.84;	//Bulk density 
		break;
	case 3:     //sand (92%, 3%)
		Hydro_P[0] = 0.326;	//Porosity,
		Hydro_P[1] = 0.116;	//Field_Capacity
		Hydro_P[2] = 0.045;	//Wilting_Point
		Hydro_P[3] = 3.5;     //Suction at saturation cm
		Hydro_P[4] = 1.056;   //12.043/60; // 1.056; Conductivity as saturation, cm/min
		Hydro_P[5] = 4.05;     //parameter b
		Hydro_P[6] = 1.79;	 //Bulk density 
		break;
	case 4:		//loamy sandy (81, 6)
		Hydro_P[0] = 0.373;	//Porosity,
		Hydro_P[1] = 0.153;	//Field_Capacity
		Hydro_P[2] = 0.067;	//Wilting_Point
		Hydro_P[3] = 1.78;    //Suction at saturation cm
		Hydro_P[4] = 0.938;   //5.648/60;  //0.938; Conductivity at saturation, cm/min
		Hydro_P[5] = 4.38;     //parameter b
		Hydro_P[6] = 1.66;		//Bulk density 
		break;
	case 5:	//sandy loam 64	11	0.418	0.200	0.093	2.317	1.54
		Hydro_P[0] = 0.418;		//Porosity,
		Hydro_P[1] = 0.200;		//Field_Capacity
		Hydro_P[2] = 0.093;	//Wilting_Point
		Hydro_P[3] = 7.18;     //Suction at saturation cm
		Hydro_P[4] = 0.208;   //2.317/60;  //0.208; Conductivity as saturation, cm/min
		Hydro_P[5] = 4.90;     //parameter b
		Hydro_P[6] = 1.54;	//Bulk density 
		break;
	case 6: //loam (41	19	0.465	0.258	0.122	0.959	1.42
		Hydro_P[0] = 0.465;		//Porosity,
		Hydro_P[1] = 0.258;		//Field_Capacity
		Hydro_P[2] = 0.122;	//Wilting_Point
		Hydro_P[3] = 14.6;     //Suction at saturation cm
		Hydro_P[4] = 0.0417;   //0.959/60;  //0.0417, Conductivity as saturation, cm/min
		Hydro_P[5] = 5.39;     //parameter b
		Hydro_P[6] = 1.42;		//Bulk density
		break;
	case 7: //silt loam (22	13	0.458	0.282	0.102	2.239	1.44
		Hydro_P[0] = 0.458;		//Porosity,
		Hydro_P[1] = 0.282;		//Field_Capacity
		Hydro_P[2] = 0.102;	//Wilting_Point
		Hydro_P[3] = 56.6;     //Suction at saturation cm
		Hydro_P[4] = 0.0432;   //2.239/60;  //0.0432; Conductivity as saturation, cm/min
		Hydro_P[5] = 5.30;     //parameter b
		Hydro_P[6] = 1.44;	     //Bulk density
		break;
	case 8:		//silt clay loam(10	34	0.520	0.362	0.188	0.454	1.27
		Hydro_P[0] = 0.520;		//Porosity,
		Hydro_P[1] = 0.362;	//Field_Capacity
		Hydro_P[2] = 0.188;	//Wilting_Point
		Hydro_P[3] = 14.6;     //Suction at saturation cm
		Hydro_P[4] = 0.0102;   //0.454/60;  //0.0102; Conductivity as saturation, cm/min
		Hydro_P[5] = 7.75;     //parameter b
		Hydro_P[6] = 1.27;		//Bulk density
		break;
	case 9:     //clay loam(33	33	0.502	0.321	0.184	0.307	1.32
		Hydro_P[0] = 0.502;		//Porosity,
		Hydro_P[1] = 0.321;		//Field_Capacity
		Hydro_P[2] = 0.184;	//Wilting_Point
		Hydro_P[3] = 36.1;     //Suction at saturation cm
		Hydro_P[4] = 0.0147;   //0.307/60;    //0.0147; Conductivity as saturation, cm/min
		Hydro_P[5] = 8.52;     //parameter b
		Hydro_P[6] = 1.32;		//Bulk density
		break;
	case 10:  // silt clay (6	47	0.541	0.435	0.272	0.289	1.22
		Hydro_P[0] = 0.541;		//Porosity,
		Hydro_P[1] = 0.435;		//Field_Capacity
		Hydro_P[2] = 0.272;	//Wilting_Point
		Hydro_P[3] = 17.4;     //Suction at saturation cm
		Hydro_P[4] = 0.0062;   //0.289/60;  //0.0062; Conductivity as saturation,  cm/min
		Hydro_P[5] = 10.4;     //parameter b
		Hydro_P[6] = 1.22;		//Bulk density
		break;
	case 11:		//sandy clay (51	42	0.502	0.324	0.228	0.133	1.32
		Hydro_P[0] = 0.502;		//Porosity,
		Hydro_P[1] = 0.324;		//Field_Capacity
		Hydro_P[2] = 0.228;	//Wilting_Point
		Hydro_P[3] = 18.6;     //Suction at saturation cm
		Hydro_P[4] = 0.0130;   //0.133/60;   //0.0130; Conductivity as saturation, cm/min
		Hydro_P[5] = 11.4;     //parameter b
		Hydro_P[6] = 1.32;		//Bulk density
		break;
	case 12:		//clay (27	51	0.530	0.419	0.289	0.163	1.24
		Hydro_P[0] = 0.530;		//Porosity,
		Hydro_P[1] = 0.419;		//Field_Capacity
		Hydro_P[2] = 0.289;	//Wilting_Point
		Hydro_P[3] = 18.6;     //Suction at saturation cm
		Hydro_P[4] = 0.0077;   //0.163/60;  //0.0077; Conductivity as saturation,  cm/min
		Hydro_P[5] = 11.4;     //parameter b
		Hydro_P[6] = 1.24;		//Bulk density
		break;
	case 13: //Sandy clay loam (60	27	0.471	0.254	0.159	0.341	1.40
		Hydro_P[0] = 0.471;		//Porosity,
		Hydro_P[1] = 0.254;		//Field_Capacity
		Hydro_P[2] = 0.159;	//Wilting_Point
		Hydro_P[3] = 18.6;     //Suction at saturation cm
		Hydro_P[4] = 0.0378;   //0.341/60;  //0.0378; Conductivity as saturation,  cm/min
		Hydro_P[5] = 11.4;     //parameter b
		Hydro_P[6] = 1.40;		//Bulk density
		break;
	case 14: //Silt (8	5	0.415	0.315	0.096	4.320	1.55	14
		Hydro_P[0] = 0.415;		//Porosity,
		Hydro_P[1] = 0.315;		//Field_Capacity
		Hydro_P[2] = 0.096;	//Wilting_Point
		Hydro_P[3] = 18.6;     //Suction at saturation cm
		Hydro_P[4] = 4.32 / 60;  //Conductivity as saturation,  cm/min
		Hydro_P[5] = 11.4;     //parameter b
		Hydro_P[6] = 1.55;		//Bulk density
		break;
	default: //Sandy clay loam (60	27	0.471	0.254	0.159	0.341	1.40
		Hydro_P[0] = 0.471;		//Porosity,
		Hydro_P[1] = 0.254;		//Field_Capacity
		Hydro_P[2] = 0.159;	//Wilting_Point
		Hydro_P[3] = 18.6;     //Suction at saturation cm
		Hydro_P[4] = 0.0378;   //0.341/60;  //0.0378; Conductivity as saturation,  cm/min
		Hydro_P[5] = 11.4;     //parameter b
		Hydro_P[6] = 1.40;		//Bulk density
		break;
	}
}

void Monthly_Day_Length(int* DAYS, double sin_Lat, double cos_Lat, double* Day_Length)
{
	//day lenth effects;
	double DEC, SINLD, COSLD, AOB, DOY[12];
	int i;

	DOY[0] = DAYS[0] / 2.0;
	for (i = 1; i < 12; i++) DOY[i] = DOY[i - 1] + (DAYS[i - 1] + DAYS[i]) / 2.0;

	for (i = 0; i < 12; i++) {
		DEC = -23.45 * RAD * cos(2. * Pi * (DOY[i] + 10.0) / 365.0);
		SINLD = sin_Lat * sin(DEC);
		COSLD = cos_Lat * cos(DEC);
		AOB = SINLD / COSLD;
		if (AOB < -0.9999) AOB = -0.9999;
		else if (AOB > 0.9999) AOB = 0.9999;
		Day_Length[i] = 12.0 * (1.0 + 2.0 * asin(AOB) / Pi);
	}
}

void Solar_Radiation(int Leap_Year, double sin_Lat, double cos_Lat, int DOY, double* Rad_DayLength)
{
	//Calculate daily solar radiation based on latitude and DOY;
	double Day_Length, DEC, SINLD, COSLD, AOB, DSINB, SC, DS0;
	//   double a=0.18, b=0.55;
	//   double Solar_Rad;

	DEC = -23.45 * RAD * cos(2. * Pi * (DOY + 10.0) / (365. + Leap_Year));
	SINLD = sin_Lat * sin(DEC);
	COSLD = cos_Lat * cos(DEC);
	AOB = SINLD / COSLD;
	if (AOB < -0.9999) AOB = -0.9999;
	else if (AOB > 0.9999) AOB = 0.9999;
	Day_Length = 12.0 * (1.0 + 2.0 * asin(AOB) / Pi);

	//-----solar constant (SC) and daily extraterrestrial radiation (DS0)
	DSINB = 3600.0 * (Day_Length * SINLD + 24.0 * COSLD * sqrt(1. - AOB * AOB) / Pi);
	SC = 1370. * (1.0 + 0.033 * cos(2.0 * Pi * DOY / (365.0 + Leap_Year)));
	DS0 = SC * DSINB / 1000000.0;  //MJ/m2/day
	if (DS0 < 0.0) DS0 = 0.0;

	// Cloud_Fraction=0.7-((Latitude-70.0)/90.0)*((Latitude-70.0)/90.0); 	
	//Solar_Rad = DS0*(a+b*(1.0-Cloud_Fraction) );//MJ/m2/day

	Rad_DayLength[0] = DS0;
	Rad_DayLength[1] = Day_Length;
}


double Shusen_Integration_Eqs(double sin_Lat, double cos_Lat, int DOY, double Factors[])
{
	double S, C, AOB, Time, Step;
	double Declination, Altitude0, Hour_Angle,
		sin_D, cos_D, sin_Altitude0,
		Alt_Noon, cos_Alt_Noon, Alt, cos_Alt;
	double Day_Length, SunRise, SunSet, Sum0, Sum1, Sum2, P0;

	int N;

	Declination = 23.45 * RAD * sin(2 * Pi * (DOY + 284) / 365.25);
	sin_D = sin(Declination);
	cos_D = cos(Declination);

	S = sin_Lat * sin_D;
	C = cos_Lat * cos_D;

	AOB = S / C;
	if (AOB < -0.9999) Day_Length = 0.0;
	else if (AOB > 0.9999) Day_Length = 24.0;
	else Day_Length = 12.0 * (1.0 + 2.0 * asin(AOB) / Pi);

	if (Day_Length > 0.001) {
		Hour_Angle = 2 * Pi * (12 - 12.0) / 24.0;  //time = 12 (at noon)
		sin_Altitude0 = S + C * cos(Hour_Angle);
		Altitude0 = asin(sin_Altitude0);
		if (Altitude0 < 0) Altitude0 = 0;
		Alt_Noon = Pi / 2 - Altitude0;  //zenith angle
		cos_Alt_Noon = cos(Alt_Noon);

		SunRise = 12.0 - Day_Length / 2;
		SunSet = 12.0 + Day_Length / 2;

		Time = 0;
		N = 0;
		Step = 1.0 / 12.0;  //5 minutes

		Sum0 = 0;  //radiation at top of the atmosphere
		Sum1 = 0;
		Sum2 = 0;
		for (Time = SunRise + Step; Time < SunSet; Time += Step) {
			Hour_Angle = 2 * Pi * (Time - 12.0) / 24.0;
			sin_Altitude0 = S + C * cos(Hour_Angle);

			cos_Alt = sin_Altitude0;
			if (cos_Alt > 0) {
				Sum0 += cos_Alt;
				Altitude0 = asin(cos_Alt);
				Alt = Pi / 2 - Altitude0;

				P0 = cos((Alt - Alt_Noon) / (Pi / 2 - Alt_Noon) * Pi / 2);

				Sum1 += P0 * cos_Alt / cos_Alt_Noon;  //direct ratio
				Sum2 += P0;  //diffuse ratio

				N++;
			}
		}  //Time

		//printf("%d %lf %lf %lf\n", DOY, Alt_Noon, 1/Sum1, 1/Sum2);  
		if (Alt_Noon > 89.0 * Pi / 180.0) Alt_Noon = 89.0 * Pi / 180.0; //solar altitude at noon no less 2 degrees
		Factors[0] = Alt_Noon;

		Factors[1] = 1 / (Sum1 * Step / Day_Length);
		Factors[2] = 1 / (Sum2 * Step / Day_Length);
		Factors[3] = 1350.0 * Sum0 / N + 0.001;  //average solar radiation on the top of the atmosphere
	}
	else {
		Factors[0] = 0.0;
		Factors[1] = 0;
		Factors[2] = 0;
		Factors[3] = 0.001;
	}

	return Day_Length;
}

