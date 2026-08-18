#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void Output_AllSites_Results_For_Mapping_For_Fsnow_1yr();
void Output_AllSites_SnowDepth_April_Ob_Days();
void Output_AllSites_SnowDepth_April_5_17_avg_from_2010_2025();

void Output_AllSites_SnowEnd_Dates();
void Estimate_SnowEndData_When_It_Is_Much_Smaller_Than_The_Next();

void Monotonic_process_for_SnowEnd_Dates();
void Smooth_SnowEnd_Dates_along_aspect();
void Output_AllSites_1_year_snow_cover_dates_for_Checking();

#define YEARS 16

//max categories for generating input data files and check model output, 
#define LAI_NN 7	
#define Slope_NN 14
#define Aspect_NN 24
#define Fsnow_NN 32   

//max for mapping Fsnow
#define Sedge_LAI_NN 7	    //Total: 37632 
#define Sedge_Slope_NN 7
#define Sedge_Aspect_NN 24
#define Sedge_Fsnow_NN 32   

#define Shrub_LAI_NN 7	            //Total: 75264 
#define Shrub_Slope_NN 14
#define Shrub_Aspect_NN 24
#define Shrub_Fsnow_NN 32   

#define Total_Rows 1
#define Total_Columns 1

int main()
{
//	Output_AllSites_Results_For_Mapping_For_Fsnow_1yr();  //old
 
//	Monotonic_process_for_SnowEnd_Dates();

//	Smooth_SnowEnd_Dates_along_aspect();
//	Estimate_SnowEndData_When_It_Is_Much_Smaller_Than_The_Next();  //not need

//	Output_AllSites_SnowEnd_Dates();
// 
//	Output_AllSites_1_year_snow_cover_dates_for_Checking();

	Output_AllSites_SnowDepth_April_Ob_Days();
//	Output_AllSites_SnowDepth_April_5_17_avg_from_2010_2025();

  return 0;
}


void Output_AllSites_1_year_snow_cover_dates_for_Checking()
{
	char CoverType_Name[3][16] = { "NotUsed", "Sedge", "Shrub" };
	char Path[] = "E:\\Snow_Alaska\\Modelling_2026\\Model_Input_Output\\";
	int Number_In_Files[3] = { 0, 7, 14 };
	int Site_Number[3] = { 0, 37632, 75264 };
	int Model_Output_Years = 16;

	char Path_In[200], Path_Out[200];
	char String[300], Note[300];
	char Climate_Site_Name[] = "CLPX_Ob_Precip";
	int Output_Year = 2024;

	int Row, Col, N_In_File, N_Site, N_Climate_Site;
	int N, N1, N2;
	int YY, YY1, i;
	int Type, SiteNo;
	double LAI, Slope, Aspect, Fsnow;
	double SnowCover_Dates[4];  
	double F;

	FILE* fpt_Input, * fpt_Output;

	for (Type = 2; Type <= 2; Type++) {
		printf("Output: %s %d snowcover start and end dates\n", CoverType_Name[Type], Output_Year);

		sprintf(Path_In, "%s%s\\Output\\", Path, CoverType_Name[Type]);
		sprintf(Path_Out, "%s%s\\From_Output_for_Mapping\\SnowDate\\", Path, CoverType_Name[Type]);

		N_Climate_Site = 0;
		for (Row = 0; Row < Total_Rows; Row++) {
			for (Col = 0; Col < Total_Columns; Col++) {
				N_Climate_Site++;

				//---Output Snow depth on a day to a temporary file

				sprintf(Note, "%s%s_%s_SnowCover_Dates_%d.txt",
					Path_Out, Climate_Site_Name, CoverType_Name[Type], Output_Year);
				if ((fpt_Output = fopen(Note, "w")) == NULL) {
					printf("Cannot open file %s\n", Note);
					exit(0);
				}
				fprintf(fpt_Output, "Site LAI Slope Aspect Fsnow ");
				fprintf(fpt_Output, "SnowCoverstart1 End1 Start2 End2\n");

				N1 = 1;
				for (N_In_File = 0; N_In_File < Number_In_Files[Type]; N_In_File++) {
					N2 = N1 + 4999;
					sprintf(Note, "%s%s_%s_Sites_%d_%d.txt", Path_In, Climate_Site_Name, CoverType_Name[Type], N1, N2);
					printf("%s\n", Note);

					if ((fpt_Input = fopen(Note, "r")) == NULL) {
						printf("Cannot open file %s\n", Note);
						exit(0);
					}
					fgets(String, 299, fpt_Input);			//Note line

					for (N = N1; N <= N2; N++) {
						fscanf(fpt_Input, "%d%lf%lf%lf%lf", &SiteNo, &LAI, &Slope, &Aspect, &Fsnow);
						if (SiteNo != N) {
							printf("Site mis-match (Sequence %s, Site %d\n", N, SiteNo);
							fclose(fpt_Input);
							fclose(fpt_Output);
							return;
						}
						for (YY = 0; YY < Model_Output_Years; YY++) {
							fscanf(fpt_Input, "%d%lf%lf%lf", &YY1, &F, &F, &F);
							for (i = 0; i < 4; i++) fscanf(fpt_Input, "%lf", &SnowCover_Dates[i]);
							for (i = 0; i < 37; i++) fscanf(fpt_Input, "%lf", &F); //skip 12*2+13 numbers

							if (YY1 == Output_Year) {
								fprintf(fpt_Output, "%d %.2lf %.0lf %.0lf %.2lf ", SiteNo, LAI, Slope, Aspect, Fsnow);
								for (i = 0; i < 4; i++) fprintf(fpt_Output, "%.0lf ", SnowCover_Dates[i]);
								fprintf(fpt_Output, "\n");
							}
						}  //YY

					}   //N
					fclose(fpt_Input);

					N1 = N2 + 1;
				}  //N_In_File

				N2 = Site_Number[Type];
				sprintf(Note, "%s%s_%s_Sites_%d_%d.txt", Path_In, Climate_Site_Name, CoverType_Name[Type], N1, N2);
				printf("%s\n", Note);

				if ((fpt_Input = fopen(Note, "r")) == NULL) {
					printf("Cannot open file %s\n", Note);
					exit(0);
				}
				fgets(String, 299, fpt_Input);			//Note line

				for (N = N1; N <= N2; N++) {
					fscanf(fpt_Input, "%d%lf%lf%lf%lf", &SiteNo, &LAI, &Slope, &Aspect, &Fsnow);
					if (SiteNo != N) {
						printf("Site mis-match (Sequence %s, Site %d\n", N, SiteNo);
						fclose(fpt_Input);
						fclose(fpt_Output);
						return;
					}
					for (YY = 0; YY < Model_Output_Years; YY++) {
						fscanf(fpt_Input, "%d%lf%lf%lf", &YY1, &F, &F, &F);
						for (i = 0; i < 4; i++) fscanf(fpt_Input, "%lf", &SnowCover_Dates[i]);
						for (i = 0; i < 37; i++) fscanf(fpt_Input, "%lf", &F); //skip 12*2+13 numbers

						if (YY1 == Output_Year) {
							fprintf(fpt_Output, "%d %.2lf %.0lf %.0lf %.2lf ", SiteNo, LAI, Slope, Aspect, Fsnow);
							for (i = 0; i < 4; i++) fprintf(fpt_Output, "%.0lf ", SnowCover_Dates[i]);
							fprintf(fpt_Output, "\n");
						}
					}
				}  //N
				fclose(fpt_Input);

				fclose(fpt_Output);
			}  //Col
		}  //Row
	}  //Type
}

void Estimate_SnowEndData_When_It_Is_Much_Smaller_Than_The_Next() {

	char CoverType_Name[3][16] = { "NotUsed", "Sedge", "Shrub" };
	char Path[] = "E:\\Snow_Alaska\\Modelling_2026\\Model_Input_Output\\";
	//	int Site_Number[3] = { 0, 37632, 75264 };

	char Climate_Name[2][20] = { "CLPX_Ob_Precip", "CLPX_ERA5_Precip" };
//	char Climate_Name[2][20] = { "HV_Top", "HV_Bot" };

	char Path_In[200];
	char String[300], Note[300];
	int  Slope_N, Years = 16, Type, Climate_Site;

	int SiteNo[Fsnow_NN];
	double LAI[Fsnow_NN], Slope[Fsnow_NN], Aspect[Fsnow_NN], Fsnow[Fsnow_NN];
	double Snow_End[16][Fsnow_NN], Snow_End_x[16][Fsnow_NN];

	int i, i_LAI, i_Slope, i_Aspect, i_Fsnow, YY;

	FILE* fpt_Input, * fpt_Output;

	for (Type = 1; Type <= 2; Type++) {  //1, 2
		for (Climate_Site = 0; Climate_Site <= 1; Climate_Site++) { //0, 1
			//-------------------------------------
			if (Type == 1)  Slope_N = Sedge_Slope_NN;
			else            Slope_N = Shrub_Slope_NN;

			printf("Modify EndDate when it is much less than that of the next Fsnow: %s_%s\n", Climate_Name[Climate_Site], CoverType_Name[Type]);

			sprintf(Path_In, "%s%s\\From_Output_for_Mapping\\SnowDate\\", Path, CoverType_Name[Type]);

			sprintf(String, "%s%s_%s_EndDate.txt", Path_In, Climate_Name[Climate_Site], CoverType_Name[Type]);
			if ((fpt_Input = fopen(String, "r")) == NULL) {
				printf("Cannot open file %s\n", String);
				exit(0);
			}
			fgets(Note, 299, fpt_Input);			//Note line

			sprintf(String, "%s%s_%s_EndDate_1.txt", Path_In, Climate_Name[Climate_Site], CoverType_Name[Type]);
			if ((fpt_Output = fopen(String, "w")) == NULL) {
				printf("Cannot open file %s\n", String);
				exit(0);
			}
			fprintf(fpt_Output, "%s", Note);

			for (i_LAI = 0; i_LAI < LAI_NN; i_LAI++) {
				printf("%d ", i_LAI);

				for (i_Slope = 0; i_Slope < Slope_N; i_Slope++) {   //Not Slope_NN
					for (i_Aspect = 0; i_Aspect < Aspect_NN; i_Aspect++) {
						for (i = 0; i < Fsnow_NN; i++) {
							fscanf(fpt_Input, "%d%lf%lf%lf%lf", &SiteNo[i], &LAI[i], &Slope[i], &Aspect[i], &Fsnow[i]);
							for (YY = 0; YY < Years; YY++) fscanf(fpt_Input, "%lf", &Snow_End[YY][i]);
						}   //i_Fsnow

		//make the enddata is no more 30 days less than that of the next Fsnow
						for (YY = 0; YY < Years; YY++) {

							for (i = Fsnow_NN - 2; i >= 0; i--) {
								if (Snow_End[YY][i] < (Snow_End[YY][i + 1] - 20))  //HV:30
									Snow_End[YY][i] = Snow_End[YY][i + 1] - 20;
							}  //for
						}  //YY

						for (i = 0; i < Fsnow_NN; i++) {
							fprintf(fpt_Output, "%d %.1lf %.0lf %.0lf %.2lf ", SiteNo[i], LAI[i], Slope[i], Aspect[i], Fsnow[i]);
							for (YY = 0; YY < Years; YY++) fprintf(fpt_Output, "%.0lf ", Snow_End[YY][i]); //from 2010
							fprintf(fpt_Output, "\n");
						}  //i_Fsnow
					}  //i_Aspect
				}
			}
			fclose(fpt_Input);
			fclose(fpt_Output);
		} //Climate_Site
	}  //Type
}

void Smooth_SnowEnd_Dates_along_aspect() {
	char CoverType_Name[3][16] = { "NotUsed", "Sedge", "Shrub" };
	char Path[] = "E:\\Snow_Alaska\\Modelling_2026\\Model_Input_Output\\";
	//	int Site_Number[3] = { 0, 37632, 75264 };

	char Climate_Name[2][20] = { "CLPX_Ob_Precip", "CLPX_ERA5_Precip" };
//	char Climate_Name[2][20] = {"HV_Top", "HV_Bot"};

	char Path_In[200];
	char String[300], Note[300];
	int  Slope_N, Type, Climate_Site;

	int SiteNo[Fsnow_NN], Years = 7;  //2019-2025
	double LAI[Aspect_NN], Slope[Aspect_NN], Aspect[Aspect_NN], Fsnow[Aspect_NN];
	double Snow_End[7][Aspect_NN];
	double d = 2.01;  //correct only when differ > d
	double F;

	int i, i_LAI, i_Slope, i_Aspect, i_Fsnow, YY;

	FILE* fpt_Input, * fpt_Output;

	for (Type = 1; Type <= 2; Type++) {  //1 and 2
		for (Climate_Site = 0; Climate_Site <= 1; Climate_Site++) {  //0, 1
			if (Type == 1)  Slope_N = Sedge_Slope_NN;
			else            Slope_N = Shrub_Slope_NN;

			printf("Smooth SnowEnd Dates along aspect: %s\n", CoverType_Name[Type]);

			sprintf(Path_In, "%s%s\\From_Output_for_Mapping\\SnowDate\\", Path, CoverType_Name[Type]);

			sprintf(String, "%s%s_%s_EndDate_1.txt", Path_In, Climate_Name[Climate_Site], CoverType_Name[Type]);
			if ((fpt_Input = fopen(String, "r")) == NULL) {
				printf("Cannot open file %s\n", String);
				exit(0);
			}
			fgets(Note, 299, fpt_Input);			//Note line

			sprintf(String, "%s%s_%s_EndDate_2.txt", Path_In, Climate_Name[Climate_Site], CoverType_Name[Type]);
			if ((fpt_Output = fopen(String, "w")) == NULL) {
				printf("Cannot open file %s\n", String);
				exit(0);
			}
			fprintf(fpt_Output, "Site LAI Slope Aspect Fsnow SnowEnd2019 2020 2021 2022 2023 2024 2025\n");

			for (i_LAI = 0; i_LAI < LAI_NN; i_LAI++) {
				printf("%d ", i_LAI);
				for (i_Fsnow = 0; i_Fsnow < Fsnow_NN; i_Fsnow++) {
					for (i_Slope = 0; i_Slope < Slope_N; i_Slope++) {   //Not Slope_NN
						for (i = 0; i < Aspect_NN; i++) {
							fscanf(fpt_Input, "%d%lf%lf%lf%lf", &SiteNo[i], &LAI[i], &Slope[i], &Aspect[i], &Fsnow[i]);

							for (YY = 0; YY < 9; YY++) fscanf(fpt_Input, "%lf", &F);  //skip 2010-2018

							for (YY = 0; YY < Years; YY++) fscanf(fpt_Input, "%lf", &Snow_End[YY][i]);  //2019-2025
						}   //i_Fsnow

						//making the snow end monotonic
						for (YY = 0; YY < Years; YY++) {
							i = 12;  // int(Aspect[i]) == 180
							if (Snow_End[YY][i] > Snow_End[YY][i - 1] && Snow_End[YY][i] > Snow_End[YY][i + 1]) {
								if (Snow_End[YY][i - 1] > Snow_End[YY][i + 1]) Snow_End[YY][i] = Snow_End[YY][i + 1];  //using the smaller
								else  Snow_End[YY][i] = Snow_End[YY][i - 1];
							}

							for (i = 1; i < Aspect_NN - 1; i++) {
								if (int(Aspect[i]) != 180) {
									if ((Snow_End[YY][i] > Snow_End[YY][i - 1] - d && Snow_End[YY][i] > Snow_End[YY][i + 1] + d) ||
										(Snow_End[YY][i] < Snow_End[YY][i - 1] - d && Snow_End[YY][i] < Snow_End[YY][i + 1] + d))
										Snow_End[YY][i] = (Snow_End[YY][i - 1] + Snow_End[YY][i + 1]) / 2.0;
								}
							}  //i_Fsnow
						}  //YY

						for (i = 0; i < Aspect_NN; i++) {
							fprintf(fpt_Output, "%d %.1lf %.0lf %.0lf %.2lf ", SiteNo[i], LAI[i], Slope[i], Aspect[i], Fsnow[i]);
							for (YY = 0; YY < Years; YY++) fprintf(fpt_Output, "%.1lf ", Snow_End[YY][i]); //from 2019
							fprintf(fpt_Output, "\n");
						}  //i
					}  //i_Slope
				}  //i_Fsnow
			}  //i_LAI

			fclose(fpt_Input);
			fclose(fpt_Output);
		}  //Climate
	}  //Type
}


void Monotonic_process_for_SnowEnd_Dates()  {

	char CoverType_Name[3][16] = { "NotUsed", "Sedge", "Shrub" };
	char Path[] = "E:\\Snow_Alaska\\Modelling_2026\\Model_Input_Output\\";
//	int Site_Number[3] = { 0, 37632, 75264 };

//	char Climate_Name[2][20] = { "CLPX_Ob_Precip", "CLPX_ERA5_Precip" };
	char Climate_Name[2][20] = { "HV_Top", "HV_Bot" };

	char Path_In[200];
	char String[300], Note[300];
	int  Slope_N, Years = 7, Type, Climate_Site;

	int SiteNo[Fsnow_NN];
	double LAI[Fsnow_NN], Slope[Fsnow_NN], Aspect[Fsnow_NN], Fsnow[Fsnow_NN];
	double Snow_End[7][Fsnow_NN], Snow_End_x[7][Fsnow_NN];
	double F;
	int i, i_LAI, i_Slope, i_Aspect, i_Fsnow, YY;

	FILE* fpt_Input, * fpt_Output;

	for (Type = 1; Type <= 2; Type++) { //1, 2
		for (Climate_Site = 0; Climate_Site <= 1; Climate_Site++) { //0, 1
			//-------------------------------------
			if (Type == 1)  Slope_N = Sedge_Slope_NN;
			else            Slope_N = Shrub_Slope_NN;

			printf("Make sure snow end is monotonic with Fsnow: %s_%s\n", Climate_Name[Climate_Site], CoverType_Name[Type]);

			sprintf(Path_In, "%s%s\\From_Output_for_Mapping\\SnowDate\\", Path, CoverType_Name[Type]);

			sprintf(String, "%s%s_%s_EndDate.txt", Path_In, Climate_Name[Climate_Site], CoverType_Name[Type]);
			if ((fpt_Input = fopen(String, "r")) == NULL) {
				printf("Cannot open file %s\n", String);
				exit(0);
			}
			fgets(Note, 299, fpt_Input);			//Note line

			sprintf(String, "%s%s_%s_EndDate_Mono_July14.txt", Path_In, Climate_Name[Climate_Site], CoverType_Name[Type]);
			if ((fpt_Output = fopen(String, "w")) == NULL) {
				printf("Cannot open file %s\n", String);
				exit(0);
			}
			fprintf(fpt_Output, "Site LAI Slope Aspect Fsnow SnowEnd2019 2020 2021 2022 2023 2024 2025\n");

			for (i_LAI = 0; i_LAI < LAI_NN; i_LAI++) {
				printf("%d ", i_LAI);

				for (i_Slope = 0; i_Slope < Slope_N; i_Slope++) {   //Not Slope_NN
					for (i_Aspect = 0; i_Aspect < Aspect_NN; i_Aspect++) {
						for (i = 0; i < Fsnow_NN; i++) {
							fscanf(fpt_Input, "%d%lf%lf%lf%lf", &SiteNo[i], &LAI[i], &Slope[i], &Aspect[i], &Fsnow[i]);

							for (YY = 0; YY < 9; YY++) fscanf(fpt_Input, "%lf", &F); //skip 2010-2018
							for (YY = 0; YY < Years; YY++) fscanf(fpt_Input, "%lf", &Snow_End[YY][i]);
						}   //i_Fsnow

						//making the snow end monotonic
						for (YY = 0; YY < Years; YY++) {
							//correct isolated case over/below its neigbours
							for (i = 1; i < Fsnow_NN - 1; i++) {
								if ((Snow_End[YY][i] > Snow_End[YY][i - 1] && Snow_End[YY][i] > Snow_End[YY][i + 1]) ||
									(Snow_End[YY][i] < Snow_End[YY][i - 1] && Snow_End[YY][i] < Snow_End[YY][i + 1]))
									Snow_End[YY][i] = (Snow_End[YY][i - 1] + Snow_End[YY][i + 1]) / 2.0;
								//						else Snow_End_x[YY][i] = Snow_End[YY][i];
							}

							//					for (i = 1; i < Fsnow_NN - 1; i++) Snow_End[YY][i] = Snow_End_x[YY][i];
							//make sure monotonic
							for (i = 1; i < Fsnow_NN; i++) {
								if (Snow_End[YY][i] < Snow_End[YY][i - 1]) Snow_End[YY][i] = Snow_End[YY][i - 1] + 0.1;
								else if (Snow_End[YY][i] == Snow_End[YY][i - 1]) Snow_End[YY][i] = Snow_End[YY][i - 1] + 0.2;
								//else Snow_End[YY][i_Fsnow] = Snow_End[YY][i_Fsnow];
							}  //i_Fsnow
						}  //YY

						for (i = 0; i < Fsnow_NN; i++) {
							fprintf(fpt_Output, "%d %.1lf %.0lf %.0lf %.2lf ", SiteNo[i], LAI[i], Slope[i], Aspect[i], Fsnow[i]);
							for (YY = 0; YY < Years; YY++) fprintf(fpt_Output, "%.1lf ", Snow_End[YY][i]); //from 2019
							fprintf(fpt_Output, "\n");
						}  //i_Fsnow
					}  //i_Aspect
				}
			}
			fclose(fpt_Input);
			fclose(fpt_Output);
		}  //Climate_Site
	}  //TYpe
}


void Output_AllSites_SnowEnd_Dates()
{
	char CoverType_Name[3][16] = { "NotUsed", "Sedge", "Shrub" };
	char Path[] = "E:\\Snow_Alaska\\Modelling_2026\\Model_Input_Output\\";
	int Number_In_Files[3] = { 0, 7, 14 };
	int Site_Number[3] = { 0, 37632, 75264 };

	char Path_In[200], Path_Out[200];
	char String[300], Note[300];

	char Climate_Site_Name[] = "CLPX_Ob_Precip";  //CLPX: Ob, ERA5
//	char Climate_Site_Name[] = "HV_Bot";  //HV_Top, HV_Bot

	int Row, Col, N_In_File, N_Site, N_Climate_Site;
	int N, N1, N2;
	int YY, YY1, i;
	int Type, SiteNo;
	double LAI, Slope, Aspect, Fsnow;
	double SnowCover_Dates[4], Snow_End[16];  //16 years
	double F;

	FILE* fpt_Input, * fpt_Output;

	for (Type = 2; Type <= 2; Type++) {
		printf("Output: %s Snow end dates\n", CoverType_Name[Type]);

		sprintf(Path_In, "%s%s\\Output\\", Path, CoverType_Name[Type]);
		sprintf(Path_Out, "%s%s\\From_Output_for_Mapping\\SnowDate\\", Path, CoverType_Name[Type]);

		N_Climate_Site = 0;
		for (Row = 0; Row < Total_Rows; Row++) {
			for (Col = 0; Col < Total_Columns; Col++) {
				N_Climate_Site++;

				//---Output Snow depth on a day to a temporary file

				sprintf(Note, "%s%s_%s_EndDate.txt",
					Path_Out, Climate_Site_Name, CoverType_Name[Type]);
				if ((fpt_Output = fopen(Note, "w")) == NULL) {
					printf("Cannot open file %s\n", Note);
					exit(0);
				}
				fprintf(fpt_Output, "Site LAI Slope Aspect Fsnow SnowEnd");
				for (YY = 0; YY < YEARS; YY++) fprintf(fpt_Output, "%d ", YY + 2010);
				fprintf(fpt_Output, "\n");

				N1 = 1;
				for (N_In_File = 0; N_In_File < Number_In_Files[Type]; N_In_File++) {
					N2 = N1 + 4999;
					sprintf(Note, "%s%s_%s_Sites_%d_%d.txt", Path_In, Climate_Site_Name, CoverType_Name[Type], N1, N2);
					printf("%s\n", Note);

					if ((fpt_Input = fopen(Note, "r")) == NULL) {
						printf("Cannot open file %s\n", Note);
						exit(0);
					}
					fgets(String, 299, fpt_Input);			//Note line

					for (N = N1; N <= N2; N++) {
						fscanf(fpt_Input, "%d%lf%lf%lf%lf", &SiteNo, &LAI, &Slope, &Aspect, &Fsnow);
						if (SiteNo != N) {
							printf("Site mis-match (Sequence %s, Site %d\n", N, SiteNo);
							fclose(fpt_Input);
							fclose(fpt_Output);
							return;
						}

						for (YY = 0; YY < YEARS; YY++) {
							fscanf(fpt_Input, "%d%lf%lf%lf", &YY1, &F, &F, &F);
							for (i = 0; i < 4; i++) fscanf(fpt_Input, "%lf", &SnowCover_Dates[i]);  
							for (i = 0; i < 37; i++) fscanf(fpt_Input, "%lf", &F); //skip 12*2+13 numbers

							if(SnowCover_Dates[3]>0) Snow_End[YY] = SnowCover_Dates[3];
							else                     Snow_End[YY] = SnowCover_Dates[1];
						}  //YY

						fprintf(fpt_Output, "%d %.2lf %.0lf %.0lf %.2lf ", SiteNo, LAI, Slope, Aspect, Fsnow);
						for (YY = 0; YY < YEARS; YY++) fprintf(fpt_Output, "%.0lf ", Snow_End[YY]);
						fprintf(fpt_Output, "\n");
					}   //N
					fclose(fpt_Input);

					N1 = N2 + 1;
				}  //N_In_File

				N2 = Site_Number[Type];
				sprintf(Note, "%s%s_%s_Sites_%d_%d.txt", Path_In, Climate_Site_Name, CoverType_Name[Type], N1, N2);
				printf("%s\n", Note);

				if ((fpt_Input = fopen(Note, "r")) == NULL) {
					printf("Cannot open file %s\n", Note);
					exit(0);
				}
				fgets(String, 299, fpt_Input);			//Note line

				for (N = N1; N <= N2; N++) {
					fscanf(fpt_Input, "%d%lf%lf%lf%lf", &SiteNo, &LAI, &Slope, &Aspect, &Fsnow);
					if (SiteNo != N) {
						printf("Site mis-match (Sequence %s, Site %d\n", N, SiteNo);
						fclose(fpt_Input);
						fclose(fpt_Output);
						return;
					}
					for (YY = 0; YY < YEARS; YY++) {
						fscanf(fpt_Input, "%d%lf%lf%lf", &YY1, &F, &F, &F);
						for (i = 0; i < 4; i++) fscanf(fpt_Input, "%lf", &SnowCover_Dates[i]);
						for (i = 0; i < 37; i++) fscanf(fpt_Input, "%lf", &F); //skip 12*2+13 numbers

						if (SnowCover_Dates[3] > 0) Snow_End[YY] = SnowCover_Dates[3];
						else                        Snow_End[YY] = SnowCover_Dates[1];
					}
					fprintf(fpt_Output, "%d %.1lf %.0lf %.0lf %.2lf ", SiteNo, LAI, Slope, Aspect, Fsnow);
					for (YY = 0; YY < YEARS; YY++) fprintf(fpt_Output, "%.0lf ", Snow_End[YY]);
					fprintf(fpt_Output, "\n");
				}  //N
				fclose(fpt_Input);

				fclose(fpt_Output);
			}  //Col
		}  //Row
	}  //Type
}

void Output_AllSites_SnowDepth_April_5_17_avg_from_2010_2025()
{
	char CoverType_Name[3][16] = { "NotUsed", "Sedge", "Shrub" };
	char Path[] = "E:\\Snow_Alaska\\Modelling_2026\\Model_Input_Output\\";
	int Number_In_Files[3] = { 0, 7, 14 };
	int Site_Number[3] = { 0, 37632, 75264 };

	char Path_In[200], Path_Out[200];
	char String[300], Note[300];
	//	char Climate_Site_Name[] = "CLPX_ERA5_precip";  //CLPX: Ob, ERA5
	char Climate_Site_Name[] = "HV_Bot";  //HV: Top, Bot

	int Row, Col, N_In_File, N_Site, N_Climate_Site;
	int N, N1, N2;
	int YY, YY1, i, j;
	int Type, SiteNo;

	double LAI, Slope, Aspect, Fsnow;
	double SnowDepth_Apr_Output[YEARS], SnowDepth_Apr_All[YEARS];

	double F, avg;

	FILE* fpt_Input, * fpt_Output;

	//sequence:  0 1 2 3 4  5  6  7  8  9 10 11 12  
	//Output: Apr5 6 7 8 9 10 11 12 13 14 15 16 17
	//Ob dates: 2012-04-16, 2013-04-13, 2015-04-8, 2016-04-5,2017-04-12, 2018-04-13

	for (Type = 1; Type <= 2; Type++) {
		printf("Output: %s Snow depth avg in April 5-17 for years 2019-2025\n", CoverType_Name[Type]);

		sprintf(Path_In, "%s%s\\Output\\", Path, CoverType_Name[Type]);
		sprintf(Path_Out, "%s%s\\From_Output_for_Mapping\\SnowDepthSWE\\", Path, CoverType_Name[Type]);

		N_Climate_Site = 0;
		for (Row = 0; Row < Total_Rows; Row++) {
			for (Col = 0; Col < Total_Columns; Col++) {
				N_Climate_Site++;

				//---Output Snow depth on a day to a temporary file

				sprintf(Note, "%s%s_%s_SnowDepth_avg_Apr_5_17_2019_2025.txt",
					Path_Out, Climate_Site_Name, CoverType_Name[Type]);
				if ((fpt_Output = fopen(Note, "w")) == NULL) {
					printf("Cannot open file %s\n", Note);
					exit(0);
				}
				fprintf(fpt_Output, "Site LAI Slope Aspect Fsnow SnowDepth_Apr5_17_avg");
				for (YY = 9; YY < YEARS; YY++) fprintf(fpt_Output, "%d ", YY + 2010);  //
				fprintf(fpt_Output, "\n");

				N1 = 1;
				for (N_In_File = 0; N_In_File < Number_In_Files[Type]; N_In_File++) {
					N2 = N1 + 4999;
					sprintf(Note, "%s%s_%s_Sites_%d_%d.txt", Path_In, Climate_Site_Name, CoverType_Name[Type], N1, N2);
					printf("%s\n", Note);

					if ((fpt_Input = fopen(Note, "r")) == NULL) {
						printf("Cannot open file %s\n", Note);
						exit(0);
					}
					fgets(String, 299, fpt_Input);			//Note line

					for (N = N1; N <= N2; N++) {
						fscanf(fpt_Input, "%d%lf%lf%lf%lf", &SiteNo, &LAI, &Slope, &Aspect, &Fsnow);
						if (SiteNo != N) {
							printf("Site mis-match (Sequence %s, Site %d\n", N, SiteNo);
							fclose(fpt_Input);
							fclose(fpt_Output);
							return;
						}
						for (YY = 0; YY < YEARS; YY++) {
							fscanf(fpt_Input, "%d%lf%lf%lf", &YY1, &F, &F, &F);
							for (i = 0; i < 28; i++) fscanf(fpt_Input, "%lf", &F); //skip 12*2+4 numbers
							for (i = 0; i < 13; i++) fscanf(fpt_Input, "%lf", &SnowDepth_Apr_All[i]);  //13 days in April from April 5 to 17

							avg = 0;
							for (i = 0; i < 13; i++) avg += SnowDepth_Apr_All[i];  //
							avg /= 13;
							SnowDepth_Apr_Output[YY] = avg;
						}  //YY

						fprintf(fpt_Output, "%d %.2lf %.0lf %.0lf %.2lf ", SiteNo, LAI, Slope, Aspect, Fsnow);
						for (YY = 9; YY <YEARS; YY++) fprintf(fpt_Output, "%.1lf ", SnowDepth_Apr_Output[YY]);  //2019-2025
						fprintf(fpt_Output, "\n");
					}   //N
					fclose(fpt_Input);

					N1 = N2 + 1;
				}  //N_In_File

				N2 = Site_Number[Type];
				sprintf(Note, "%s%s_%s_Sites_%d_%d.txt", Path_In, Climate_Site_Name, CoverType_Name[Type], N1, N2);
				printf("%s\n", Note);

				if ((fpt_Input = fopen(Note, "r")) == NULL) {
					printf("Cannot open file %s\n", Note);
					exit(0);
				}
				fgets(String, 299, fpt_Input);			//Note line

				for (N = N1; N <= N2; N++) {
					fscanf(fpt_Input, "%d%lf%lf%lf%lf", &SiteNo, &LAI, &Slope, &Aspect, &Fsnow);
					if (SiteNo != N) {
						printf("Site mis-match (Sequence %s, Site %d\n", N, SiteNo);
						fclose(fpt_Input);
						fclose(fpt_Output);
						return;
					}
					for (YY = 0; YY < YEARS; YY++) {
						fscanf(fpt_Input, "%d%lf%lf%lf", &YY1, &F, &F, &F);
						for (i = 0; i < 28; i++) fscanf(fpt_Input, "%lf", &F); //skip 12*2+4 numbers
						for (i = 0; i < 13; i++) fscanf(fpt_Input, "%lf", &SnowDepth_Apr_All[i]);  //13 days in April from April 5 to 17

						avg = 0;
						for (i = 0; i < 13; i++) avg += SnowDepth_Apr_All[i];
						avg /= 13;
						SnowDepth_Apr_Output[YY] = avg;
					}
					fprintf(fpt_Output, "%d %.1lf %.0lf %.0lf %.2lf ", SiteNo, LAI, Slope, Aspect, Fsnow);
					for (YY = 9; YY <YEARS; YY++) fprintf(fpt_Output, "%.1lf ", SnowDepth_Apr_Output[YY]); //2019-2025
					fprintf(fpt_Output, "\n");
				}  //N
				fclose(fpt_Input);

				fclose(fpt_Output);
			}  //Col
		}  //Row
	}  //Type
}


void Output_AllSites_SnowDepth_April_Ob_Days()
{
	char CoverType_Name[3][16] = { "NotUsed", "Sedge", "Shrub" };
	char Path[] = "E:\\Snow_Alaska\\Modelling_2026\\Model_Input_Output\\";
	int Number_In_Files[3] = { 0, 7, 14 };
	int Site_Number[3] = { 0, 37632, 75264 };

	char Path_In[200], Path_Out[200];
	char String[300], Note[300];
	char Climate_Site_Name[] = "CLPX_Ob_precip";  //CLPX: Ob, ERA5
//	char Climate_Site_Name[] = "HV_Bot";  //HV: Top, Bot

	int Row, Col, N_In_File, N_Site, N_Climate_Site;
	int N, N1, N2;
	int YY, YY1, i, j;
	int Type, SiteNo;

	double LAI, Slope, Aspect, Fsnow;
	double SnowDepth_Apr_Output[YEARS], SnowDepth_Apr_All[YEARS];

	double F;

	FILE* fpt_Input, * fpt_Output;

	int Ob_Apr_Date[YEARS];
	
	for (YY = 0; YY < YEARS; YY++) Ob_Apr_Date[YY] = 0;  //2010-2025, 0 from April 5
	Ob_Apr_Date[2] = 11; //Ob date: 2012, Apr.16
	Ob_Apr_Date[3] = 8;  //Ob date: 2013, Apr.13
	Ob_Apr_Date[5] = 3;  //Ob date: 2015, Apr.8
	Ob_Apr_Date[6] = 0;  //Ob date: 2016, Apr.5
	Ob_Apr_Date[7] = 7;  //Ob date: 2017, Apr.12
	Ob_Apr_Date[8] = 8;  //Ob date: 2018, Apr.13

	//sequence:  0 1 2 3 4  5  6  7  8  9 10 11 12  
	//Output: Apr5 6 7 8 9 10 11 12 13 14 15 16 17
	//Ob dates: 2012-04-16, 2013-04-13, 2015-04-8, 2016-04-5,2017-04-12, 2018-04-13

	for (Type = 2; Type <= 2; Type++) {
		printf("Output: %s Snow depth in April\n", CoverType_Name[Type]);

		sprintf(Path_In, "%s%s\\Output\\", Path, CoverType_Name[Type]);
		sprintf(Path_Out, "%s%s\\From_Output_for_Mapping\\SnowDepthSWE\\", Path, CoverType_Name[Type]);

		N_Climate_Site = 0;
		for (Row = 0; Row < Total_Rows; Row++) {
			for (Col = 0; Col < Total_Columns; Col++) {
				N_Climate_Site++;

				//---Output Snow depth on a day to a temporary file

				sprintf(Note, "%s%s_%s_SnowDepth_Apr_ObYears_July14.txt",
					Path_Out, Climate_Site_Name, CoverType_Name[Type]);
				if ((fpt_Output = fopen(Note, "w")) == NULL) {
					printf("Cannot open file %s\n", Note);
					exit(0);
				}
				fprintf(fpt_Output, "Site LAI Slope Aspect Fsnow SnowDepth");
				for (YY = 2; YY <=8; YY++) fprintf(fpt_Output, "%dApr%d ", YY + 2010, Ob_Apr_Date[YY]+5);  //2012-2018
				fprintf(fpt_Output, "\n");

				N1 = 1;
				for (N_In_File = 0; N_In_File < Number_In_Files[Type]; N_In_File++) {
					N2 = N1 + 4999;
					sprintf(Note, "%s%s_%s_Sites_%d_%d.txt", Path_In, Climate_Site_Name, CoverType_Name[Type], N1, N2);
					printf("%s\n", Note);

					if ((fpt_Input = fopen(Note, "r")) == NULL) {
						printf("Cannot open file %s\n", Note);
						exit(0);
					}
					fgets(String, 299, fpt_Input);			//Note line

					for (N = N1; N <= N2; N++) {
						fscanf(fpt_Input, "%d%lf%lf%lf%lf", &SiteNo, &LAI, &Slope, &Aspect, &Fsnow);
						if (SiteNo != N) {
							printf("Site mis-match (Sequence %s, Site %d\n", N, SiteNo);
							fclose(fpt_Input);
							fclose(fpt_Output);
							return;
						}
						for (YY = 0; YY < YEARS; YY++) {
							fscanf(fpt_Input, "%d%lf%lf%lf", &YY1, &F, &F, &F);
							for (i = 0; i < 28; i++) fscanf(fpt_Input, "%lf", &F); //skip 12*2+4 numbers
							for (i = 0; i < 13; i++) fscanf(fpt_Input, "%lf", &SnowDepth_Apr_All[i]);  //13 days in April from April 5 to 17
							j = Ob_Apr_Date[YY];
							SnowDepth_Apr_Output[YY] = SnowDepth_Apr_All[j];
						}  //YY

						fprintf(fpt_Output, "%d %.2lf %.0lf %.0lf %.2lf ", SiteNo, LAI, Slope, Aspect, Fsnow);
						for (YY = 2; YY <= 8; YY++) fprintf(fpt_Output, "%.1lf ", SnowDepth_Apr_Output[YY]);
						fprintf(fpt_Output, "\n");
					}   //N
					fclose(fpt_Input);

					N1 = N2 + 1;
				}  //N_In_File

				N2 = Site_Number[Type];
				sprintf(Note, "%s%s_%s_Sites_%d_%d.txt", Path_In, Climate_Site_Name, CoverType_Name[Type], N1, N2);
				printf("%s\n", Note);

				if ((fpt_Input = fopen(Note, "r")) == NULL) {
					printf("Cannot open file %s\n", Note);
					exit(0);
				}
				fgets(String, 299, fpt_Input);			//Note line

				for (N = N1; N <= N2; N++) {
					fscanf(fpt_Input, "%d%lf%lf%lf%lf", &SiteNo, &LAI, &Slope, &Aspect, &Fsnow);
					if (SiteNo != N) {
						printf("Site mis-match (Sequence %s, Site %d\n", N, SiteNo);
						fclose(fpt_Input);
						fclose(fpt_Output);
						return;
					}
					for (YY = 0; YY < YEARS; YY++) {
						fscanf(fpt_Input, "%d%lf%lf%lf", &YY1, &F, &F, &F);
						for (i = 0; i < 28; i++) fscanf(fpt_Input, "%lf", &F); //skip 12*2+4 numbers
						for (i = 0; i < 13; i++) fscanf(fpt_Input, "%lf", &SnowDepth_Apr_All[i]);  //13 days inApril from April 5 to 17
						j = Ob_Apr_Date[YY];
						SnowDepth_Apr_Output[YY] = SnowDepth_Apr_All[j];
					}
					fprintf(fpt_Output, "%d %.1lf %.0lf %.0lf %.2lf ", SiteNo, LAI, Slope, Aspect, Fsnow);
					for (YY = 2; YY <= 8; YY++) fprintf(fpt_Output, "%.1lf ", SnowDepth_Apr_Output[YY]);
					fprintf(fpt_Output, "\n");
				}  //N
				fclose(fpt_Input);

				fclose(fpt_Output);
			}  //Col
		}  //Row
	}  //Type
}


void Output_AllSites_Results_For_Mapping_For_Fsnow_1yr()
{
	char Path_In_Model_Output[] = "E:\\Snow_Alaska\\Modelling\\CLPX\\Model_Output\\";                 //HV, CLPX
	char Path_In_LAI_Fsnow[]    = "E:\\Snow_Alaska\\Modelling\\CLPX\\Model_Output\\";                 //HV,CLPX   
	char Path_Out[]             = "E:\\Snow_Alaska\\Modelling\\CLPX\\Generated_from_Model_Output\\";  //HV,CLPX

	//0:Shrub, 1 Sedge
	char CoverType_Name[2][16] = {"Shrub", "Sedge"};

	char String[600], Note[200];

	int Row;  //, Col

	int N_Site, Total_Climate_Sites = 2;

	int Type, N, N_LAI, N_Slope, N_Aspect, N_Fsnow, 
		         i_LAI, i_Slope, i_Aspect, i_Fsnow;
	double LAI_Avg[LAI_NN], Slope_Avg[Slope_NN], Aspect_Avg[Aspect_NN], Fsnow_Avg[Fsnow_NN];

	int Y, L, No, SiteNo, Year[YEARS];

//	SiteNo LAI Slope Aspect Fsnow Year SnowD_Apr SnowCover_Days_1 SnowCover_Days_2 D96 D97 D98 D99 D100 D101 D102 D103 D104 D105 D106 D107

	double SnowD_Apr[YEARS], SnowCover_Days_1[YEARS], SnowCover_Days_2[YEARS],
		   SnowDepth[YEARS][12];  	       //0 to 9 for DOY from 96 to 107

	double F;

	FILE* fpt_Input, * fpt_Output, * fpt_In;

	int Output_Year = 2023, Ob_DOY_from_96 = 0;  // values: 2000 to 2023. Using 0 for >= 2019

	//	    Year    DOY_ob  Ob_DOY_from_96
	//		2012	107     11
	//		2013	103     7
	//		2015	98      2  
	//		2016	96      0
	//		2017	102     6
	//		2018	103     7

	printf("Output year %d\n", Output_Year);

	for (Type = 0; Type < 2; Type++) {   //2
		printf("Land cover type: %s\n", CoverType_Name[Type]);

		//read LAI
		sprintf(Note, "%s%s\\%s_LAI_N.txt", Path_In_LAI_Fsnow, CoverType_Name[Type], CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_LAI);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_LAI; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_LAI, &LAI_Avg[N]);
		}
		fclose(fpt_In);

		//read Slope
		sprintf(Note, "%s%s\\%s_Slope_N.txt", Path_In_LAI_Fsnow, CoverType_Name[Type], CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_Slope);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_Slope; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_Slope, &Slope_Avg[N]);
		}
		fclose(fpt_In);

		//read Aspect
		sprintf(Note, "%s%s\\%s_Aspect_N.txt", Path_In_LAI_Fsnow, CoverType_Name[Type], CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_Aspect);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_Aspect; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_Aspect, &Aspect_Avg[N]);
		}
		fclose(fpt_In);

		//Read Fsnow
		sprintf(Note, "%s%s\\%s_Fsnow_N.txt", Path_In_LAI_Fsnow, CoverType_Name[Type], CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_Fsnow);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_Fsnow; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_Fsnow, &Fsnow_Avg[N]);
		}
		fclose(fpt_In);

		N_Site = 0;
		for (Row = 0; Row <2 ; Row++) {  //Total_Rows
//			for (Col = 0; Col < Total_Columns; Col++) 	{
				N_Site += 1;   //file name start from 1
				printf("Output climate location %d\n", N_Site);
				//------------------------------------------------------------------------------

				sprintf(Note, "%s%s\\L%d_%d.txt", Path_Out, CoverType_Name[Type], N_Site, Output_Year);     //L1, L2, L3 .....
				if ((fpt_Output = fopen(Note, "w")) == NULL) {
					printf("Cannot open file %s\n", Note);
					exit(0);
				}
				fprintf(fpt_Output, "%d %d %d %d\n", N_LAI, N_Slope, N_Aspect, N_Fsnow);
				fprintf(fpt_Output, "No LAI Slope Aspect Fsnow Snowcover_day1 SnowCover_day2 SnowDepth\n");

//HV
//				if(Row==0) sprintf(String, "%s%s\\Site_%s_Top_Allsites.txt",    Path_In_Model_Output, CoverType_Name[Type], CoverType_Name[Type]);  
//				else       sprintf(String, "%s%s\\Site_%s_Bottom_Allsites.txt", Path_In_Model_Output, CoverType_Name[Type], CoverType_Name[Type]);
//CLPX
				if (Row == 0) sprintf(String, "%s%s\\Site_%s_Left_Allsites.txt", Path_In_Model_Output, CoverType_Name[Type], CoverType_Name[Type]);
				else          sprintf(String, "%s%s\\Site_%s_Right_Allsites.txt",Path_In_Model_Output, CoverType_Name[Type], CoverType_Name[Type]);

				if ((fpt_Input = fopen(String, "r")) == NULL) {
					printf("Cannot open file %s\n", String);
					exit(0);
				}
				fgets(String, 599, fpt_Input);			//fist line

				No = 0;
				for (i_LAI = 0; i_LAI < N_LAI; i_LAI++) {                       //N_LAI
					for (i_Slope = 0; i_Slope < N_Slope; i_Slope++) {           //N_Slope
						for (i_Aspect = 0; i_Aspect < N_Aspect; i_Aspect++) {   //N_Aspect
							for (i_Fsnow = 0; i_Fsnow < N_Fsnow; i_Fsnow++) {   //N_Fsnow
								No++;

								if((No % 1000)==0) printf("%d ", No);

								for (Y = 0; Y < YEARS; Y++) {
									fscanf(fpt_Input, "%d%lf%lf%lf%lf%d%lf%lf%lf",
										&SiteNo, &F, &F, &F, &F, &Year[Y], &SnowD_Apr[Y],
										&SnowCover_Days_1[Y], &SnowCover_Days_2[Y]);

									for (L = 0; L < 12; L++) fscanf(fpt_Input, "%lf", &SnowDepth[Y][L]);
								}

								for (Y = 1; Y < YEARS; Y++) {
									if (Year[Y] == Output_Year) {
										fprintf(fpt_Output, "%d %.3lf %.1lf %.1lf %.3lf %.0lf %.0lf %.2lf\n",
											SiteNo, LAI_Avg[i_LAI], Slope_Avg[i_Slope], Aspect_Avg[i_Aspect], Fsnow_Avg[i_Fsnow],
											SnowCover_Days_1[Y], SnowCover_Days_2[Y], SnowDepth[Y][Ob_DOY_from_96]);
									}
								}  //Y
							}  //i_Fsnow
						}  //i_Aspect
					} //i_Slope
				}    //i_LAI
				fprintf(fpt_Output, "0 -10 -10\n");  //
				fclose(fpt_Input);
				fclose(fpt_Output);
				printf("\n");
//			}  //Col
		}    //Row
	}  //Type loop
}

