#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void Mapping_Fsnow_HV();
void Mapping_Fsnow_HV_using_SnowDepth();

void Read_Model_Results_for_Two_Sites_Fsnow(char* File_Name, int Num_Input_Values, int Year, int N_LAI, int N_Slope, int N_Aspect, int N_Fsnow, float* Lx_Values);

void Mapping_Snow_Status_HV();

void Output_a_mutiband_image_to_individual_Images();

void Output_pixel_values_of_Fsnow_images_for_calculating_regression();
void Output_pixel_values_of_Fsnow_images_from_SnowDepth_for_calculating_regression();

void Output_pixel_values_of_SnowDepth_images_7_years_for_calculating_regression();

void Output_pixel_values_of_one_image();
void Calculate_mean_and_differences_Fsnow();

void Fsnow_from_SnowDepth_caluclate_avg_or_Median();
void Fsnow_caluclate_mean_relative_absolute_difference_from_avg();

void Output_pixel_values_of_observed_Modelled_SnowDepth_images();
void Calculate_mean_and_differences_SnowDepth();
void Linear_Regression(int N, double* x, double* y, double* Regression_V);

void Output_Values_of_A_Pixel_HV();

void Correct_Fsnow_and_generate_Fsnow_for_each_Year();

void Get_XY_Values_for_The_Two_Locations_Fsnow(double LAI, double Slope, double Aspect, int N_LAI, int N_Slope, int N_Aspect, int N_Fsnow,
	double* LAI_Avg, double* Slope_Avg, double* Aspect_Avg, double* xx_LAI, double* xx_Slope, double* xx_Aspect, float* L_Values,
	double** Loc2_Snow111, double** Loc2_Snow112, double** Loc2_Snow121, double** Loc2_Snow122,
	double** Loc2_Snow211, double** Loc2_Snow212, double** Loc2_Snow221, double** Loc2_Snow222);

void Get_XY_Values_for_The_Two_Locations_Snow_Status(double LAI, double Slope, double Aspect, double Fsnow,
	int N_LAI, int N_Slope, int N_Aspect, int N_Fsnow, double* LAI_Avg, double* Slope_Avg, double* Aspect_Avg, double* Fsnow_Avg,
	double* xx_LAI, double* xx_Slope, double* xx_Aspect, double* xx_Fsnow, float* L_Values,
	double** Loc2_Snow111, double** Loc2_Snow112, double** Loc2_Snow121, double** Loc2_Snow122,
	double** Loc2_Snow211, double** Loc2_Snow212, double** Loc2_Snow221, double** Loc2_Snow222);

double Linear_Interpolation_Fsnow(int Line, double* Loc_Line, double LAI, double Slope, double Aspect, double SnowCondition,
	int N_Fsnow, double* Fsnow_Avg, double* xx_LAI, double* xx_Slope, double* xx_Aspect,
	double** Loc2_Snow111, double** Loc2_Snow112, double** Loc2_Snow121, double** Loc2_Snow122,
	double** Loc2_Snow211, double** Loc2_Snow212, double** Loc2_Snow221, double** Loc2_Snow222);

double Linear_Interpolation_Snow_Status(int Line, double* Loc_Line, double LAI, double Slope, double Aspect, double Fsnow,
	double* xx_LAI, double* xx_Slope, double* xx_Aspect, double* xx_Fsnow,
	double** Loc2_Snow111, double** Loc2_Snow112, double** Loc2_Snow121, double** Loc2_Snow122,
	double** Loc2_Snow211, double** Loc2_Snow212, double** Loc2_Snow221, double** Loc2_Snow222);


#define PIXELS 113   //HV:113   CLPX:458,       
#define LINES  513   //HV:513   CLPX:370,  

#define YEARS_Fsnow 7

#define Total_Columns 1  //model run locations
#define Total_Rows  2    //
#define Snow_LC_NN 2

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


void Processing_Images();
void Fsnow_caluclate_avg_or_Median();
int Assign_Fsnow(int N_Years, float SnowEnd_u, short Fsnow_x, double u_max, double* Fsnow);

int main()
{
//	Mapping_Fsnow_HV();  
//	Mapping_Fsnow_HV_using_SnowDepth();

	Processing_Images();

//	Mapping_Snow_Status_HV();


//	Output_Values_of_A_Pixel_HV();

  return 0;
}


void Processing_Images() {

//	Output_a_mutiband_image_to_individual_Images();

//	Fsnow_caluclate_avg_or_Median();
//	Fsnow_from_SnowDepth_caluclate_avg_or_Median();

	Fsnow_caluclate_mean_relative_absolute_difference_from_avg();

//	Output_pixel_values_of_Fsnow_images_for_calculating_regression();
//	Output_pixel_values_of_Fsnow_images_from_SnowDepth_for_calculating_regression();
 
//	Output_pixel_values_of_one_image();

//Calculate_mean_and_differences_Fsnow();

//	Output_pixel_values_of_observed_Modelled_SnowDepth_images();
//	Calculate_mean_and_differences_SnowDepth();

//	Output_pixel_values_of_SnowDepth_images_7_years_for_calculating_regression();

//	Correct_Fsnow_and_generate_Fsnow_for_each_Year();
}

void Correct_Fsnow_and_generate_Fsnow_for_each_Year() {
//1) calculate relative difference (ER) of each year's Fsnow from the 7-yr average Fsnow (2019-2025)
//2) correct ER based on its linear relationships with Snow depth
//3) generate corrected Fsnow for each year based on the linear relationship

	char Path_In_Mask[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow\\";
	char Path_In_SnowEnd_u[] = "E:\\Snow_Alaska\\Data_From_Mitchell\\2026_YZ\\HV_SnowEndB_Layers\\";
	char Path_In[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow\\";
	char Path_Out[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow\\";

	char String[200];

	int Year;
	int i, N, Line, Pix;

	double Fsnow[7], Fsnow_avg, Fsnow_ER[7], Fsnow_Out[7];
	double X[7], Y[7], Regression_V[3], R, slope, intercept;
	double Modelled_SnowStatus_1[7] = { 139, 146, 154, 150, 150, 152, 167 },  //2019-2025, SDD period]
		   Modelled_SnowStatus_2[7] = { 150, 163, 155, 138, 140, 146, 147 };  //2012-2018 Snowdepth ob period

	unsigned char* aLine_8U = new unsigned char[PIXELS];

	short* aLine_16S_0 = new short[PIXELS]; 	short* aLine_16S_1 = new short[PIXELS];
	short* aLine_16S_2 = new short[PIXELS]; 	short* aLine_16S_3 = new short[PIXELS];
	short* aLine_16S_4 = new short[PIXELS]; 	short* aLine_16S_5 = new short[PIXELS];
	short* aLine_16S_6 = new short[PIXELS];     short* aLine_16S_avg = new short[PIXELS];

	float* aLine_32R_0 = new float[PIXELS]; 	float* aLine_32R_1 = new float[PIXELS];
	float* aLine_32R_2 = new float[PIXELS]; 	float* aLine_32R_3 = new float[PIXELS];
	float* aLine_32R_4 = new float[PIXELS]; 	float* aLine_32R_5 = new float[PIXELS];
	float* aLine_32R_6 = new float[PIXELS];

	short* aLine_Out_0 = new short[PIXELS]; 	short* aLine_Out_1 = new short[PIXELS];
	short* aLine_Out_2 = new short[PIXELS]; 	short* aLine_Out_3 = new short[PIXELS];
	short* aLine_Out_4 = new short[PIXELS]; 	short* aLine_Out_5 = new short[PIXELS];

	FILE* fpt_In_8U, * fpt_In_16S[7], * fpt_In_16S_avg, * fpt_In_32R[7];
	FILE* fpt_Out[6];  //2012-2018

//output file
	for (i = 0; i < 6; i++) {
		if(i<=1) Year = 2012 + i;
		else     Year = 2013 + i;  //skip 2014

		sprintf(String, "%sHV_Fsnow_estimated_from_7yr_avg_Fsnow_%d.dat", Path_Out, Year);
		if ((fpt_Out[i] = fopen(String, "wb")) == NULL) {
			printf("Cannot open file %s\n", String);
			exit(0);
		}
	}

// input and calculate ER
	sprintf(String, "%sHV_study_area_exclude_water.raw", Path_In_Mask);  //
	if ((fpt_In_8U = fopen(String, "rb")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}

	for (i = 0; i < 7; i++) {
		Year = 2019 + i;
		sprintf(String, "%sHV_avg_Fsnow_%d.dat", Path_In, Year);
		if ((fpt_In_16S[i] = fopen(String, "rb")) == NULL) {
			printf("Cannot open file %s\n", String);
			exit(0);
		}
		sprintf(String, "%sHV_SnowEndB_u_%d.raw", Path_In_SnowEnd_u, Year);
		if ((fpt_In_32R[i] = fopen(String, "rb")) == NULL) {
			printf("Cannot open file %s\n", String);
			exit(0);
		}
	}
	sprintf(String, "%sHV_avg_Fsnow_7yr_avg.dat", Path_In);
	if ((fpt_In_16S_avg = fopen(String, "rb")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}

	for (Line = 0; Line < LINES; Line++) {
		printf(".");
		fread(aLine_8U, 1, PIXELS, fpt_In_8U);

		fread(aLine_16S_0, 2, PIXELS, fpt_In_16S[0]);
		fread(aLine_16S_1, 2, PIXELS, fpt_In_16S[1]);
		fread(aLine_16S_2, 2, PIXELS, fpt_In_16S[2]);
		fread(aLine_16S_3, 2, PIXELS, fpt_In_16S[3]);
		fread(aLine_16S_4, 2, PIXELS, fpt_In_16S[4]);
		fread(aLine_16S_5, 2, PIXELS, fpt_In_16S[5]);
		fread(aLine_16S_6, 2, PIXELS, fpt_In_16S[6]);

		fread(aLine_16S_avg, 2, PIXELS, fpt_In_16S_avg);

		fread(aLine_32R_0, 4, PIXELS, fpt_In_32R[0]);  //for u
		fread(aLine_32R_1, 4, PIXELS, fpt_In_32R[1]);  //for u
		fread(aLine_32R_2, 4, PIXELS, fpt_In_32R[2]);  //for u
		fread(aLine_32R_3, 4, PIXELS, fpt_In_32R[3]);  //for u
		fread(aLine_32R_4, 4, PIXELS, fpt_In_32R[4]);  //for u
		fread(aLine_32R_5, 4, PIXELS, fpt_In_32R[5]);  //for u
		fread(aLine_32R_6, 4, PIXELS, fpt_In_32R[6]);  //for u

		for (Pix = 0; Pix < PIXELS; Pix++) {
			Fsnow_avg = aLine_16S_avg[Pix];
			R = 0;
			slope = 0;
			intercept = 0;
			if (aLine_8U[Pix] == 1) {
				for (i = 0; i < 7; i++) Fsnow[i] = -200;

				if (aLine_32R_0[Pix] < 5.6) Fsnow[0] = aLine_16S_0[Pix];
				if (aLine_32R_1[Pix] < 5.6) Fsnow[1] = aLine_16S_1[Pix];
				if (aLine_32R_2[Pix] < 5.6) Fsnow[2] = aLine_16S_2[Pix];
				if (aLine_32R_3[Pix] < 5.6) Fsnow[3] = aLine_16S_3[Pix];
				if (aLine_32R_4[Pix] < 5.6) Fsnow[4] = aLine_16S_4[Pix];
				if (aLine_32R_5[Pix] < 5.6) Fsnow[5] = aLine_16S_5[Pix];
				if (aLine_32R_6[Pix] < 5.6) Fsnow[6] = aLine_16S_6[Pix];

				for (i = 0; i < 7; i++) Fsnow_ER[i] = (Fsnow[i] - Fsnow_avg) / Fsnow_avg;

				N = 0;
				for (i = 0; i < 7; i++) {
					if (Fsnow[i] > -100) {
						X[N] = Modelled_SnowStatus_1[i];
						Y[N] = Fsnow_ER[i];
						N++;
					}
				}
				if (N == 7) {
					Linear_Regression(N, X, Y, Regression_V); //0-2: R, Slope, Intercept
					R = Regression_V[0];
					slope = Regression_V[1];
					intercept = Regression_V[2];
				}
				else R = 0;
			}  //if

//correct Fsnow
			for (i = 0; i < 7; i++) {   //2012-2018
				if (fabs(R) < 0.5) Fsnow_Out[i] = Fsnow_avg;  //no correction, using avg Fsnow
				else {
					Fsnow_Out[i] = Fsnow_avg * (1.0 + intercept + slope * Modelled_SnowStatus_2[i]);
					if (Fsnow_Out[i] < 150.) Fsnow_Out[i] = 150.0;
				}
			}

			aLine_Out_0[Pix] = short(Fsnow_Out[0] + 0.5);  //2012
			aLine_Out_1[Pix] = short(Fsnow_Out[1] + 0.5);  //2013
			aLine_Out_2[Pix] = short(Fsnow_Out[3] + 0.5);  //2015, skip 2014
			aLine_Out_3[Pix] = short(Fsnow_Out[4] + 0.5);  //2016
			aLine_Out_4[Pix] = short(Fsnow_Out[5] + 0.5);  //2017
			aLine_Out_5[Pix] = short(Fsnow_Out[6] + 0.5);  //2018
		}  //pix

		fwrite(aLine_Out_0, 2, PIXELS, fpt_Out[0]);
		fwrite(aLine_Out_1, 2, PIXELS, fpt_Out[1]);
		fwrite(aLine_Out_2, 2, PIXELS, fpt_Out[2]);
		fwrite(aLine_Out_3, 2, PIXELS, fpt_Out[3]);
		fwrite(aLine_Out_4, 2, PIXELS, fpt_Out[4]);
		fwrite(aLine_Out_5, 2, PIXELS, fpt_Out[5]);
	}  //line

	fclose(fpt_In_8U);
	for (i = 0; i < 7; i++) fclose(fpt_In_16S[i]);
	for (i = 0; i < 7; i++) fclose(fpt_In_32R[i]);
	for (i = 0; i < 6; i++) fclose(fpt_Out[i]);

	delete[]aLine_8U;
	delete[]aLine_16S_0; delete[]aLine_16S_1; delete[]aLine_16S_2; delete[]aLine_16S_3;
	delete[]aLine_16S_4; delete[]aLine_16S_5; delete[]aLine_16S_6;  delete[]aLine_16S_avg;

	delete[]aLine_32R_0; delete[]aLine_32R_1; delete[]aLine_32R_2; delete[]aLine_32R_3;
	delete[]aLine_32R_4; delete[]aLine_32R_5; delete[]aLine_32R_6;

	delete[]aLine_Out_0; delete[]aLine_Out_1; delete[]aLine_Out_2;
	delete[]aLine_Out_3; delete[]aLine_Out_4; delete[]aLine_Out_5;
}


void Output_a_mutiband_image_to_individual_Images() {

//	char Path[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow_2026July\\";
	char Path[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_SnowDepth_2026July\\";
	char Note[300];

	int NN = 6;  //Fsnow:7, Ob_snowDepth:6
//	int Year[7] = { 2019, 2020, 2021, 2022, 2023, 2024, 2025 };
	int Year[6] = { 2012, 2013, 2015, 2016, 2017, 2018};
	int N;

	int Line, Pix;

	short* Line_x = new short[PIXELS];

	FILE* fpt_Input,  * fpt_Output;

//	sprintf(Note, "%sHV_avg_Fsnow_2019_2025.dat", Path);  //
//	sprintf(Note, "%sHV_Fsnow_from_SnowDepth_2012_2018.dat", Path);  //
	sprintf(Note, "%sHV_SnowDepth_using_7yr_avg_Fsnow_2012_2018.dat", Path);  

	if ((fpt_Input = fopen(Note, "rb")) == NULL) {
		printf("Cannot open input file %s\n", Note);
		exit(0);
	}

	for (N = 0; N < NN; N++) {
		printf("%d\n", Year[N]);

		sprintf(Note, "%sHV_SnowDepth_avg_Fsnow_Apr_%d.dat", Path, Year[N]);  //

//		sprintf(Note, "%sHV_avg_Fsnow_%d.dat", Path, Year[N]);  //
//		sprintf(Note, "%sHV_SnowDepth_avg_Apr_5_17_using_7yr_avg_Fsnow_%d.dat", Path, Year[N]); 

		if ((fpt_Output = fopen(Note, "wb")) == NULL) {
			printf("Cannot open output file %s\n", Note);
			exit(0);
		}

		for (Line = 0; Line < LINES; Line++) {
			fread( Line_x, 2, PIXELS, fpt_Input);
			fwrite(Line_x, 2, PIXELS, fpt_Output);
		}  //Line
		fclose(fpt_Output);
	}  //N

	fclose(fpt_Input);
	delete[]Line_x;
	}


void Calculate_mean_and_differences_SnowDepth() {
	char Path[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_SnowDepth_2026July\\";
	char String[300];
	int N_cols, N_rows, N_cols_half;

	int Line, Pix, V[12], N[12];

	double avg_ob[12], avg_mod[12], diff[12], abs_diff[12];
	int i, j;

	FILE* fpt_Input, * fpt_Output;

	sprintf(String, "%sa.txt", Path);  //
	if ((fpt_Output = fopen(String, "w")) == NULL) {
		printf("Cannot open output file %s\n", String);
		exit(0);
	}

	sprintf(String, "%sSnowDepth.txt", Path);  //
	if ((fpt_Input = fopen(String, "r")) == NULL) {
		printf("Cannot open output file %s\n", String);
		exit(0);
	}
	fgets(String, 299, fpt_Input);
	N_cols = 12;
	N_cols_half = N_cols / 2;
	N_rows = 31170;

	for (i = 0; i < N_cols; i++) {
		avg_ob[i] = 0;
		avg_mod[i] = 0;
		diff[i] = 0;
		abs_diff[i] = 0;
		N[i] = 0;
	}

	for (j = 0; j < N_rows; j++) {
		fscanf(fpt_Input, "%d%d", &Line, &Pix);
		for (i = 0; i < N_cols; i++) fscanf(fpt_Input, "%d", &V[i]);

		for (i = 0; i < N_cols_half; i++) {
			if (V[i] > -100 && V[N_cols_half + i] > -100) {
				N[i]++;
				avg_ob[i] += V[i];   
				avg_mod[i] += V[N_cols_half + i];

				diff[i] += V[N_cols_half + i] - V[i];
				abs_diff[i] += fabs(V[N_cols_half + i] - V[i]);
			}
		}  //i
	}  //j

	for (i = 0; i < N_cols_half; i++) {
		avg_ob[i] /= N[i];
		avg_mod[i] /= N[i];
		diff[i] /= N[i];
		abs_diff[i] /= N[i];
	}

	for (i = 0; i < N_cols_half; i++) fprintf(fpt_Output, "%d ", N[i]);
	fprintf(fpt_Output, "\n");
	for (i = 0; i < N_cols_half; i++) fprintf(fpt_Output, "%.1lf ", avg_ob[i] * 0.1);
	fprintf(fpt_Output, "\n");

	for (i = 0; i < N_cols_half; i++) fprintf(fpt_Output, "%.1lf ", avg_mod[i] * 0.1);
	fprintf(fpt_Output, "\n");

	for (i = 0; i < N_cols_half; i++) fprintf(fpt_Output, "%.3lf ", avg_mod[i]/avg_ob[i]);
	fprintf(fpt_Output, "\n");

	for (i = 0; i < N_cols_half; i++) fprintf(fpt_Output, "%.1lf ", diff[i] * 0.1);
	fprintf(fpt_Output, "\n");

	for (i = 0; i < N_cols_half; i++) fprintf(fpt_Output, "%.1lf ", abs_diff[i] * 0.1);
	fprintf(fpt_Output, "\n");

	for (i = 0; i < N_cols_half; i++) fprintf(fpt_Output, "%.1lf ", abs_diff[i] / avg_ob[i] * 100);
	fprintf(fpt_Output, "\n");

	fclose(fpt_Input);
	fclose(fpt_Output);
}



void Calculate_mean_and_differences_Fsnow() { 
	char Path[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow_2026July\\";
	char String[200];
	int N_cols, N_rows, N_cols_1;

	int Line, Pix, V[8], N[8];

	double avg[8], avg_last_col[8], diff[8], abs_diff[8];
	int i, j;

	FILE* fpt_Input, *fpt_Output;

	sprintf(String, "%sa.txt", Path);  //
	if ((fpt_Output = fopen(String, "w")) == NULL) {
		printf("Cannot open output file %s\n", String);
		exit(0);
	}

	sprintf(String, "%sHV_Fsnow_with_7yr_avg.txt", Path);  //
	if ((fpt_Input = fopen(String, "r")) == NULL) {
		printf("Cannot open output file %s\n", String);
		exit(0);
	}
	fgets(String, 199, fpt_Input);
	fscanf(fpt_Input, "%d%d", &N_cols, &N_rows);

	N_cols_1 = N_cols - 1;

	for (i = 0; i < N_cols; i++) {
		avg[i] = 0;
		avg_last_col[i] = 0;
		diff[i] = 0;
		abs_diff[i] = 0;
		N[i] = 0;
	}

	for (j = 0; j < N_rows; j++) {
		fscanf(fpt_Input, "%d%d", &Line, &Pix);
		for (i = 0; i < N_cols; i++) fscanf(fpt_Input, "%d", &V[i]);

		for (i = 0; i < N_cols_1; i++) {
			if (V[i] > -100) {
				avg[i]          += V[i];
				avg_last_col[i] += V[N_cols_1];
				diff[i]         += V[i] - V[N_cols_1];
				abs_diff[i]     += fabs(V[i] - V[N_cols_1]);
				N[i]++;
			}
		}
	}

	for (i = 0; i < N_cols_1; i++) {
		avg[i] /= N[i];
		avg_last_col[i] /= N[i];
		diff[i] /= N[i];
		abs_diff[i] /= N[i];
	}

	for (i = 0; i < N_cols_1; i++) fprintf(fpt_Output, "%d ", N[i]);
	fprintf(fpt_Output, "\n");
	for (i = 0; i < N_cols_1; i++) fprintf(fpt_Output, "%.3lf ", avg[i]*0.001);
	fprintf(fpt_Output, "\n");

	for (i = 0; i < N_cols_1; i++) fprintf(fpt_Output, "%lf ", diff[i]*0.001);
	fprintf(fpt_Output, "\n");

	for (i = 0; i < N_cols_1; i++) fprintf(fpt_Output, "%lf ", diff[i]/ avg_last_col[i] * 100);
	fprintf(fpt_Output, "\n");

	for (i = 0; i < N_cols_1; i++) fprintf(fpt_Output, "%lf ", abs_diff[i] * 0.001);
	fprintf(fpt_Output, "\n");

	for (i = 0; i < N_cols_1; i++) fprintf(fpt_Output, "%lf ", abs_diff[i] / avg_last_col[i] * 100);
	fprintf(fpt_Output, "\n");

	fclose(fpt_Input);
	fclose(fpt_Output);

}

void Fsnow_from_SnowDepth_caluclate_avg_or_Median() {

	char Path_Fsnow[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow_2026\\";
	char Clime_Site_Name[] = "HV";
	char Note[300];

	int Avg_or_Median = 1;  //1:avg, 2:median
	int i, j;

	int YEARS = 6, Year[6] = { 2012, 2013, 2015, 2016, 2017, 2018 };
	int Line, Pix;

	int N_Years;
	double Fsnow[6], avg;
	double F, x[6];

	short* Line_Fsnow_0 = new short[PIXELS], * Line_Fsnow_1 = new short[PIXELS],
		* Line_Fsnow_2 = new short[PIXELS], * Line_Fsnow_3 = new short[PIXELS],
		* Line_Fsnow_4 = new short[PIXELS], * Line_Fsnow_5 = new short[PIXELS];

	short* Line_Fsnow_Output = new short[PIXELS];
	unsigned char* Line_Data_Years = new unsigned char[PIXELS];

	FILE* fpt_Input_Fsnow[6], * fpt_Output_Fsnow, * fpt_Output_Years;

	if (Avg_or_Median == 1) sprintf(Note, "%s%s_Fsnow_from_SnowDepth_6yr_avg.dat", Path_Fsnow, Clime_Site_Name);
	else                    sprintf(Note, "%s%s_Fsnow_from_SnowDepth_6yr_median.dat", Path_Fsnow, Clime_Site_Name);

	if ((fpt_Output_Fsnow = fopen(Note, "wb")) == NULL) {
		printf("Cannot open output file %s\n", Note);
		exit(0);
	}

	sprintf(Note, "%s%s_Years_Fsnow_from_SnowDepth.dat", Path_Fsnow, Clime_Site_Name);
	if ((fpt_Output_Years = fopen(Note, "wb")) == NULL) {
		printf("Cannot open output file %s\n", Note);
		exit(0);
	}

	for (i = 0; i < YEARS; i++) {
		sprintf(Note, "%s%s_Fsnow_from_SnowDepth_%d.dat", Path_Fsnow, Clime_Site_Name, Year[i]);  //
		if ((fpt_Input_Fsnow[i] = fopen(Note, "rb")) == NULL) {
			printf("Cannot open output file %s\n", Note);
			exit(0);
		}
	}    //i

	for (Line = 0; Line < LINES; Line++) {

		if ((Line % 10) == 0) printf("%d ", Line);  //printf(".", Line);

		fread(Line_Fsnow_0, 2, PIXELS, fpt_Input_Fsnow[0]); 
		fread(Line_Fsnow_1, 2, PIXELS, fpt_Input_Fsnow[1]); 
		fread(Line_Fsnow_2, 2, PIXELS, fpt_Input_Fsnow[2]); 
		fread(Line_Fsnow_3, 2, PIXELS, fpt_Input_Fsnow[3]); 
		fread(Line_Fsnow_4, 2, PIXELS, fpt_Input_Fsnow[4]); 
		fread(Line_Fsnow_5, 2, PIXELS, fpt_Input_Fsnow[5]); 

		for (Pix = 0; Pix < PIXELS; Pix++) {
			N_Years = 0;
			if (Line_Fsnow_0[Pix] > -1) {
				Fsnow[N_Years] = Line_Fsnow_0[Pix];
				N_Years++;
			}
			if (Line_Fsnow_1[Pix] > -1) {
				Fsnow[N_Years] = Line_Fsnow_1[Pix];
				N_Years++;
			}
			if (Line_Fsnow_2[Pix] > -1) {
				Fsnow[N_Years] = Line_Fsnow_2[Pix];
				N_Years++;
			}
			if (Line_Fsnow_3[Pix] > -1) {
				Fsnow[N_Years] = Line_Fsnow_3[Pix];
				N_Years++;
			}
			if (Line_Fsnow_4[Pix] > -1) {
				Fsnow[N_Years] = Line_Fsnow_4[Pix];
				N_Years++;
			}
			if (Line_Fsnow_5[Pix] > -1) {
				Fsnow[N_Years] = Line_Fsnow_5[Pix];
				N_Years++;
			}

//calculate average
			if (N_Years == 0) avg = -200.5;
			else {
				if (Avg_or_Median == 1) {
					avg = 0;
					for (i = 0; i < N_Years; i++) avg += Fsnow[i];
					avg /= N_Years;
				}
				else {  //median
					if (N_Years == 1) avg = Fsnow[0];
					else if (N_Years == 2) avg = (Fsnow[0] + Fsnow[1]) / 2;
					else {   //median value
						for (i = 0; i < N_Years - 1; i++) {
							x[i] = Fsnow[i];
							for (j = i + 1; j < N_Years; j++) {
								if (Fsnow[j] <= x[i]) {
									F = x[i];
									x[i] = Fsnow[j];
									Fsnow[j] = F;
								}
							}
						}
						x[N_Years - 1] = Fsnow[N_Years - 1];

						i = N_Years / 2;
						if ((N_Years % 2) == 0) avg = (x[i - 1] + x[i]) / 2;
						else avg = x[i];

					}  //else
				} //else median
			}  //else


			Line_Data_Years[Pix] = (unsigned char)N_Years;
			Line_Fsnow_Output[Pix] = short(avg + 0.5);
		}  //Pix

		fwrite(Line_Data_Years, 1, PIXELS, fpt_Output_Years);
		fwrite(Line_Fsnow_Output, 2, PIXELS, fpt_Output_Fsnow);

	}  //Line

	for (i = 0; i < YEARS; i++) fclose(fpt_Input_Fsnow[i]);
	fclose(fpt_Output_Fsnow);
	fclose(fpt_Output_Years);

	delete[]Line_Fsnow_0;  	delete[]Line_Fsnow_1; 	delete[]Line_Fsnow_2;
	delete[]Line_Fsnow_3;  	delete[]Line_Fsnow_4; 	delete[]Line_Fsnow_5;

	delete[]Line_Fsnow_Output;  
	delete[]Line_Data_Years;
}

void Fsnow_caluclate_mean_relative_absolute_difference_from_avg() {

	char Path_Fsnow[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow_2026July\\";
	char Path_SnowEnd_u[] = "E:\\Snow_Alaska\\Data_From_Mitchell\\2026_YZ\\HV_SnowEndB_Layers\\";
	char Clime_Site_Name[] = "HV";  
	char Note[300];

	int i, j;

	int YEARS = 7, Year;
	int Line, Pix;

	int N_Years;
	double Fsnow[7], Fsnow_avg, avg, u_max = 5.51;
	double Relative_Diff;

	short* Line_Fsnow_avg = new short[PIXELS];

	short* Line_Fsnow_0 = new short[PIXELS], * Line_Fsnow_1 = new short[PIXELS],
		* Line_Fsnow_2 = new short[PIXELS], * Line_Fsnow_3 = new short[PIXELS],
		* Line_Fsnow_4 = new short[PIXELS], * Line_Fsnow_5 = new short[PIXELS],
		* Line_Fsnow_6 = new short[PIXELS];

	float* Line_SnowEnd_u0 = new float[PIXELS], * Line_SnowEnd_u1 = new float[PIXELS],
		* Line_SnowEnd_u2 = new float[PIXELS], * Line_SnowEnd_u3 = new float[PIXELS],
		* Line_SnowEnd_u4 = new float[PIXELS], * Line_SnowEnd_u5 = new float[PIXELS],
		* Line_SnowEnd_u6 = new float[PIXELS];

	short* Line_Output = new short[PIXELS];

	FILE* fpt_Input_Fsnow_avg, * fpt_Input_Fsnow[7], * fpt_Input_SnowEnd_u[7], * fpt_Output;

	sprintf(Note, "%s%s_mean_relative_abs_Diff.dat", Path_Fsnow, Clime_Site_Name);
	if ((fpt_Output = fopen(Note, "wb")) == NULL) {
		printf("Cannot open output file %s\n", Note);
		exit(0);
	}

	sprintf(Note, "%s%s_avg_Fsnow_7yr_avg.dat", Path_Fsnow, Clime_Site_Name);  //
	if ((fpt_Input_Fsnow_avg = fopen(Note, "rb")) == NULL) {
		printf("Cannot open output file %s\n", Note);
		exit(0);
	}

	for (i = 0; i < YEARS; i++) {
		Year = i + 2019;

		sprintf(Note, "%s%s_avg_Fsnow_%d.dat", Path_Fsnow, Clime_Site_Name, Year);  //
		if ((fpt_Input_Fsnow[i] = fopen(Note, "rb")) == NULL) {
			printf("Cannot open output file %s\n", Note);
			exit(0);
		}

		sprintf(Note, "%sHV_SnowEndB_u_%d.raw", Path_SnowEnd_u, Year);  //
		if ((fpt_Input_SnowEnd_u[i] = fopen(Note, "rb")) == NULL) {
			printf("Cannot open output file %s\n", Note);
			exit(0);
		}
	}    //i

	for (Line = 0; Line < LINES; Line++) {

		if ((Line % 10) == 0) printf("%d ", Line);  //printf(".", Line);

		fread(Line_Fsnow_avg, 2, PIXELS, fpt_Input_Fsnow_avg);

		fread(Line_Fsnow_0, 2, PIXELS, fpt_Input_Fsnow[0]); fread(Line_SnowEnd_u0, 4, PIXELS, fpt_Input_SnowEnd_u[0]);
		fread(Line_Fsnow_1, 2, PIXELS, fpt_Input_Fsnow[1]); fread(Line_SnowEnd_u1, 4, PIXELS, fpt_Input_SnowEnd_u[1]);
		fread(Line_Fsnow_2, 2, PIXELS, fpt_Input_Fsnow[2]); fread(Line_SnowEnd_u2, 4, PIXELS, fpt_Input_SnowEnd_u[2]);
		fread(Line_Fsnow_3, 2, PIXELS, fpt_Input_Fsnow[3]); fread(Line_SnowEnd_u3, 4, PIXELS, fpt_Input_SnowEnd_u[3]);
		fread(Line_Fsnow_4, 2, PIXELS, fpt_Input_Fsnow[4]); fread(Line_SnowEnd_u4, 4, PIXELS, fpt_Input_SnowEnd_u[4]);
		fread(Line_Fsnow_5, 2, PIXELS, fpt_Input_Fsnow[5]); fread(Line_SnowEnd_u5, 4, PIXELS, fpt_Input_SnowEnd_u[5]);
		fread(Line_Fsnow_6, 2, PIXELS, fpt_Input_Fsnow[6]); fread(Line_SnowEnd_u6, 4, PIXELS, fpt_Input_SnowEnd_u[6]);

		for (Pix = 0; Pix < PIXELS; Pix++) {
			N_Years = 0;
			Relative_Diff = 0;
			if (Line_Fsnow_avg[Pix] > -1) {  //not water/road
				if (Line_SnowEnd_u0[Pix] < u_max) {
					Relative_Diff += fabs(Line_Fsnow_0[Pix] - Line_Fsnow_avg[Pix]) / Line_Fsnow_avg[Pix] * 100.0;
					N_Years++;
				}
				if (Line_SnowEnd_u1[Pix] < u_max) {
					Relative_Diff += fabs(Line_Fsnow_1[Pix] - Line_Fsnow_avg[Pix]) / Line_Fsnow_avg[Pix] * 100.0;
					N_Years++;
				}
				if (Line_SnowEnd_u2[Pix] < u_max) {
					Relative_Diff += fabs(Line_Fsnow_2[Pix] - Line_Fsnow_avg[Pix]) / Line_Fsnow_avg[Pix] * 100.0;
					N_Years++;
				}
				if (Line_SnowEnd_u3[Pix] < u_max) {
					Relative_Diff += fabs(Line_Fsnow_3[Pix] - Line_Fsnow_avg[Pix]) / Line_Fsnow_avg[Pix] * 100.0;
					N_Years++;
				}
				if (Line_SnowEnd_u4[Pix] < u_max) {
					Relative_Diff += fabs(Line_Fsnow_4[Pix] - Line_Fsnow_avg[Pix]) / Line_Fsnow_avg[Pix] * 100.0;
					N_Years++;
				}
				if (Line_SnowEnd_u5[Pix] < u_max) {
					Relative_Diff += fabs(Line_Fsnow_5[Pix] - Line_Fsnow_avg[Pix]) / Line_Fsnow_avg[Pix] * 100.0;
					N_Years++;
				}
				if (Line_SnowEnd_u6[Pix] < u_max) {
					Relative_Diff += fabs(Line_Fsnow_6[Pix] - Line_Fsnow_avg[Pix]) / Line_Fsnow_avg[Pix] * 100.0;
					N_Years++;
				}

				if (N_Years > 0) 	Relative_Diff /= N_Years;
				else Relative_Diff = -20;
			}
			else Relative_Diff = -20.0;

			Line_Output[Pix] = short(Relative_Diff * 10 + 0.5);
		}  //Pix

		fwrite(Line_Output, 2, PIXELS, fpt_Output);
	}  //Line

	for (i = 0; i < YEARS; i++) {
		Year = i + 2019;
		fclose(fpt_Input_Fsnow[i]);
		fclose(fpt_Input_SnowEnd_u[i]);
	}
	fclose(fpt_Input_Fsnow_avg);

	fclose(fpt_Output);

	delete[]Line_Fsnow_avg;
	delete[]Line_Fsnow_0;  	delete[]Line_Fsnow_1; 	delete[]Line_Fsnow_2;
	delete[]Line_Fsnow_3;  	delete[]Line_Fsnow_4; 	delete[]Line_Fsnow_5; delete[]Line_Fsnow_6;

	delete[]Line_SnowEnd_u0; delete[]Line_SnowEnd_u1;  delete[]Line_SnowEnd_u2;
	delete[]Line_SnowEnd_u3; delete[]Line_SnowEnd_u4;  delete[]Line_SnowEnd_u5;  delete[]Line_SnowEnd_u6;

	delete[]Line_Output;
}




void Fsnow_caluclate_avg_or_Median() {

	char Path_Fsnow[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow_2026July\\";  
	char Path_SnowEnd_u[] = "E:\\Snow_Alaska\\Data_From_Mitchell\\2026_YZ\\HV_SnowEndB_Layers\\";
	char Clime_Site_Name[] = "HV";
	char Note[300];

	int Avg_or_Median = 1;  //1:avg, 2:median
	int i, j;

	int YEARS = 7, Year;
	int Line, Pix;

	int N_Years;
	double Fsnow[7], avg, u_max = 5.51;
	double F, x[7];

	short* Line_Fsnow_0 = new short[PIXELS], * Line_Fsnow_1 = new short[PIXELS],
		* Line_Fsnow_2 = new short[PIXELS], * Line_Fsnow_3 = new short[PIXELS],
		* Line_Fsnow_4 = new short[PIXELS], * Line_Fsnow_5 = new short[PIXELS],
		* Line_Fsnow_6 = new short[PIXELS];

	float* Line_SnowEnd_u0 = new float[PIXELS], * Line_SnowEnd_u1 = new float[PIXELS],
		* Line_SnowEnd_u2 = new float[PIXELS], * Line_SnowEnd_u3 = new float[PIXELS],
		* Line_SnowEnd_u4 = new float[PIXELS], * Line_SnowEnd_u5 = new float[PIXELS],
		* Line_SnowEnd_u6 = new float[PIXELS];

	short* Line_Fsnow_Output = new short[PIXELS];
	unsigned char *Line_Data_Years = new unsigned char[PIXELS];

	FILE* fpt_Input_Fsnow[7], * fpt_Input_SnowEnd_u[7], * fpt_Output_Fsnow, * fpt_Output_Years;

	
	if(Avg_or_Median == 1) sprintf(Note, "%s%s_avg_Fsnow_7yr_avg.dat", Path_Fsnow, Clime_Site_Name);  
	else                   sprintf(Note, "%s%s_avg_Fsnow_7yr_median.dat", Path_Fsnow, Clime_Site_Name);

	if ((fpt_Output_Fsnow = fopen(Note, "wb")) == NULL) {
		printf("Cannot open output file %s\n", Note);
		exit(0);
	}

	sprintf(Note, "%s%s_Years_u_LE_55.dat", Path_Fsnow, Clime_Site_Name);  //
	if ((fpt_Output_Years = fopen(Note, "wb")) == NULL) {
		printf("Cannot open output file %s\n", Note);
		exit(0);
	}

	for (i = 0; i < YEARS; i++) {
		Year = i + 2019;

		sprintf(Note, "%s%s_avg_Fsnow_%d.dat", Path_Fsnow, Clime_Site_Name, Year);  //
		if ((fpt_Input_Fsnow[i] = fopen(Note, "rb")) == NULL) {
			printf("Cannot open output file %s\n", Note);
			exit(0);
		}

		sprintf(Note, "%sHV_SnowEndB_u_%d.raw", Path_SnowEnd_u, Year);  //
		if ((fpt_Input_SnowEnd_u[i] = fopen(Note, "rb")) == NULL) {
			printf("Cannot open output file %s\n", Note);
			exit(0);
		}
	}    //i

	for (Line = 0; Line < LINES; Line++) {

		if ((Line % 10) == 0) printf("%d ", Line);  //printf(".", Line);

		fread(Line_Fsnow_0, 2, PIXELS, fpt_Input_Fsnow[0]); fread(Line_SnowEnd_u0, 4, PIXELS, fpt_Input_SnowEnd_u[0]);
		fread(Line_Fsnow_1, 2, PIXELS, fpt_Input_Fsnow[1]); fread(Line_SnowEnd_u1, 4, PIXELS, fpt_Input_SnowEnd_u[1]);
		fread(Line_Fsnow_2, 2, PIXELS, fpt_Input_Fsnow[2]); fread(Line_SnowEnd_u2, 4, PIXELS, fpt_Input_SnowEnd_u[2]);
		fread(Line_Fsnow_3, 2, PIXELS, fpt_Input_Fsnow[3]); fread(Line_SnowEnd_u3, 4, PIXELS, fpt_Input_SnowEnd_u[3]);
		fread(Line_Fsnow_4, 2, PIXELS, fpt_Input_Fsnow[4]); fread(Line_SnowEnd_u4, 4, PIXELS, fpt_Input_SnowEnd_u[4]);
		fread(Line_Fsnow_5, 2, PIXELS, fpt_Input_Fsnow[5]); fread(Line_SnowEnd_u5, 4, PIXELS, fpt_Input_SnowEnd_u[5]);
		fread(Line_Fsnow_6, 2, PIXELS, fpt_Input_Fsnow[6]); fread(Line_SnowEnd_u6, 4, PIXELS, fpt_Input_SnowEnd_u[6]);

		for (Pix = 0; Pix < PIXELS; Pix++) {
			N_Years = 0;
			if (Line_Fsnow_0[Pix] > -1) {
				N_Years = Assign_Fsnow(N_Years, Line_SnowEnd_u0[Pix], Line_Fsnow_0[Pix], u_max, Fsnow);
				N_Years = Assign_Fsnow(N_Years, Line_SnowEnd_u1[Pix], Line_Fsnow_1[Pix], u_max, Fsnow);
				N_Years = Assign_Fsnow(N_Years, Line_SnowEnd_u2[Pix], Line_Fsnow_2[Pix], u_max, Fsnow);
				N_Years = Assign_Fsnow(N_Years, Line_SnowEnd_u3[Pix], Line_Fsnow_3[Pix], u_max, Fsnow);
				N_Years = Assign_Fsnow(N_Years, Line_SnowEnd_u4[Pix], Line_Fsnow_4[Pix], u_max, Fsnow);
				N_Years = Assign_Fsnow(N_Years, Line_SnowEnd_u5[Pix], Line_Fsnow_5[Pix], u_max, Fsnow);
				N_Years = Assign_Fsnow(N_Years, Line_SnowEnd_u6[Pix], Line_Fsnow_6[Pix], u_max, Fsnow);
			}
//calculate average
			if (N_Years == 0) avg = -200.5;
			else {
				if (Avg_or_Median == 1) {
					avg = 0;
					for (i = 0; i < N_Years; i++) avg += Fsnow[i];
					avg /= N_Years;
				}
				else {  //median
					if     (N_Years==1) avg = Fsnow[0];
					else if(N_Years == 2) avg = (Fsnow[0]+Fsnow[1])/2;
					else {   //median value
						for (i = 0; i < N_Years - 1; i++) {
							x[i] = Fsnow[i];
							for (j = i + 1; j < N_Years; j++) {
								if (Fsnow[j] <= x[i]) {
									F = x[i];
									x[i] = Fsnow[j];
									Fsnow[j] = F;
								}
							}
						}
						x[N_Years - 1] = Fsnow[N_Years - 1];

						i = N_Years / 2;
						if ((N_Years % 2) == 0) avg = (x[i-1] + x[i]) / 2;
						else avg = x[i];

					}  //else
				} //else median
			}  //else


			Line_Data_Years[Pix] = (unsigned char) N_Years;
			Line_Fsnow_Output[Pix] = short(avg + 0.5);
		}  //Pix

		fwrite(Line_Data_Years, 1, PIXELS, fpt_Output_Years);
		fwrite(Line_Fsnow_Output, 2, PIXELS, fpt_Output_Fsnow);

	}  //Line

	for (i = 0; i < YEARS; i++) {
		Year = i + 2019;
		fclose(fpt_Input_Fsnow[i]);
		fclose(fpt_Input_SnowEnd_u[i]);
	}
	fclose(fpt_Output_Fsnow);
	fclose(fpt_Output_Years);

	delete[]Line_Fsnow_0;  	delete[]Line_Fsnow_1; 	delete[]Line_Fsnow_2;
	delete[]Line_Fsnow_3;  	delete[]Line_Fsnow_4; 	delete[]Line_Fsnow_5; delete[]Line_Fsnow_6;

	delete[]Line_SnowEnd_u0; delete[]Line_SnowEnd_u1;  delete[]Line_SnowEnd_u2;
	delete[]Line_SnowEnd_u3; delete[]Line_SnowEnd_u4;  delete[]Line_SnowEnd_u5;  delete[]Line_SnowEnd_u6;

	delete[]Line_Fsnow_Output;  delete[]Line_Data_Years;
}


int Assign_Fsnow(int N_Years, float SnowEnd_u, short Fsnow_x, double u_max, double* Fsnow)
{
	if (SnowEnd_u < u_max) {
			Fsnow[N_Years] = Fsnow_x;
			N_Years++;
	}
	return N_Years;
}


void Output_Values_of_A_Pixel_HV()
{
	char Path_In_Area[] = "E:\\Snow_Alaska\\HV\\HV_30m\\";                            //for spatial input
	char Path_In_Spatial_Output[] = "E:\\Snow_Alaska\\Modelling\\HV\\HV_30m_Spatial_Output\\";
	char Path_Out_Area[] =          "E:\\Snow_Alaska\\Modelling\\HV\\HV_30m_Spatial_Output\\Analysis\\";  //_5m_Spatial_Output

	char Path[100];
	char Note[200];

	int Years[11] = { 2012, 2013, 2015, 2016, 2017, 2018, 2019, 2020, 2021, 2022, 2023 };
	int DOY[11] = { 107, 103, 98, 96, 102, 103, 0, 0, 0, 0, 0 };

	int Output_Line, Output_Pix, Skip;
	int yy, LC;
	double Height;

	float Slope, Aspect, Latitude;
	short LAI, SnowStatus[11], Fsnow[11];

	unsigned char a8U;

	FILE* fpt_Output, * fpt_LC, * fpt_LAI, * fpt_Slope, * fpt_Aspect, * fpt_SnowStatus[11], * fpt_Fsnow[11];

	Output_Pix = 16;
	Output_Line = 3;

	Skip = (Output_Line - 1) * PIXELS + Output_Pix - 1;
	Latitude = 69.20066667 - Output_Line*30.0 / (110.0 * 1000.0);  //from top

	//Mapping: open image files  --------------------------------
	sprintf(Note, "%sLP_%d_%d.txt", Path_Out_Area, Output_Line, Output_Pix);  
	if ((fpt_Output = fopen(Note, "w")) == NULL) {
		printf("Cannot open output file %s\n", Note);
		exit(0);
	}

		sprintf(Note, "%sHV_30m_Landcover.raw", Path_In_Area);  //8U, _5m
		//	sprintf(Note, "%sHV_30m_Landcover.raw", Path_In_Area);  //8U, HV_5m
		if ((fpt_LC = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}

		sprintf(Note, "%sHV_30m_LAI.raw", Path_In_Area);  //16S, LAI*1000,  _5m
		if ((fpt_LAI = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}
		sprintf(Note, "%sHV_30m_ArcticDEM_Slope.raw", Path_In_Area);  //32R, degrees, _5m, 30m
		//	sprintf(Note, "%sHV_5m_Slope.raw", Path_In_Area);  //32R, degrees
		if ((fpt_Slope = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}
		sprintf(Note, "%sHV_30m_ArcticDEM_Aspect.raw", Path_In_Area);  //32R, degrees, _5m, 30m
		//	sprintf(Note, "%sHV_5m_Aspect.raw", Path_In_Area);  //32R, degrees
		if ((fpt_Aspect = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}

		for (yy = 0; yy < 11; yy++) {
			if (Years[yy] <= 2018) sprintf(Note, "%sSnowDepth_30m_%d.raw", Path_In_Area, Years[yy]);  //16S, 2012_2018
			else                   sprintf(Note, "%sWinterYear_%d_%d.raw", Path_In_Area, Years[yy - 1], Years[yy]);  //16S, 2019_2023
			if ((fpt_SnowStatus[yy] = fopen(Note, "rb")) == NULL) {
				printf("Cannot open input file %s\n", Note);
				exit(0);
			}

			sprintf(Note, "%sHV_30m_Fsnow_%d.dat", Path_In_Spatial_Output, Years[yy]);  //16S, 2012_2023
			if ((fpt_Fsnow[yy] = fopen(Note, "rb")) == NULL) {
				printf("Cannot open input file %s\n", Note);
				exit(0);
			}
		}

		fseek(fpt_LC, Skip, SEEK_SET); fread(&a8U, 1, 1, fpt_LC);
		LC = a8U;

		fseek(fpt_LAI, Skip*2, SEEK_SET); fread(&LAI, 2, 1, fpt_LAI);

		fseek(fpt_Slope,  Skip * 4, SEEK_SET); fread(&Slope,  4, 1, fpt_Slope);
		fseek(fpt_Aspect, Skip * 4, SEEK_SET); fread(&Aspect, 4, 1, fpt_Aspect);

		for (yy = 0; yy < 11; yy++) {
			fseek(fpt_SnowStatus[yy], Skip * 2, SEEK_SET); fread(&SnowStatus[yy], 2, 1, fpt_SnowStatus[yy]);
			fseek(fpt_Fsnow[yy],      Skip * 2, SEEK_SET); fread(&Fsnow[yy], 2, 1, fpt_Fsnow[yy]);
		}

		fprintf(fpt_Output, "LinePixel %d %d\n", Output_Line, Output_Pix);
		fprintf(fpt_Output, "Latitude %.5f\n", Latitude);

		if (LC == 51 || LC == 52) {
			fprintf(fpt_Output, "Type Shrub\n");
			Height = 0.3 + LAI * 0.001 * 0.5;
		}
		else if (LC == 72 || LC == 95) {
			fprintf(fpt_Output, "Type Sedge\n");
			Height = 0.2 + LAI * 0.001 * 0.3;
		}
		else {
			Height = 0.1;
			fprintf(fpt_Output, "Type Water\n");
		}

		fprintf(fpt_Output, "LAI %.3lf\n", LAI * 0.001);
		fprintf(fpt_Output, "Height %.3lf\n", Height);

		fprintf(fpt_Output, "Slope %.3f\n", Slope);
		fprintf(fpt_Output, "Aspect %.3f\n", Aspect);

		fprintf(fpt_Output, "Year ");
		for (yy = 0; yy < 11; yy++) fprintf(fpt_Output, "%d ", Years[yy]);
		fprintf(fpt_Output, "\n");

		fprintf(fpt_Output, "DOY ");
		for (yy = 0; yy < 6; yy++) fprintf(fpt_Output, "%d ", DOY[yy]);
		fprintf(fpt_Output, "\n");

		fprintf(fpt_Output, "SnowStatus ");
		for (yy = 0; yy < 11; yy++) {
			if(Years[yy]<=2018) fprintf(fpt_Output, "%.1lf ", SnowStatus[yy]*0.1);
			else                fprintf(fpt_Output, "%.0lf ", SnowStatus[yy] * 1.0);
		}
		fprintf(fpt_Output, "\n");

		fprintf(fpt_Output, "Fsnow ");
		for (yy = 0; yy < 11; yy++) fprintf(fpt_Output, "%.3lf ", Fsnow[yy]*0.001);
		fprintf(fpt_Output, "\n");

	fclose(fpt_LC);
	fclose(fpt_LAI);
	fclose(fpt_Slope);
	fclose(fpt_Aspect);

	for (yy = 0; yy < 11; yy++) {
		fclose(fpt_SnowStatus[yy]);
		fclose(fpt_Fsnow[yy]);
	}

	fclose(fpt_Output);
}

void Mapping_Snow_Status_HV()
{
	char Common_Path[]   = "E:\\Snow_Alaska\\Modelling_2026\\";
	char Path_In_Area[]  = "E:\\Snow_Alaska\\HV\\HV_30m\\";                    
	char Path_In_Fsnow[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow\\";
	char Path_Out_Area[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_SnowDepth_2026July\\";
	char Clim_Site_Name[] = "HV";

	char Path[300], File_Name[300];
	char Note[200];

	//0:Shrub, 1:Sedge
	char CoverType_Name[2][7] = { "Shrub", "Sedge" };

	int n, N, i_LAI, i_Slope, i_Aspect, i_Fsnow, N_LAI[2], N_Slope[2], N_Aspect[2], N_Fsnow[2];

	int Type, R, IF_Data;
	double LAI_Avg[2][LAI_NN], Slope_Avg[2][Slope_NN], Aspect_Avg[2][Aspect_NN], Fsnow_Avg[2][Fsnow_NN];

	int Year;
	int Num_Input_Values = 7;  //for observation: 7 years from 2012 to 2018, 7: 2019-2025

	double ALL_Loc_Lines[2], Loc_Line[2]; //, ALL_Loc_Pixels[Total_Rows], Loc_Pix[2], Total_Sites = Total_Rows;

	double LAI, Slope, Aspect, Fsnow;
	double xx_LAI[2], xx_Slope[2], xx_Aspect[2], xx_Fsnow[2];

	double Loc2_Snow111[2][2], Loc2_Snow112[2][2], Loc2_Snow121[2][2], Loc2_Snow122[2][2],
		Loc2_Snow211[2][2], Loc2_Snow212[2][2], Loc2_Snow221[2][2], Loc2_Snow222[2][2];

	double* pLoc2_Snow111[2], * pLoc2_Snow112[2], * pLoc2_Snow121[2], * pLoc2_Snow122[2],
		* pLoc2_Snow211[2], * pLoc2_Snow212[2], * pLoc2_Snow221[2], * pLoc2_Snow222[2];

	float* pL_Values_Sedge = new float[Total_Rows * Sedge_LAI_NN * Sedge_Slope_NN * Sedge_Aspect_NN * Sedge_Fsnow_NN],
		 * pL_Values_Shrub = new float[Total_Rows * Shrub_LAI_NN * Shrub_Slope_NN * Shrub_Aspect_NN * Shrub_Fsnow_NN];

	int Line, Pix, LC;
	double yy;

	unsigned char* Line_LC_8U = new unsigned char[PIXELS];
	short* Line_LAI_16S = new short[PIXELS], a16S;
	float* Line_Slope = new float[PIXELS];
	float* Line_Aspect = new float[PIXELS];
	short* Line_Fsnow = new short[PIXELS];

	short* Line_Output_16S = new short[PIXELS];       //snow depth

	FILE* fpt_Output, * fpt_In, * fpt_LC, * fpt_LAI, * fpt_Slope, * fpt_Aspect, * fpt_Fsnow;

	for (n = 0; n < 2; n++) {
		pLoc2_Snow111[n] = Loc2_Snow111[n];
		pLoc2_Snow112[n] = Loc2_Snow112[n];
		pLoc2_Snow121[n] = Loc2_Snow121[n];
		pLoc2_Snow122[n] = Loc2_Snow122[n];

		pLoc2_Snow211[n] = Loc2_Snow211[n];
		pLoc2_Snow212[n] = Loc2_Snow212[n];
		pLoc2_Snow221[n] = Loc2_Snow221[n];
		pLoc2_Snow222[n] = Loc2_Snow222[n];
	}

	//Lines for the climate locations
	ALL_Loc_Lines[0] = 1. - 1;
	ALL_Loc_Lines[1] = LINES - 1.0;  //HV: using LINES
//	ALL_Loc_Lines[1] = PIXELS - 1.0;  //CLPX using PIXELS
	for (R = 0; R < 2; R++) Loc_Line[R] = ALL_Loc_Lines[R];

	for (Type = 0; Type < 2; Type++) {   //2

		sprintf(Path, "%sModel_Input_Output\\%s\\Input\\", Common_Path, CoverType_Name[Type]);

		//read LAI
		sprintf(Note, "%s%s_LAI_N.txt", Path, CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_LAI[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_LAI[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_LAI, &LAI_Avg[Type][N]);
		}
		fclose(fpt_In);

		//read Slope
		sprintf(Note, "%s%s_Slope_N.txt", Path, CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_Slope[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_Slope[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_Slope, &Slope_Avg[Type][N]);
		}
		fclose(fpt_In);

		//read Aspect
		sprintf(Note, "%s%s_Aspect_N.txt", Path, CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_Aspect[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_Aspect[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_Aspect, &Aspect_Avg[Type][N]);
		}
		fclose(fpt_In);

		//Read Fsnow
		sprintf(Note, "%s%s_Fsnow_N.txt", Path, CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_Fsnow[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_Fsnow[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_Fsnow, &Fsnow_Avg[Type][N]);
		}
		fclose(fpt_In);
	}   //type

	sprintf(Note, "%s%s_SnowDepth_using_7yr_avg_Fsnow_2012_2018.dat", Path_Out_Area, Clim_Site_Name);
	if ((fpt_Output = fopen(Note, "wb")) == NULL) {
		printf("Cannot open output file %s\n", Note);
		exit(0);
	}

	for (Year = 2012; Year <= 2018; Year++) {  //2012 to 2018 for Ob years, 6 yrs
		if (Year == 2014) Year = 2015;  //skip year 2014
		printf("\n%d\n", Year);

		//Input model results at L1 and L2 ---------------------------------------
		Type = 0;
		sprintf(Path, "%sModel_Input_Output\\%s\\From_Output_for_Mapping\\SnowDepthSWE\\", Common_Path, CoverType_Name[Type]);
		sprintf(File_Name, "%s%s_%s_SnowDepth_Apr_ObYears", Path, Clim_Site_Name, CoverType_Name[Type]);
//		sprintf(File_Name, "%s%s_%s_SnowDepth_avg_Apr_5_17_2019_2025", Path, Clim_Site_Name, CoverType_Name[Type]);
		Read_Model_Results_for_Two_Sites_Fsnow(File_Name, Num_Input_Values, Year, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type], pL_Values_Shrub);

		Type = 1;
		sprintf(Path, "%sModel_Input_Output\\%s\\From_Output_for_Mapping\\SnowDepthSWE\\", Common_Path, CoverType_Name[Type]);
		sprintf(File_Name, "%s%s_%s_SnowDepth_Apr_ObYears", Path, Clim_Site_Name, CoverType_Name[Type]);
//		sprintf(File_Name, "%s%s_%s_SnowDepth_avg_Apr_5_17_2019_2025", Path, Clim_Site_Name, CoverType_Name[Type]);
		Read_Model_Results_for_Two_Sites_Fsnow(File_Name, Num_Input_Values, Year, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type], pL_Values_Sedge);

//Mapping: open image files  --------------------------------
		sprintf(Note, "%sHV_30m_Landcover.raw", Path_In_Area);  //8U, _5m
		if ((fpt_LC = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}
		sprintf(Note, "%sHV_30m_LAI.raw", Path_In_Area);  //16S, LAI*1000,  _5m
		if ((fpt_LAI = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}
		sprintf(Note, "%sHV_30m_ArcticDEM_Slope.raw", Path_In_Area);  //32R, degrees, _5m, 30m
		if ((fpt_Slope = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}
		sprintf(Note, "%sHV_30m_ArcticDEM_Aspect.raw", Path_In_Area);  //32R, degrees, _5m, 30m
		if ((fpt_Aspect = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}

		sprintf(Note, "%sHV_avg_Fsnow_7yr_avg.dat", Path_In_Fsnow);  //16S, 2019_2025 avg
//		sprintf(Note, "%sHV_Fsnow_estimated_from_7yr_avg_Fsnow_%d.dat", Path_In_Fsnow, Year);  //16S, individual year
		if ((fpt_Fsnow = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}

		for (Line = 0; Line < LINES; Line++) {

			if ((Line % 10) == 0) printf("%d ", Line);  //printf(".", Line);

			fread(Line_LC_8U, 1, PIXELS, fpt_LC);
			fread(Line_LAI_16S, 2, PIXELS, fpt_LAI);
			fread(Line_Slope, 4, PIXELS, fpt_Slope);
			fread(Line_Aspect, 4, PIXELS, fpt_Aspect);
			fread(Line_Fsnow, 2, PIXELS, fpt_Fsnow);

			for (Pix = 0; Pix < PIXELS; Pix++) {
				LC = Line_LC_8U[Pix];
				LAI = Line_LAI_16S[Pix] * 0.001;
				Slope = Line_Slope[Pix];
				Aspect = Line_Aspect[Pix];

				Fsnow = Line_Fsnow[Pix] * 0.001;

				if(Fsnow > 0.0) IF_Data = 1;  //no data is -0.2;
				else IF_Data = 0;

				Fsnow = 1.0 - Fsnow;

//if (Line == 26 && Pix == 442)
//	N = N;

//			Landcover in HV: 11:open water, 51:Dwarf shrub, 52:Shrub/scrub, 72: Sedge, 95:herbasceous wetland

				if (IF_Data == 1) {
					if (LC == 51 || LC == 52) {   //Shrub
						Type = 0;
						Get_XY_Values_for_The_Two_Locations_Snow_Status(LAI, Slope, Aspect, Fsnow, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type],
							LAI_Avg[Type], Slope_Avg[Type], Aspect_Avg[Type], Fsnow_Avg[Type], xx_LAI, xx_Slope, xx_Aspect, xx_Fsnow, pL_Values_Shrub,
							pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
							pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);

						yy = Linear_Interpolation_Snow_Status(Pix, Loc_Line, LAI, Slope, Aspect, Fsnow, xx_LAI, xx_Slope, xx_Aspect, xx_Fsnow,  //HV using line, CLPX using Pix
							pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122, pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);
					}
					else if (LC == 72 || LC == 95) {   //Sedge
						Type = 1;
						Get_XY_Values_for_The_Two_Locations_Snow_Status(LAI, Slope, Aspect, Fsnow, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type],
							LAI_Avg[Type], Slope_Avg[Type], Aspect_Avg[Type], Fsnow_Avg[Type], xx_LAI, xx_Slope, xx_Aspect, xx_Fsnow, pL_Values_Sedge,
							pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
							pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);

						yy = Linear_Interpolation_Snow_Status(Pix, Loc_Line, LAI, Slope, Aspect, Fsnow, xx_LAI, xx_Slope, xx_Aspect, xx_Fsnow,  //HV using line, CLPX using Pix
							pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122, pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);
					}  //if LC
					else yy = -1.;  //water
				}   //
				else  yy = -1.;  //not snowcondition data

				if (yy < 0) a16S = -9999;
				else        a16S = int(yy * 10.0 + 0.5);  //cm to mm
				Line_Output_16S[Pix] = a16S;
			}  //Pix loop

			fwrite(Line_Output_16S, 2, PIXELS, fpt_Output);
		}  //Line loop

	fclose(fpt_LC);
	fclose(fpt_LAI);
	fclose(fpt_Fsnow);
	fclose(fpt_Slope);
	fclose(fpt_Aspect);

}  //year loop

fclose(fpt_Output);

	delete[]pL_Values_Sedge;
	delete[]pL_Values_Shrub;

	delete[]Line_LC_8U;
	delete[]Line_LAI_16S;
	delete[]Line_Slope;
	delete[]Line_Aspect;
	delete[]Line_Fsnow;

	delete[]Line_Output_16S;

	printf("\nFinihsed! ");
}

/*
void Mapping_Snow_Status_HV()
{
	char Path_In_LAI_Fsnow[] = "E:\\Snow_Alaska\\Modelling\\HV\\Model_Output\\";      //for category N of LAI, SLope, Aspect and Fsnow
	char Path_In_Area[] = "E:\\Snow_Alaska\\HV\\HV_30m\\";                            //for spatial input
	char Path_In_from_NEST[] = "E:\\Snow_Alaska\\Modelling\\HV\\Generated_from_Model_Output\\";  //generated yearly values from model output

	char Path_Out_Area[] = "E:\\Snow_Alaska\\Modelling\\HV\\HV_30m_Spatial_Output\\Analysis\\";  //_5m_Spatial_Output

	char Path[100];
	char Note[200];

	//0:Shrub, 1:Sedge
	char CoverType_Name[2][7] = { "Shrub", "Sedge" };

	int n, N, i_LAI, i_Slope, i_Aspect, i_Fsnow, N_LAI[2], N_Slope[2], N_Aspect[2], N_Fsnow[2];

	int Type, R, IF_Data;
	double LAI_Avg[2][LAI_NN], Slope_Avg[2][Slope_NN], Aspect_Avg[2][Aspect_NN], Fsnow_Avg[2][Fsnow_NN];

	int Modelled_Year[23] = {2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,
						     2011,2012,2013,2014,2015,2016,2017,2018,2019,2020,2021,2022,2023};
	//double Fsnow_adj[23] = { 0.613,0.651,0.963,1.170,1.060,0.745,0.787,0.654,0.864,0.765,
	//						1.410,1.035,0.699,1.553,1.046,0.608,1.334,1.576,1.129,1.250,
	//						0.858,0.920,1.308 };  //ratio of snowfall from Nov to March to aerage from 2001 to 2023

	int Output_Calandar_Year, Output_yy = 12;  //0 to 22. 11-17 for 2012 - 2018, 

	int Output_Variable = 2; //0, 1,or 2. 0:Snowcover_day1, 1:SnowCover_day2, 2:SnowDepth 

	double ALL_Loc_Lines[2], Loc_Line[2]; //, ALL_Loc_Pixels[Total_Rows], Loc_Pix[2], Total_Sites = Total_Rows;

	double LAI, Slope, Aspect, Fsnow0, Fsnow;
	double xx_LAI[2], xx_Slope[2], xx_Aspect[2], xx_Fsnow[2];

	double Loc2_Snow111[2][2], Loc2_Snow112[2][2], Loc2_Snow121[2][2], Loc2_Snow122[2][2],
		Loc2_Snow211[2][2], Loc2_Snow212[2][2], Loc2_Snow221[2][2], Loc2_Snow222[2][2];

	double* pLoc2_Snow111[2], * pLoc2_Snow112[2], * pLoc2_Snow121[2], * pLoc2_Snow122[2],
		* pLoc2_Snow211[2], * pLoc2_Snow212[2], * pLoc2_Snow221[2], * pLoc2_Snow222[2];

	float* pL_Values_Sedge = new float[Total_Rows * Sedge_LAI_NN * Sedge_Slope_NN * Sedge_Aspect_NN * Sedge_Fsnow_NN],
		* pL_Values_Shrub = new float[Total_Rows * Shrub_LAI_NN * Shrub_Slope_NN * Shrub_Aspect_NN * Shrub_Fsnow_NN];

	int Line, Pix, LC;
	double yy;

	unsigned char* Line_LC_8U = new unsigned char[PIXELS];
	short* Line_LAI_16S = new short[PIXELS], a16S;
	float* Line_Slope = new float[PIXELS];
	float* Line_Aspect = new float[PIXELS];
	short* Line_Fsnow = new short[PIXELS];

	short* Line_Output_16S = new short[PIXELS];       //snow depth

	FILE* fpt_Output, * fpt_In, * fpt_LC, * fpt_LAI, * fpt_Slope, * fpt_Aspect, * fpt_Fsnow;

	Output_Calandar_Year = Modelled_Year[Output_yy];
	printf("%d\n", Output_Calandar_Year);

	for (n = 0; n < 2; n++) {
		pLoc2_Snow111[n] = Loc2_Snow111[n];
		pLoc2_Snow112[n] = Loc2_Snow112[n];
		pLoc2_Snow121[n] = Loc2_Snow121[n];
		pLoc2_Snow122[n] = Loc2_Snow122[n];

		pLoc2_Snow211[n] = Loc2_Snow211[n];
		pLoc2_Snow212[n] = Loc2_Snow212[n];
		pLoc2_Snow221[n] = Loc2_Snow221[n];
		pLoc2_Snow222[n] = Loc2_Snow222[n];
	}

	//Lines for the climate locations
	ALL_Loc_Lines[0] = 1. - 1;
	ALL_Loc_Lines[1] = LINES - 1.0;  //HV: using LINES

	for (R = 0; R < 2; R++) Loc_Line[R] = ALL_Loc_Lines[R];

	for (Type = 0; Type < 2; Type++) {   //2
		//read LAI
		sprintf(Note, "%s%s\\%s_LAI_N.txt", Path_In_LAI_Fsnow, CoverType_Name[Type], CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_LAI[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_LAI[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_LAI, &LAI_Avg[Type][N]);
		}
		fclose(fpt_In);

		//read Slope
		sprintf(Note, "%s%s\\%s_Slope_N.txt", Path_In_LAI_Fsnow, CoverType_Name[Type], CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_Slope[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_Slope[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_Slope, &Slope_Avg[Type][N]);
		}
		fclose(fpt_In);

		//read Aspect
		sprintf(Note, "%s%s\\%s_Aspect_N.txt", Path_In_LAI_Fsnow, CoverType_Name[Type], CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_Aspect[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_Aspect[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_Aspect, &Aspect_Avg[Type][N]);
		}
		fclose(fpt_In);

		//Read Fsnow
		sprintf(Note, "%s%s\\%s_Fsnow_N.txt", Path_In_LAI_Fsnow, CoverType_Name[Type], CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_Fsnow[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_Fsnow[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_Fsnow, &Fsnow_Avg[Type][N]);
		}
		fclose(fpt_In);
	}   //type

//Input model results at L1 and L2 ---------------------------------------
	Type = 0;
	sprintf(Path, "%s%s\\", Path_In_from_NEST, CoverType_Name[Type]);
	Read_Model_Results_for_Two_Sites_Fsnow(Path, Output_Calandar_Year, Output_Variable, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type], pL_Values_Shrub);

	Type = 1;
	sprintf(Path, "%s%s\\", Path_In_from_NEST, CoverType_Name[Type]);
	Read_Model_Results_for_Two_Sites_Fsnow(Path, Output_Calandar_Year, Output_Variable, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type], pL_Values_Sedge);

	//Mapping: open image files  --------------------------------
	sprintf(Note, "%sHV_30m_SnowDepth_%d_Modelled.dat", Path_Out_Area, Output_Calandar_Year);  //_5m, _30m
	if ((fpt_Output = fopen(Note, "wb")) == NULL) {
		printf("Cannot open output file %s\n", Note);
		exit(0);
	}

	sprintf(Note, "%sHV_30m_Landcover.raw", Path_In_Area);  //8U, _5m
	//	sprintf(Note, "%sHV_30m_Landcover.raw", Path_In_Area);  //8U, HV_5m
	if ((fpt_LC = fopen(Note, "rb")) == NULL) {
		printf("Cannot open input file %s\n", Note);
		exit(0);
	}
	sprintf(Note, "%sHV_30m_LAI.raw", Path_In_Area);  //16S, LAI*1000,  _5m
	if ((fpt_LAI = fopen(Note, "rb")) == NULL) {
		printf("Cannot open input file %s\n", Note);
		exit(0);
	}
	sprintf(Note, "%sHV_30m_ArcticDEM_Slope.raw", Path_In_Area);  //32R, degrees, _5m, 30m
	//	sprintf(Note, "%sHV_5m_Slope.raw", Path_In_Area);  //32R, degrees
	if ((fpt_Slope = fopen(Note, "rb")) == NULL) {
		printf("Cannot open input file %s\n", Note);
		exit(0);
	}
	sprintf(Note, "%sHV_30m_ArcticDEM_Aspect.raw", Path_In_Area);  //32R, degrees, _5m, 30m
	//	sprintf(Note, "%sHV_5m_Aspect.raw", Path_In_Area);  //32R, degrees
	if ((fpt_Aspect = fopen(Note, "rb")) == NULL) {
		printf("Cannot open input file %s\n", Note);
		exit(0);
	}

	sprintf(Note, "%sHV_30m_Fsnow_avg_2019_2023.dat", Path_Out_Area);  //16S, 2019_2023
	if ((fpt_Fsnow = fopen(Note, "rb")) == NULL) {
		printf("Cannot open input file %s\n", Note);
		exit(0);
	}

	for (Line = 0; Line < LINES; Line++) {

		if ((Line % 100) == 0) printf("%d ", Line);  //printf(".", Line);

		fread(Line_LC_8U, 1, PIXELS, fpt_LC);
		fread(Line_LAI_16S, 2, PIXELS, fpt_LAI);
		fread(Line_Slope, 4, PIXELS, fpt_Slope);
		fread(Line_Aspect, 4, PIXELS, fpt_Aspect);
		fread(Line_Fsnow, 2, PIXELS, fpt_Fsnow);

		for (Pix = 0; Pix < PIXELS; Pix++) {
			LC = Line_LC_8U[Pix];
			LAI = Line_LAI_16S[Pix] * 0.001;
			Slope = Line_Slope[Pix];
			Aspect = Line_Aspect[Pix];

			Fsnow0 = Line_Fsnow[Pix] * 0.001;
//			Fsnow = Fsnow0 * Fsnow_adj[Output_yy];

			if (Fsnow > 1.1) IF_Data = 0;
			else IF_Data = 1;

			//			Landcover in HV: 11:open water, 51:Dwarf shrub, 52:Shrub/scrub, 72: Sedge, 95:herbasceous wetland

			if (IF_Data == 1) {
				if (LC == 51 || LC == 52) {   //Shrub
					Type = 0;
					Get_XY_Values_for_The_Two_Locations_Snow_Status(LAI, Slope, Aspect, Fsnow, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type],
						LAI_Avg[Type], Slope_Avg[Type], Aspect_Avg[Type], Fsnow_Avg[Type], xx_LAI, xx_Slope, xx_Aspect, xx_Fsnow, pL_Values_Shrub,
						pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
						pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);

					yy = Linear_Interpolation_Snow_Status(Line, Loc_Line, LAI, Slope, Aspect, Fsnow, xx_LAI, xx_Slope, xx_Aspect, xx_Fsnow,  //HV using line, CLPX using Pix
						pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122, pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);
				}
				else if (LC == 72 || LC == 95) {   //Sedge
					Type = 1;
					Get_XY_Values_for_The_Two_Locations_Snow_Status(LAI, Slope, Aspect, Fsnow, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type],
						LAI_Avg[Type], Slope_Avg[Type], Aspect_Avg[Type], Fsnow_Avg[Type], xx_LAI, xx_Slope, xx_Aspect, xx_Fsnow, pL_Values_Shrub,
						pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
						pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);

					yy = Linear_Interpolation_Snow_Status(Line, Loc_Line, LAI, Slope, Aspect, Fsnow, xx_LAI, xx_Slope, xx_Aspect, xx_Fsnow,  //HV using line, CLPX using Pix
						pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122, pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);
				}  //if LC
				else yy = -1.;  //water
			}   //
			else  yy = -1.;  //not snowcondition data

			if (yy < 0) a16S = -9999;
			else        a16S = int(yy * 10.0 + 0.5);  //cm to mm
			Line_Output_16S[Pix] = a16S;
		}  //Pix loop

		fwrite(Line_Output_16S, 2, PIXELS, fpt_Output);
	}  //Line loop

	fclose(fpt_LC);
	fclose(fpt_LAI);
	fclose(fpt_Fsnow);

	fclose(fpt_Output);

	delete[]pL_Values_Sedge;
	delete[]pL_Values_Shrub;

	delete[]Line_LC_8U;
	delete[]Line_LAI_16S;
	delete[]Line_Slope;
	delete[]Line_Aspect;
	delete[]Line_Fsnow;

	delete[]Line_Output_16S;

	printf("\nFinihsed! ");
}
*/

void Output_pixel_values_of_one_image()
{
	char Path_In_Mask[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow_2026July\\";
	char Path_In[] =      "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow_2026July\\";
	char Path_Out[] =     "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow_2026July\\";

	char String[200];

	int yy, Line, Pix, V;

	unsigned char* aLine_8U = new unsigned char[PIXELS];

	short* aLine_16S = new short[PIXELS];

	FILE* fpt_Out, * fpt_In_8U, * fpt_In_16S;

	//output file
	sprintf(String, "%sa.txt", Path_Out);
	if ((fpt_Out = fopen(String, "w")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}
	fprintf(fpt_Out, "Line Pix Fsnow_7yr_avg\n");

	//input files
	sprintf(String, "%sHV_study_area_exclude_water.raw", Path_In_Mask);  //
	if ((fpt_In_8U = fopen(String, "rb")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}

	sprintf(String, "%sHV_avg_Fsnow_7yr_avg.dat", Path_In);
	if ((fpt_In_16S = fopen(String, "rb")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}

	for (Line = 0; Line < LINES; Line++) {
		printf(".");
		fread(aLine_8U, 1, PIXELS, fpt_In_8U);
		fread(aLine_16S, 2, PIXELS, fpt_In_16S);

		for (Pix = 0; Pix < PIXELS; Pix++) {
			if (aLine_8U[Pix] == 1) {
				V = aLine_16S[Pix];
				fprintf(fpt_Out, "%d %d %d\n", Line, Pix, V);
			}
		}  //pix
	}  //line

	fclose(fpt_Out);
	fclose(fpt_In_8U);
	fclose(fpt_In_16S);

	delete[]aLine_8U;
	delete[]aLine_16S;
}


void Output_pixel_values_of_observed_Modelled_SnowDepth_images()
{
	char Path_In_Mask[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow\\";
	char Path_In_Ob[] = "E:\\Snow_Alaska\\HV\\HV_30m\\";
	char Path_In[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_SnowDepth_2026July\\";
	char Path_Out[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_SnowDepth_2026July\\";

	char String[200];
	int N_Files = 6;

	int Year[6] = { 2012, 2013, 2015, 2016, 2017, 2018 };
	int yy, Line, Pix, SnowDepth_Ob[6], SnowDepth_Mod[6];

	unsigned char* aLine_8U = new unsigned char[PIXELS];

	short* aLine_Ob_0 = new short[PIXELS]; 	short* aLine_Ob_1 = new short[PIXELS];
	short* aLine_Ob_2 = new short[PIXELS]; 	short* aLine_Ob_3 = new short[PIXELS];
	short* aLine_Ob_4 = new short[PIXELS]; 	short* aLine_Ob_5 = new short[PIXELS];

	short* aLine_Mod_0 = new short[PIXELS]; 	short* aLine_Mod_1 = new short[PIXELS];
	short* aLine_Mod_2 = new short[PIXELS]; 	short* aLine_Mod_3 = new short[PIXELS];
	short* aLine_Mod_4 = new short[PIXELS]; 	short* aLine_Mod_5 = new short[PIXELS];

	FILE* fpt_Out, * fpt_In_8U, * fpt_In_Ob[6], * fpt_In_Mod[6];

	//output file
	sprintf(String, "%sSnowDepth.txt", Path_Out);
	if ((fpt_Out = fopen(String, "w")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}
	fprintf(fpt_Out, "Line Pix ");
	for (yy = 0; yy < N_Files; yy++) fprintf(fpt_Out, "Ob_%d ", Year[yy]);
	for (yy = 0; yy < N_Files; yy++) fprintf(fpt_Out, "Mod_%d ", Year[yy]);
	fprintf(fpt_Out, "\n");

	//input files
	sprintf(String, "%sHV_study_area_exclude_water.raw", Path_In_Mask);  //
	if ((fpt_In_8U = fopen(String, "rb")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}

	for (yy = 0; yy < N_Files; yy++) {
		sprintf(String, "%sSnowDepth_30m_%d.raw", Path_In_Ob, Year[yy]);
		if ((fpt_In_Ob[yy] = fopen(String, "rb")) == NULL) {
			printf("Cannot open file %s\n", String);
			exit(0);
		}

		sprintf(String, "%sHV_SnowDepth_avg_Fsnow_Apr_%d.dat", Path_In, Year[yy]);
		if ((fpt_In_Mod[yy] = fopen(String, "rb")) == NULL) {
			printf("Cannot open file %s\n", String);
			exit(0);
		}
	}

	for (Line = 0; Line < LINES; Line++) {
		printf(".");
		fread(aLine_8U, 1, PIXELS, fpt_In_8U);

		fread(aLine_Ob_0, 2, PIXELS, fpt_In_Ob[0]);
		fread(aLine_Ob_1, 2, PIXELS, fpt_In_Ob[1]);
		fread(aLine_Ob_2, 2, PIXELS, fpt_In_Ob[2]);
		fread(aLine_Ob_3, 2, PIXELS, fpt_In_Ob[3]);
		fread(aLine_Ob_4, 2, PIXELS, fpt_In_Ob[4]);
		fread(aLine_Ob_5, 2, PIXELS, fpt_In_Ob[5]);

		fread(aLine_Mod_0, 2, PIXELS, fpt_In_Mod[0]);  //for 
		fread(aLine_Mod_1, 2, PIXELS, fpt_In_Mod[1]);  //for 
		fread(aLine_Mod_2, 2, PIXELS, fpt_In_Mod[2]);  //for 
		fread(aLine_Mod_3, 2, PIXELS, fpt_In_Mod[3]);  //for 
		fread(aLine_Mod_4, 2, PIXELS, fpt_In_Mod[4]);  //for 
		fread(aLine_Mod_5, 2, PIXELS, fpt_In_Mod[5]);  //for 

		for (Pix = 0; Pix < PIXELS; Pix++) {
			if (aLine_8U[Pix] == 1) {

				SnowDepth_Ob[0] = aLine_Ob_0[Pix];
				SnowDepth_Ob[1] = aLine_Ob_1[Pix];
				SnowDepth_Ob[2] = aLine_Ob_2[Pix];
				SnowDepth_Ob[3] = aLine_Ob_3[Pix];
				SnowDepth_Ob[4] = aLine_Ob_4[Pix];
				SnowDepth_Ob[5] = aLine_Ob_5[Pix];

				SnowDepth_Mod[0] = aLine_Mod_0[Pix];
				SnowDepth_Mod[1] = aLine_Mod_1[Pix];
				SnowDepth_Mod[2] = aLine_Mod_2[Pix];
				SnowDepth_Mod[3] = aLine_Mod_3[Pix];
				SnowDepth_Mod[4] = aLine_Mod_4[Pix];
				SnowDepth_Mod[5] = aLine_Mod_5[Pix];

				fprintf(fpt_Out, "%d %d ", Line, Pix);
				for (yy = 0; yy < N_Files; yy++) fprintf(fpt_Out, "%d ", SnowDepth_Ob[yy]);
				for (yy = 0; yy < N_Files; yy++) fprintf(fpt_Out, "%d ", SnowDepth_Mod[yy]);
				fprintf(fpt_Out, "\n");
			}
		}  //pix
	}  //line

	fclose(fpt_Out);
	fclose(fpt_In_8U);
	for (yy = 0; yy < N_Files; yy++) fclose(fpt_In_Ob[yy]);
	for (yy = 0; yy < N_Files; yy++) fclose(fpt_In_Mod[yy]);

	delete[]aLine_8U;
	delete[]aLine_Ob_0; delete[]aLine_Ob_1; delete[]aLine_Ob_2; delete[]aLine_Ob_3;
	delete[]aLine_Ob_4; delete[]aLine_Ob_5;

	delete[]aLine_Mod_0; delete[]aLine_Mod_1; delete[]aLine_Mod_2; delete[]aLine_Mod_3;
	delete[]aLine_Mod_4; delete[]aLine_Mod_5;
}

void Output_pixel_values_of_Fsnow_images_from_SnowDepth_for_calculating_regression()
{
	char Path_In_Mask[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow\\";
	char Path_In[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow\\";
	char Path_Out[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow\\";

	char String[200];
	int N_Files = 6;

	int Year[6] = { 2012, 2013, 2015, 2016, 2017, 2018};
	int yy, Line, Pix, Fsnow[6];

	unsigned char* aLine_8U = new unsigned char[PIXELS];

	short* aLine_16S_0 = new short[PIXELS]; 	short* aLine_16S_1 = new short[PIXELS];
	short* aLine_16S_2 = new short[PIXELS]; 	short* aLine_16S_3 = new short[PIXELS];
	short* aLine_16S_4 = new short[PIXELS]; 	short* aLine_16S_5 = new short[PIXELS];

	FILE* fpt_Out, * fpt_In_8U, * fpt_In_16S[6];

	//output file
	sprintf(String, "%sHV_Fsnow_from_SnowDepth.txt", Path_Out);
	if ((fpt_Out = fopen(String, "w")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}
	fprintf(fpt_Out, "Line Pix ");
	for (yy = 0; yy < 6; yy++) fprintf(fpt_Out, "%d ", Year[yy]);
	fprintf(fpt_Out, "\n");

	//input files
	sprintf(String, "%sHV_study_area_exclude_water.raw", Path_In_Mask);  //
	if ((fpt_In_8U = fopen(String, "rb")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}

	for (yy = 0; yy < 6; yy++) {
		sprintf(String, "%sHV_Fsnow_from_SnowDepth_%d.dat", Path_In, Year[yy]);
		if ((fpt_In_16S[yy] = fopen(String, "rb")) == NULL) {
			printf("Cannot open file %s\n", String);
			exit(0);
		}
	}

	for (Line = 0; Line < LINES; Line++) {
		printf(".");
		fread(aLine_8U, 1, PIXELS, fpt_In_8U);

		fread(aLine_16S_0, 2, PIXELS, fpt_In_16S[0]);
		fread(aLine_16S_1, 2, PIXELS, fpt_In_16S[1]);
		fread(aLine_16S_2, 2, PIXELS, fpt_In_16S[2]);
		fread(aLine_16S_3, 2, PIXELS, fpt_In_16S[3]);
		fread(aLine_16S_4, 2, PIXELS, fpt_In_16S[4]);
		fread(aLine_16S_5, 2, PIXELS, fpt_In_16S[5]);

		for (Pix = 0; Pix < PIXELS; Pix++) {
			if (aLine_8U[Pix] == 1) {
				for (yy = 0; yy < 6; yy++) Fsnow[yy] = -200;

				Fsnow[0] = aLine_16S_0[Pix];
				Fsnow[1] = aLine_16S_1[Pix];
				Fsnow[2] = aLine_16S_2[Pix];
				Fsnow[3] = aLine_16S_3[Pix];
				Fsnow[4] = aLine_16S_4[Pix];
				Fsnow[5] = aLine_16S_5[Pix];

				fprintf(fpt_Out, "%d %d ", Line, Pix);
				for (yy = 0; yy < 6; yy++) fprintf(fpt_Out, "%d ", Fsnow[yy]);
				fprintf(fpt_Out, "\n");

			}
		}  //pix
	}  //line

	fclose(fpt_Out);
	fclose(fpt_In_8U);
	for (yy = 0; yy < 6; yy++) fclose(fpt_In_16S[yy]);

	delete[]aLine_8U;
	delete[]aLine_16S_0; delete[]aLine_16S_1; delete[]aLine_16S_2; delete[]aLine_16S_3;
	delete[]aLine_16S_4; delete[]aLine_16S_5; 
}

void Linear_Regression(int N, double* x, double* y, double *Regression_V)
{
	int i;
	double sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0, sum_y2 = 0;
	double slope=0, intercept=0, R=0;

	double Fx, Fy;

	if (N > 0) { 
		for (i = 0; i < N; i++) {
			sum_x += x[i];
			sum_y += y[i];
			sum_xy += x[i] * y[i];
			sum_x2 += x[i] * x[i];
			sum_y2 += y[i] * y[i];
		}

		Fx = N * sum_x2 - sum_x * sum_x;
		Fy = N * sum_y2 - sum_y * sum_y;

		if (Fx * Fy != 0) {
			slope = (N * sum_xy - sum_x * sum_y) / Fx;
			intercept = (sum_y - slope * sum_x) / N;
			R = (N * sum_xy - sum_x * sum_y) / sqrt(Fx * Fy);
		}
		else {
			slope = 0;
			intercept = 0;
			R = 0;
		}
	}

	Regression_V[0] = R;
	Regression_V[1] = slope;
	Regression_V[2] = intercept;
}



void Output_pixel_values_of_SnowDepth_images_7_years_for_calculating_regression()
{
	char Path_In_Mask[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow\\";
	char Path_In[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_SnowDepth\\";
	char Path_Out[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_SnowDepth\\";

	char String[200];
	int N_Files = 7;

	//	int Year[6] = { 2012, 2013, 2015, 2016, 2017, 2018};
	int Year[7] = { 2019, 2020, 2021, 2022, 2023, 2024, 2025 };
	int yy, Line, Pix, SnowDepth[7];

	unsigned char* aLine_8U = new unsigned char[PIXELS];

	short* aLine_16S_0 = new short[PIXELS]; 	short* aLine_16S_1 = new short[PIXELS];
	short* aLine_16S_2 = new short[PIXELS]; 	short* aLine_16S_3 = new short[PIXELS];
	short* aLine_16S_4 = new short[PIXELS]; 	short* aLine_16S_5 = new short[PIXELS];
	short* aLine_16S_6 = new short[PIXELS];

	FILE* fpt_Out, * fpt_In_8U, * fpt_In_16S[7];

	//output file
	sprintf(String, "%sHV_SnowDepth_2019_2025.txt", Path_Out);
	if ((fpt_Out = fopen(String, "w")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}
	fprintf(fpt_Out, "Line Pix ");
	for (yy = 0; yy < 7; yy++) fprintf(fpt_Out, "%d ", Year[yy]);
	fprintf(fpt_Out, "\n");

	//input files
	sprintf(String, "%sHV_study_area_exclude_water.raw", Path_In_Mask);  //
	if ((fpt_In_8U = fopen(String, "rb")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}

	for (yy = 0; yy < 7; yy++) {
		sprintf(String, "%sHV_SnowDepth_avg_Apr_5_17_using_7yr_avg_Fsnow_%d.dat", Path_In, Year[yy]);
		if ((fpt_In_16S[yy] = fopen(String, "rb")) == NULL) {
			printf("Cannot open file %s\n", String);
			exit(0);
		}
	}

	for (Line = 0; Line < LINES; Line++) {
		printf(".");
		fread(aLine_8U, 1, PIXELS, fpt_In_8U);

		fread(aLine_16S_0, 2, PIXELS, fpt_In_16S[0]);
		fread(aLine_16S_1, 2, PIXELS, fpt_In_16S[1]);
		fread(aLine_16S_2, 2, PIXELS, fpt_In_16S[2]);
		fread(aLine_16S_3, 2, PIXELS, fpt_In_16S[3]);
		fread(aLine_16S_4, 2, PIXELS, fpt_In_16S[4]);
		fread(aLine_16S_5, 2, PIXELS, fpt_In_16S[5]);
		fread(aLine_16S_6, 2, PIXELS, fpt_In_16S[6]);

		for (Pix = 0; Pix < PIXELS; Pix++) {
			if (aLine_8U[Pix] == 1) {
				SnowDepth[0] = aLine_16S_0[Pix];
				SnowDepth[1] = aLine_16S_1[Pix];
				SnowDepth[2] = aLine_16S_2[Pix];
				SnowDepth[3] = aLine_16S_3[Pix];
				SnowDepth[4] = aLine_16S_4[Pix];
				SnowDepth[5] = aLine_16S_5[Pix];
				SnowDepth[6] = aLine_16S_6[Pix];

				fprintf(fpt_Out, "%d %d ", Line, Pix);
				for (yy = 0; yy < 7; yy++) fprintf(fpt_Out, "%d ", SnowDepth[yy]);
				fprintf(fpt_Out, "\n");

			}
		}  //pix
	}  //line

	fclose(fpt_Out);
	fclose(fpt_In_8U);
	for (yy = 0; yy < 7; yy++) fclose(fpt_In_16S[yy]);

	delete[]aLine_8U;
	delete[]aLine_16S_0; delete[]aLine_16S_1; delete[]aLine_16S_2; delete[]aLine_16S_3;
	delete[]aLine_16S_4; delete[]aLine_16S_5; delete[]aLine_16S_6;
}


void Output_pixel_values_of_Fsnow_images_for_calculating_regression()
{
	char Path_In_Mask[]   = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow_2026July\\";
	char Path_In_SnowEnd_u[] = "E:\\Snow_Alaska\\Data_From_Mitchell\\2026_YZ\\HV_SnowEndB_Layers\\";
	char Path_In[]        = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow_2026July\\";                       
	char Path_Out[]       = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow_2026July\\";

	char String[200];
	int N_Files = 7;

//	int Year[6] = { 2012, 2013, 2015, 2016, 2017, 2018};
	int Year[7] = { 2019, 2020, 2021, 2022, 2023, 2024, 2025};
	int yy, Line, Pix, Fsnow[7];

	unsigned char* aLine_8U = new unsigned char[PIXELS];

	short* aLine_16S_0 = new short[PIXELS]; 	short* aLine_16S_1 = new short[PIXELS];
	short* aLine_16S_2 = new short[PIXELS]; 	short* aLine_16S_3 = new short[PIXELS];
	short* aLine_16S_4 = new short[PIXELS]; 	short* aLine_16S_5 = new short[PIXELS];
	short* aLine_16S_6 = new short[PIXELS]; 	

	float* aLine_32R_0 = new float[PIXELS]; 	float* aLine_32R_1 = new float[PIXELS];
	float* aLine_32R_2 = new float[PIXELS]; 	float* aLine_32R_3 = new float[PIXELS];
	float* aLine_32R_4 = new float[PIXELS]; 	float* aLine_32R_5 = new float[PIXELS];
	float* aLine_32R_6 = new float[PIXELS]; 	


	FILE* fpt_Out, * fpt_In_8U, * fpt_In_16S[7], * fpt_In_32R[7];

//output file
	sprintf(String, "%sHV_Fsnow.txt", Path_Out); 
	if ((fpt_Out = fopen(String, "w")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}
	fprintf(fpt_Out, "Line Pix ");
	for (yy = 0; yy < 7; yy++) fprintf(fpt_Out, "%d ", Year[yy]);
	fprintf(fpt_Out, "\n");

//input files
	sprintf(String, "%sHV_study_area_exclude_water.raw", Path_In_Mask);  //
	if ((fpt_In_8U = fopen(String, "rb")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}

	for (yy = 0; yy < 7; yy++) {
		sprintf(String, "%sHV_avg_Fsnow_%d.dat", Path_In, Year[yy]);  
		if ((fpt_In_16S[yy] = fopen(String, "rb")) == NULL) {
			printf("Cannot open file %s\n", String);
			exit(0);
		}

		sprintf(String, "%sHV_SnowEndB_u_%d.raw", Path_In_SnowEnd_u, Year[yy]);
		if ((fpt_In_32R[yy] = fopen(String, "rb")) == NULL) {
			printf("Cannot open file %s\n", String);
			exit(0);
		}
	}

	for (Line = 0; Line < LINES; Line++) {
		printf(".");
		fread(aLine_8U, 1, PIXELS, fpt_In_8U);

		fread(aLine_16S_0, 2, PIXELS, fpt_In_16S[0]);
		fread(aLine_16S_1, 2, PIXELS, fpt_In_16S[1]);
		fread(aLine_16S_2, 2, PIXELS, fpt_In_16S[2]);
		fread(aLine_16S_3, 2, PIXELS, fpt_In_16S[3]);
		fread(aLine_16S_4, 2, PIXELS, fpt_In_16S[4]);
		fread(aLine_16S_5, 2, PIXELS, fpt_In_16S[5]);
		fread(aLine_16S_6, 2, PIXELS, fpt_In_16S[6]);

		fread(aLine_32R_0, 4, PIXELS, fpt_In_32R[0]);  //for u
		fread(aLine_32R_1, 4, PIXELS, fpt_In_32R[1]);  //for u
		fread(aLine_32R_2, 4, PIXELS, fpt_In_32R[2]);  //for u
		fread(aLine_32R_3, 4, PIXELS, fpt_In_32R[3]);  //for u
		fread(aLine_32R_4, 4, PIXELS, fpt_In_32R[4]);  //for u
		fread(aLine_32R_5, 4, PIXELS, fpt_In_32R[5]);  //for u
		fread(aLine_32R_6, 4, PIXELS, fpt_In_32R[6]);  //for u

		for (Pix = 0; Pix < PIXELS; Pix++) {
			if (aLine_8U[Pix] == 1) {
				for (yy = 0; yy < 7; yy++) Fsnow[yy] = -200;

				if (aLine_32R_0[Pix] < 5.6) Fsnow[0] = aLine_16S_0[Pix];
				if (aLine_32R_1[Pix] < 5.6) Fsnow[1] = aLine_16S_1[Pix];
				if (aLine_32R_2[Pix] < 5.6) Fsnow[2] = aLine_16S_2[Pix];
				if (aLine_32R_3[Pix] < 5.6) Fsnow[3] = aLine_16S_3[Pix];
				if (aLine_32R_4[Pix] < 5.6) Fsnow[4] = aLine_16S_4[Pix];
				if (aLine_32R_5[Pix] < 5.6) Fsnow[5] = aLine_16S_5[Pix];
				if (aLine_32R_6[Pix] < 5.6) Fsnow[6] = aLine_16S_6[Pix];

				fprintf(fpt_Out, "%d %d ", Line, Pix);
				for (yy = 0; yy < 7; yy++) fprintf(fpt_Out, "%d ", Fsnow[yy]);
				fprintf(fpt_Out, "\n");

			}
		}  //pix
	}  //line

		fclose(fpt_Out);
		fclose(fpt_In_8U);
		for (yy = 0; yy < 7; yy++) fclose(fpt_In_16S[yy]);
		for (yy = 0; yy < 7; yy++) fclose(fpt_In_32R[yy]);

	delete[]aLine_8U;
	delete[]aLine_16S_0; delete[]aLine_16S_1; delete[]aLine_16S_2; delete[]aLine_16S_3;
	delete[]aLine_16S_4; delete[]aLine_16S_5; delete[]aLine_16S_6;

	delete[]aLine_32R_0; delete[]aLine_32R_1; delete[]aLine_32R_2; delete[]aLine_32R_3;
	delete[]aLine_32R_4; delete[]aLine_32R_5; delete[]aLine_32R_6;
}



/*
void Mapping_Fsnow_HV()
{
	char Common_Path[] = "E:\\Snow_Alaska\\Modelling_2026\\";
	char Path_In_Area[] = "E:\\Snow_Alaska\\HV\\HV_30m\\";                    //_5m  for spatial input
	char Path_Out_Area[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Files\\";  //_5m_Spatial_Output
	char Clime_Site_Name[] = "HV_ERA5_Precip";

	char Path[300], File_Name[300];
	char Note[200];


	//0:Shrub, 1:Sedge
	char CoverType_Name[2][7] = { "Sedge", "Shrub" };

	int n, N, i_LAI, i_Slope, i_Aspect, i_Fsnow, N_LAI[2], N_Slope[2], N_Aspect[2], N_Fsnow[2];

	int Type, R, IF_Data;
	double LAI_Avg[2][LAI_NN], Slope_Avg[2][Slope_NN], Aspect_Avg[2][Aspect_NN], Fsnow_Avg[2][Fsnow_NN];

	int Year = 2023;
	//Using: snow depth: 2012, 2013, 2015, 2016, 2017, 2018, modify the input and scaling factor

	//Snow end date: 2019, 2020, 2021, 2022, 2023, 2024, 2025

	int Input_Variable = 0; //0 or 2. using this variable to estimate Fsnow: 0:Snowcover_day1, (1:SnowCover_day2), 2:SnowDepth 

	double ALL_Loc_Lines[2], Loc_Line[2]; //, ALL_Loc_Pixels[Total_Rows], Loc_Pix[2], Total_Sites = Total_Rows;

	double LAI, Slope, Aspect, SnowCondition;
	double xx_LAI[2], xx_Slope[2], xx_Aspect[2];

	double Loc2_Snow111[2][Fsnow_NN], Loc2_Snow112[2][Fsnow_NN], Loc2_Snow121[2][Fsnow_NN], Loc2_Snow122[2][Fsnow_NN],
		Loc2_Snow211[2][Fsnow_NN], Loc2_Snow212[2][Fsnow_NN], Loc2_Snow221[2][Fsnow_NN], Loc2_Snow222[2][Fsnow_NN];

	double* pLoc2_Snow111[2], * pLoc2_Snow112[2], * pLoc2_Snow121[2], * pLoc2_Snow122[2],
		* pLoc2_Snow211[2], * pLoc2_Snow212[2], * pLoc2_Snow221[2], * pLoc2_Snow222[2];

	float* pL_Values_Sedge = new float[Total_Rows * Sedge_LAI_NN * Sedge_Slope_NN * Sedge_Aspect_NN * Sedge_Fsnow_NN],
		* pL_Values_Shrub = new float[Total_Rows * Shrub_LAI_NN * Shrub_Slope_NN * Shrub_Aspect_NN * Shrub_Fsnow_NN];

	int Line, Pix, LC;
	double yy;

	unsigned char* Line_LC_8U = new unsigned char[PIXELS];
	short* Line_LAI_16S = new short[PIXELS], a16S;
	float* Line_Slope = new float[PIXELS];
	float* Line_Aspect = new float[PIXELS];
	short* Line_SnowCondition = new short[PIXELS];

	short* Line_Output_16S = new short[PIXELS];       //for Fsnow

	FILE* fpt_Output, * fpt_In, * fpt_LC, * fpt_LAI, * fpt_Slope, * fpt_Aspect, * fpt_SnowCondition;

	for (n = 0; n < 2; n++) {
		pLoc2_Snow111[n] = Loc2_Snow111[n];
		pLoc2_Snow112[n] = Loc2_Snow112[n];
		pLoc2_Snow121[n] = Loc2_Snow121[n];
		pLoc2_Snow122[n] = Loc2_Snow122[n];

		pLoc2_Snow211[n] = Loc2_Snow211[n];
		pLoc2_Snow212[n] = Loc2_Snow212[n];
		pLoc2_Snow221[n] = Loc2_Snow221[n];
		pLoc2_Snow222[n] = Loc2_Snow222[n];
	}

	//Lines for the climate locations
	ALL_Loc_Lines[0] = 1. - 1;
	ALL_Loc_Lines[1] = PIXELS - 1.0;  //HV: using LINES, CLPX using PIXELS
	for (R = 0; R < 2; R++) Loc_Line[R] = ALL_Loc_Lines[R];

	for (Type = 0; Type < 2; Type++) {   //2

		sprintf(Path, "%s_Model_Input_Output\\%s\\Input\\", Common_Path, CoverType_Name[Type]);
		//read LAI
		sprintf(Note, "%s%s\\%s_LAI_N.txt", Path, CoverType_Name[Type], CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_LAI[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_LAI[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_LAI, &LAI_Avg[Type][N]);
		}
		fclose(fpt_In);

		//read Slope
		sprintf(Note, "%s%s\\%s_Slope_N.txt", Path, CoverType_Name[Type], CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_Slope[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_Slope[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_Slope, &Slope_Avg[Type][N]);
		}
		fclose(fpt_In);

		//read Aspect
		sprintf(Note, "%s%s\\%s_Aspect_N.txt", Path, CoverType_Name[Type], CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_Aspect[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_Aspect[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_Aspect, &Aspect_Avg[Type][N]);
		}
		fclose(fpt_In);

		//Read Fsnow
		sprintf(Note, "%s%s\\%s_Fsnow_N.txt", Path, CoverType_Name[Type], CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_Fsnow[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_Fsnow[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_Fsnow, &Fsnow_Avg[Type][N]);
		}
		fclose(fpt_In);
	}   //type

//Input model results at L1 and L2 ---------------------------------------
	Type = 0;
	sprintf(Path, "%s\\Model_Input_Output\\%s\\From_Output_for_Modelling\\SnowDate\\", Common_Path, CoverType_Name[Type]);
	sprintf(File_Name, "%s_%s_Snow_End_date", Path, Clime_Site_Name, CoverType_Name[Type]);
	Read_Model_Results_for_Two_Sites_Fsnow(File_Name, Year, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type], pL_Values_Shrub);

	Type = 1;
	sprintf(Path, "%s\\Model_Input_Output\\%s\\From_Output_for_Modelling\\SnowDate\\", Common_Path, CoverType_Name[Type]);
	sprintf(File_Name, "%s_%s_Snow_End_date", Path, Clime_Site_Name, CoverType_Name[Type]);
	Read_Model_Results_for_Two_Sites_Fsnow(File_Name, Year, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type], pL_Values_Sedge);

	//Mapping: open image files  --------------------------------
	sprintf(Note, "%sHV_30m_Fsnow_%d.dat", Path_Out_Area, Year);  //HV_5m, HV_30m
	if ((fpt_Output = fopen(Note, "wb")) == NULL) {
		printf("Cannot open output file %s\n", Note);
		exit(0);
	}

	sprintf(Note, "%sHV_30m_Landcover.raw", Path_In_Area);  //8U, HV_5m
	if ((fpt_LC = fopen(Note, "rb")) == NULL) {
		printf("Cannot open input file %s\n", Note);
		exit(0);
	}
	sprintf(Note, "%sHV_30m_LAI.raw", Path_In_Area);  //16S, LAI*1000,  HV_5m
	if ((fpt_LAI = fopen(Note, "rb")) == NULL) {
		printf("Cannot open input file %s\n", Note);
		exit(0);
	}
	sprintf(Note, "%sHV_30m_ArcticDEM_Slope.raw", Path_In_Area);  //32R, degrees, HV_5m, 30m
	if ((fpt_Slope = fopen(Note, "rb")) == NULL) {
		printf("Cannot open input file %s\n", Note);
		exit(0);
	}
	sprintf(Note, "%sHV_30m_ArcticDEM_Aspect.raw", Path_In_Area);  //32R, degrees, HV_5m, 30m
	if ((fpt_Aspect = fopen(Note, "rb")) == NULL) {
		printf("Cannot open input file %s\n", Note);
		exit(0);
	}

	sprintf(Note, "%sWinterYear_%d_%d.raw", Path_In_Area, Year - 1, Year);  //16S, Snow end date
	if ((fpt_SnowCondition = fopen(Note, "rb")) == NULL) {
		printf("Cannot open input file %s\n", Note);
		exit(0);
	}
	printf("%d\n", Year);
	for (Line = 0; Line < LINES; Line++) {

		if ((Line % 100) == 0) printf("%d ", Line);  //printf(".", Line);

		fread(Line_LC_8U, 1, PIXELS, fpt_LC);
		fread(Line_LAI_16S, 2, PIXELS, fpt_LAI);
		fread(Line_Slope, 4, PIXELS, fpt_Slope);
		fread(Line_Aspect, 4, PIXELS, fpt_Aspect);
		fread(Line_SnowCondition, 2, PIXELS, fpt_SnowCondition);

		for (Pix = 0; Pix < PIXELS; Pix++) {
			//			printf("%d ", Pix);

			//			if (Line == 13 && Pix == 345)
			//				Pix = Pix;

			LC = Line_LC_8U[Pix];
			LAI = Line_LAI_16S[Pix] * 0.001;
			Slope = Line_Slope[Pix];
			Aspect = Line_Aspect[Pix];

			//SnowCondition = Line_SnowCondition[Pix] * 0.1;  //snow depth in cm. *0.1 to cm
			SnowCondition = Line_SnowCondition[Pix];        //snow end date

			if (SnowCondition >= 0) IF_Data = 1;
			else IF_Data = 0;

			//Landcover in HV: 11:open water, 51:Dwarf shrub, 52:Shrub/scrub, 72: Sedge, 95:herbasceous wetland

			if (IF_Data == 1) {
				if (LC == 51 || LC == 52) {   //Shrub
					Type = 0;
					Get_XY_Values_for_The_Two_Locations_Fsnow(LAI, Slope, Aspect, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type],
						LAI_Avg[Type], Slope_Avg[Type], Aspect_Avg[Type], xx_LAI, xx_Slope, xx_Aspect, pL_Values_Shrub,
						pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
						pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);

					yy = Linear_Interpolation_Fsnow(Line, Loc_Line, LAI, Slope, Aspect, SnowCondition,
						N_Fsnow[Type], Fsnow_Avg[Type], xx_LAI, xx_Slope, xx_Aspect,
						pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
						pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);
				}
				else if (LC == 72 || LC == 95) {   //Sedge
					Type = 1;

					Get_XY_Values_for_The_Two_Locations_Fsnow(LAI, Slope, Aspect, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type],
						LAI_Avg[Type], Slope_Avg[Type], Aspect_Avg[Type], xx_LAI, xx_Slope, xx_Aspect, pL_Values_Sedge,
						pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
						pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);

					yy = Linear_Interpolation_Fsnow(Line, Loc_Line, LAI, Slope, Aspect, SnowCondition,  //HV: Line, CLPX: Pix
						N_Fsnow[Type], Fsnow_Avg[Type], xx_LAI, xx_Slope, xx_Aspect,
						pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
						pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);

				}  //if LC
				else yy = 1.2;  //water
			}   //
			else  yy = 1.2;  //not snowcondition data

			if (yy < 0) a16S = int(yy * 1000.0 - 0.5);   //
			else        a16S = int(yy * 1000.0 + 0.5);
			Line_Output_16S[Pix] = a16S;
		}  //Pix loop

		fwrite(Line_Output_16S, 2, PIXELS, fpt_Output);
	}  //Line loop

	fclose(fpt_LC);
	fclose(fpt_LAI);
	fclose(fpt_SnowCondition);

	fclose(fpt_Output);

	delete[]pL_Values_Sedge;
	delete[]pL_Values_Shrub;

	delete[]Line_LC_8U;
	delete[]Line_LAI_16S;
	delete[]Line_Slope;
	delete[]Line_Aspect;
	delete[]Line_SnowCondition;

	delete[]Line_Output_16S;

	printf("\nFinihsed! ");
}
*/

void Mapping_Fsnow_HV_using_SnowDepth()
{
	char Common_Path[] = "E:\\Snow_Alaska\\Modelling_2026\\";
	char Path_In_Area[] = "E:\\Snow_Alaska\\HV\\HV_30m\\";                    //_5m  for spatial input
	char Path_Out_Area[] = "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow\\";  //_5m_Spatial_Output
	char Clime_Site_Name[] = "HV";

	char Path[300], File_Name[300];
	char Note[200];


	//0:Shrub, 1:Sedge
	char CoverType_Name[2][7] = { "Shrub", "Sedge" };

	int n, N, i_LAI, i_Slope, i_Aspect, i_Fsnow, N_LAI[2], N_Slope[2], N_Aspect[2], N_Fsnow[2];

	int Type, R, IF_Data;
	double LAI_Avg[2][LAI_NN], Slope_Avg[2][Slope_NN], Aspect_Avg[2][Aspect_NN], Fsnow_Avg[2][Fsnow_NN];

	int Year;
	int Num_Input_Values = 7;
	//Using: snow depth: 2012, 2013, 2015, 2016, 2017, 2018, modify the input and scaling factor
	//Snow end date: 2019, 2020, 2021, 2022, 2023, 2024, 2025
//	int Input_Variable = 0; //0 or 2. using this variable to estimate Fsnow: 0:Snowcover_day1, (1:SnowCover_day2), 2:SnowDepth 

	double ALL_Loc_Lines[2], Loc_Line[2]; //, ALL_Loc_Pixels[Total_Rows], Loc_Pix[2], Total_Sites = Total_Rows;

	double LAI, Slope, Aspect, SnowCondition;
	double xx_LAI[2], xx_Slope[2], xx_Aspect[2];

	double Loc2_Snow111[2][Fsnow_NN], Loc2_Snow112[2][Fsnow_NN], Loc2_Snow121[2][Fsnow_NN], Loc2_Snow122[2][Fsnow_NN],
		Loc2_Snow211[2][Fsnow_NN], Loc2_Snow212[2][Fsnow_NN], Loc2_Snow221[2][Fsnow_NN], Loc2_Snow222[2][Fsnow_NN];

	double* pLoc2_Snow111[2], * pLoc2_Snow112[2], * pLoc2_Snow121[2], * pLoc2_Snow122[2],
		* pLoc2_Snow211[2], * pLoc2_Snow212[2], * pLoc2_Snow221[2], * pLoc2_Snow222[2];

	float* pL_Values_Sedge = new float[Total_Rows * Sedge_LAI_NN * Sedge_Slope_NN * Sedge_Aspect_NN * Sedge_Fsnow_NN],
		* pL_Values_Shrub = new float[Total_Rows * Shrub_LAI_NN * Shrub_Slope_NN * Shrub_Aspect_NN * Shrub_Fsnow_NN];

	int Line, Pix, LC;
	double FF, Fsnow, Fsnow1, Fsnow2;

	unsigned char* Line_LC_8U = new unsigned char[PIXELS];
	short* Line_LAI_16S = new short[PIXELS], a16S;
	float* Line_Slope = new float[PIXELS];
	float* Line_Aspect = new float[PIXELS];
	short* Line_SnowCondition = new short[PIXELS];  //snow depth

	short* Line_Output_16S = new short[PIXELS];       //for Fsnow

	FILE* fpt_Output, * fpt_In, * fpt_LC, * fpt_LAI, * fpt_Slope, * fpt_Aspect, * fpt_SnowCondition;

	for (n = 0; n < 2; n++) {
		pLoc2_Snow111[n] = Loc2_Snow111[n];
		pLoc2_Snow112[n] = Loc2_Snow112[n];
		pLoc2_Snow121[n] = Loc2_Snow121[n];
		pLoc2_Snow122[n] = Loc2_Snow122[n];

		pLoc2_Snow211[n] = Loc2_Snow211[n];
		pLoc2_Snow212[n] = Loc2_Snow212[n];
		pLoc2_Snow221[n] = Loc2_Snow221[n];
		pLoc2_Snow222[n] = Loc2_Snow222[n];
	}

	//Lines for the climate locations
	ALL_Loc_Lines[0] = 1. - 1;
	ALL_Loc_Lines[1] = LINES - 1.0;  //HV: using LINES, CLPX using PIXELS
	for (R = 0; R < 2; R++) Loc_Line[R] = ALL_Loc_Lines[R];

	for (Type = 0; Type < 2; Type++) {   //2

		sprintf(Path, "%sModel_Input_Output\\%s\\Input\\", Common_Path, CoverType_Name[Type]);
		//read LAI
		sprintf(Note, "%s%s_LAI_N.txt", Path, CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_LAI[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_LAI[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_LAI, &LAI_Avg[Type][N]);
		}
		fclose(fpt_In);

		//read Slope
		sprintf(Note, "%s%s_Slope_N.txt", Path, CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_Slope[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_Slope[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_Slope, &Slope_Avg[Type][N]);
		}
		fclose(fpt_In);

		//read Aspect
		sprintf(Note, "%s%s_Aspect_N.txt", Path, CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_Aspect[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_Aspect[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_Aspect, &Aspect_Avg[Type][N]);
		}
		fclose(fpt_In);

		//Read Fsnow
		sprintf(Note, "%s%s_Fsnow_N.txt", Path, CoverType_Name[Type]);
		if ((fpt_In = fopen(Note, "r")) == NULL) {
			printf("Cannot open file %s\n", Note);
			exit(0);
		}
		fscanf(fpt_In, "%d", &N_Fsnow[Type]);   fgets(Note, 199, fpt_In);
		fgets(Note, 199, fpt_In);			//Site note

		for (N = 0; N < N_Fsnow[Type]; N++) {   //
			fscanf(fpt_In, "%d%lf", &i_Fsnow, &Fsnow_Avg[Type][N]);
		}
		fclose(fpt_In);
	}   //type

	sprintf(Note, "%s%s_Fsnow_from_SnowDepth_2012_2018.dat", Path_Out_Area, Clime_Site_Name);  //
	if ((fpt_Output = fopen(Note, "wb")) == NULL) {
		printf("Cannot open output file %s\n", Note);
		exit(0);
	}

	for (Year = 2012; Year <= 2018; Year++) {
		if (Year == 2014) Year = 2015;
		printf("\n%d\n", Year);

		//Input model results at L1 and L2 ---------------------------------------

		Type = 0;  //shrub
		sprintf(Path, "%sModel_Input_Output\\%s\\From_Output_for_Mapping\\SnowDepthSWE\\", Common_Path, CoverType_Name[Type]);
		sprintf(File_Name, "%s%s_%s_SnowDepth_Apr_ObYears", Path, Clime_Site_Name, CoverType_Name[Type]);
		Read_Model_Results_for_Two_Sites_Fsnow(File_Name, Num_Input_Values, Year, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type], pL_Values_Shrub);

		Type = 1;  //sedge
		sprintf(Path, "%sModel_Input_Output\\%s\\From_Output_for_Mapping\\SnowDepthSWE\\", Common_Path, CoverType_Name[Type]);
		sprintf(File_Name, "%s%s_%s_SnowDepth_Apr_ObYears", Path, Clime_Site_Name, CoverType_Name[Type]);
		Read_Model_Results_for_Two_Sites_Fsnow(File_Name, Num_Input_Values, Year, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type], pL_Values_Sedge);

		sprintf(Note, "%sHV_30m_Landcover.raw", Path_In_Area);  //8U, HV_5m
		if ((fpt_LC = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}
		sprintf(Note, "%sHV_30m_LAI.raw", Path_In_Area);  //16S, LAI*1000,  HV_5m
		if ((fpt_LAI = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}
		sprintf(Note, "%sHV_30m_ArcticDEM_Slope.raw", Path_In_Area);  //32R, degrees, HV_5m, 30m
		if ((fpt_Slope = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}
		sprintf(Note, "%sHV_30m_ArcticDEM_Aspect.raw", Path_In_Area);  //32R, degrees, HV_5m, 30m
		if ((fpt_Aspect = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}

		sprintf(Note, "%sSnowDepth_30m_%d.raw", Path_In_Area, Year);  //16S, Snow Depth
		if ((fpt_SnowCondition = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}

		//Mapping: open image files  --------------------------------
		for (Line = 0; Line < LINES; Line++) {

			if ((Line % 10) == 0) printf("%d ", Line);  //printf(".", Line);

			fread(Line_LC_8U, 1, PIXELS, fpt_LC);
			fread(Line_LAI_16S, 2, PIXELS, fpt_LAI);
			fread(Line_Slope, 4, PIXELS, fpt_Slope);
			fread(Line_Aspect, 4, PIXELS, fpt_Aspect);
			fread(Line_SnowCondition, 2, PIXELS, fpt_SnowCondition);

			for (Pix = 0; Pix < PIXELS; Pix++) {
				//			printf("%d ", Pix);

				//			if (Line == 13 && Pix == 345)
				//				Pix = Pix;

				LC = Line_LC_8U[Pix];
				LAI = Line_LAI_16S[Pix] * 0.001;
				Slope = Line_Slope[Pix];
				Aspect = Line_Aspect[Pix];

				SnowCondition = Line_SnowCondition[Pix] * 0.1;  //snow depth in cm. *0.1 to cm

				if (SnowCondition >= 0) IF_Data = 1;  //No data -32767
				else IF_Data = 0;

//  			Landcover in HV: 11:open water, 51:Dwarf shrub, 52:Shrub/scrub, 72: Sedge, 95:herbasceous wetland

//if (Year == 2020 && Line == 64 && Pix == 448)
//	Year = Year;

				if (IF_Data == 1) {
					if (LC == 51 || LC == 52) {   //Shrub
						Type = 0;

						Get_XY_Values_for_The_Two_Locations_Fsnow(LAI, Slope, Aspect, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type],
							LAI_Avg[Type], Slope_Avg[Type], Aspect_Avg[Type], xx_LAI, xx_Slope, xx_Aspect, pL_Values_Shrub,
							pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
							pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);

						FF = SnowCondition;
						Fsnow = Linear_Interpolation_Fsnow(Line, Loc_Line, LAI, Slope, Aspect, FF,
							N_Fsnow[Type], Fsnow_Avg[Type], xx_LAI, xx_Slope, xx_Aspect,
							pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
							pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);
					}
					else if (LC == 72 || LC == 95) {   //Sedge
						Type = 1;

						Get_XY_Values_for_The_Two_Locations_Fsnow(LAI, Slope, Aspect, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type],
							LAI_Avg[Type], Slope_Avg[Type], Aspect_Avg[Type], xx_LAI, xx_Slope, xx_Aspect, pL_Values_Sedge,
							pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
							pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);

						FF = SnowCondition;
						Fsnow = Linear_Interpolation_Fsnow(Line, Loc_Line, LAI, Slope, Aspect, FF,  //HV: Line, CLPX: Pix
							N_Fsnow[Type], Fsnow_Avg[Type], xx_LAI, xx_Slope, xx_Aspect,
							pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
							pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);
					}  //if LC
					else Fsnow = 1.2;  //water
				}   //
				else  Fsnow = 1.2;  //not snowcondition data

				if (Fsnow < 0) a16S = int(Fsnow * 1000.0 - 0.5);   //
				else           a16S = int(Fsnow * 1000.0 + 0.5);
				Line_Output_16S[Pix] = 1000 - a16S;
			}  //Pix loop

			fwrite(Line_Output_16S, 2, PIXELS, fpt_Output);
		}  //Line loop

		fclose(fpt_LC);
		fclose(fpt_LAI);
		fclose(fpt_SnowCondition);
	}  //year loop

	fclose(fpt_Output);

	delete[]pL_Values_Sedge;
	delete[]pL_Values_Shrub;

	delete[]Line_LC_8U;
	delete[]Line_LAI_16S;
	delete[]Line_Slope;
	delete[]Line_Aspect;
	delete[]Line_SnowCondition;

	delete[]Line_Output_16S;

	printf("\nFinihsed! ");
}


void Mapping_Fsnow_HV()
{
	char Common_Path[]  = "E:\\Snow_Alaska\\Modelling_2026\\";  
	char Path_In_Area[] = "E:\\Snow_Alaska\\HV\\HV_30m\\";                    //_5m  for spatial input
	char Path_In_SnowEnd[] = "E:\\Snow_Alaska\\Data_From_Mitchell\\2026_YZ\\HV_SnowEndB_Layers\\";
	char Path_Out_Area[]= "E:\\Snow_Alaska\\Modelling_2026\\HV\\Output_Fsnow_2026July\\";  //_5m_Spatial_Output
	char Clime_Site_Name[] = "HV";

	char Path[300], File_Name[300];
	char Note[200];


	//0:Shrub, 1:Sedge
	char CoverType_Name[2][7] = { "Shrub", "Sedge"};

	int n, N, i_LAI, i_Slope, i_Aspect, i_Fsnow, N_LAI[2], N_Slope[2], N_Aspect[2], N_Fsnow[2];

	int Type, R, IF_Data;
	double LAI_Avg[2][LAI_NN], Slope_Avg[2][Slope_NN], Aspect_Avg[2][Aspect_NN], Fsnow_Avg[2][Fsnow_NN];

	int Year;  
	int Num_Input_Values = 7;  
    //Using: snow depth: 2012, 2013, 2015, 2016, 2017, 2018, modify the input and scaling factor
    //Snow end date: 2019, 2020, 2021, 2022, 2023, 2024, 2025
//	int Input_Variable = 0; //0 or 2. using this variable to estimate Fsnow: 0:Snowcover_day1, (1:SnowCover_day2), 2:SnowDepth 

	double ALL_Loc_Lines[2], Loc_Line[2]; //, ALL_Loc_Pixels[Total_Rows], Loc_Pix[2], Total_Sites = Total_Rows;

	double LAI, Slope, Aspect, SnowCondition, SnowEnd_u;
	double xx_LAI[2], xx_Slope[2], xx_Aspect[2];

	double Loc2_Snow111[2][Fsnow_NN], Loc2_Snow112[2][Fsnow_NN], Loc2_Snow121[2][Fsnow_NN], Loc2_Snow122[2][Fsnow_NN],
		   Loc2_Snow211[2][Fsnow_NN], Loc2_Snow212[2][Fsnow_NN], Loc2_Snow221[2][Fsnow_NN], Loc2_Snow222[2][Fsnow_NN];

	double *pLoc2_Snow111[2], *pLoc2_Snow112[2], *pLoc2_Snow121[2], *pLoc2_Snow122[2],
		   *pLoc2_Snow211[2], *pLoc2_Snow212[2], *pLoc2_Snow221[2], *pLoc2_Snow222[2];

	float* pL_Values_Sedge = new float [Total_Rows* Sedge_LAI_NN * Sedge_Slope_NN * Sedge_Aspect_NN * Sedge_Fsnow_NN],
		  * pL_Values_Shrub = new float [Total_Rows*Shrub_LAI_NN * Shrub_Slope_NN * Shrub_Aspect_NN * Shrub_Fsnow_NN];

	int Line, Pix, LC;
	double FF, Fsnow, Fsnow1, Fsnow2;

	unsigned char* Line_LC_8U = new unsigned char[PIXELS];
	short* Line_LAI_16S = new short[PIXELS], a16S;
	float* Line_Slope  = new float[PIXELS];
	float* Line_Aspect = new float[PIXELS];
	float* Line_SnowCondition = new float[PIXELS];
	float* Line_SnowEnd_u = new float[PIXELS];

	short* Line_Output_16S = new short[PIXELS];       //for Fsnow

	FILE* fpt_Output, * fpt_In, * fpt_LC, * fpt_LAI, *fpt_Slope, *fpt_Aspect, * fpt_SnowCondition, *fpt_SnowEnd_u;

	for (n = 0; n < 2; n++) {
		pLoc2_Snow111[n] = Loc2_Snow111[n];
		pLoc2_Snow112[n] = Loc2_Snow112[n];
		pLoc2_Snow121[n] = Loc2_Snow121[n];
		pLoc2_Snow122[n] = Loc2_Snow122[n];

		pLoc2_Snow211[n] = Loc2_Snow211[n];
		pLoc2_Snow212[n] = Loc2_Snow212[n];
		pLoc2_Snow221[n] = Loc2_Snow221[n];
		pLoc2_Snow222[n] = Loc2_Snow222[n];
	}

	//Lines for the climate locations
	ALL_Loc_Lines[0] = 1. -1;  
	ALL_Loc_Lines[1] = LINES - 1.0;  //HV: using LINES, CLPX using PIXELS
	for (R = 0; R < 2; R++) Loc_Line[R] = ALL_Loc_Lines[R];

		for (Type = 0; Type < 2; Type++) {   //2

			sprintf(Path, "%sModel_Input_Output\\%s\\Input\\", Common_Path, CoverType_Name[Type]);
			//read LAI
			sprintf(Note, "%s%s_LAI_N.txt", Path, CoverType_Name[Type]);
			if ((fpt_In = fopen(Note, "r")) == NULL) {
				printf("Cannot open file %s\n", Note);
				exit(0);
			}
			fscanf(fpt_In, "%d", &N_LAI[Type]);   fgets(Note, 199, fpt_In);
			fgets(Note, 199, fpt_In);			//Site note

			for (N = 0; N < N_LAI[Type]; N++) {   //
				fscanf(fpt_In, "%d%lf", &i_LAI, &LAI_Avg[Type][N]);
			}
			fclose(fpt_In);

			//read Slope
			sprintf(Note, "%s%s_Slope_N.txt", Path, CoverType_Name[Type]);
			if ((fpt_In = fopen(Note, "r")) == NULL) {
				printf("Cannot open file %s\n", Note);
				exit(0);
			}
			fscanf(fpt_In, "%d", &N_Slope[Type]);   fgets(Note, 199, fpt_In);
			fgets(Note, 199, fpt_In);			//Site note

			for (N = 0; N < N_Slope[Type]; N++) {   //
				fscanf(fpt_In, "%d%lf", &i_Slope, &Slope_Avg[Type][N]);
			}
			fclose(fpt_In);

			//read Aspect
			sprintf(Note, "%s%s_Aspect_N.txt", Path, CoverType_Name[Type]);
			if ((fpt_In = fopen(Note, "r")) == NULL) {
				printf("Cannot open file %s\n", Note);
				exit(0);
			}
			fscanf(fpt_In, "%d", &N_Aspect[Type]);   fgets(Note, 199, fpt_In);
			fgets(Note, 199, fpt_In);			//Site note

			for (N = 0; N < N_Aspect[Type]; N++) {   //
				fscanf(fpt_In, "%d%lf", &i_Aspect, &Aspect_Avg[Type][N]);
			}
			fclose(fpt_In);

			//Read Fsnow
			sprintf(Note, "%s%s_Fsnow_N.txt", Path, CoverType_Name[Type]);
			if ((fpt_In = fopen(Note, "r")) == NULL) {
				printf("Cannot open file %s\n", Note);
				exit(0);
			}
			fscanf(fpt_In, "%d", &N_Fsnow[Type]);   fgets(Note, 199, fpt_In);
			fgets(Note, 199, fpt_In);			//Site note

			for (N = 0; N < N_Fsnow[Type]; N++) {   //
				fscanf(fpt_In, "%d%lf", &i_Fsnow, &Fsnow_Avg[Type][N]);
			}
			fclose(fpt_In);
		}   //type

		sprintf(Note, "%s%s_avg_Fsnow_2019_2025.dat", Path_Out_Area, Clime_Site_Name);  //
		if ((fpt_Output = fopen(Note, "wb")) == NULL) {
			printf("Cannot open output file %s\n", Note);
			exit(0);
		}

	for (Year = 2019; Year <= 2025; Year++) {
			printf("\n%d\n", Year);

	//Input model results at L1 and L2 ---------------------------------------
//need manual modify the file names (move "Top" and "Bot" to the end of the file names
		Type = 0;  //shrub
		sprintf(Path, "%sModel_Input_Output\\%s\\From_Output_for_Mapping\\SnowDate\\", Common_Path, CoverType_Name[Type]);
		sprintf(File_Name, "%s%s_%s_EndDate_Mono_July7", Path, Clime_Site_Name, CoverType_Name[Type]);
		Read_Model_Results_for_Two_Sites_Fsnow(File_Name, Num_Input_Values, Year, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type], pL_Values_Shrub);

		Type = 1;  //sedge
		sprintf(Path, "%sModel_Input_Output\\%s\\From_Output_for_Mapping\\SnowDate\\", Common_Path, CoverType_Name[Type]);
		sprintf(File_Name, "%s%s_%s_EndDate_Mono_July7", Path, Clime_Site_Name, CoverType_Name[Type]);
		Read_Model_Results_for_Two_Sites_Fsnow(File_Name, Num_Input_Values, Year, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type], pL_Values_Sedge);



		sprintf(Note, "%sHV_30m_Landcover.raw", Path_In_Area);  //8U, HV_5m
		if ((fpt_LC = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}
		sprintf(Note, "%sHV_30m_LAI.raw", Path_In_Area);  //16S, LAI*1000,  HV_5m
		if ((fpt_LAI = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}
		sprintf(Note, "%sHV_30m_ArcticDEM_Slope.raw", Path_In_Area);  //32R, degrees, HV_5m, 30m
		if ((fpt_Slope = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}
		sprintf(Note, "%sHV_30m_ArcticDEM_Aspect.raw", Path_In_Area);  //32R, degrees, HV_5m, 30m
		if ((fpt_Aspect = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}

		sprintf(Note, "%sHV_SnowEndB_%d.raw", Path_In_SnowEnd, Year);  //32f, Snow endB date
		if ((fpt_SnowCondition = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}

		sprintf(Note, "%sHV_SnowEndB_u_%d.raw", Path_In_SnowEnd, Year);  //32f, Snow end u
		if ((fpt_SnowEnd_u = fopen(Note, "rb")) == NULL) {
			printf("Cannot open input file %s\n", Note);
			exit(0);
		}

		//Mapping: open image files  --------------------------------
		for (Line = 0; Line < LINES; Line++) {

			if ((Line % 10) == 0) printf("%d ", Line);  //printf(".", Line);

			fread(Line_LC_8U, 1, PIXELS, fpt_LC);
			fread(Line_LAI_16S, 2, PIXELS, fpt_LAI);
			fread(Line_Slope, 4, PIXELS, fpt_Slope);
			fread(Line_Aspect, 4, PIXELS, fpt_Aspect);
			fread(Line_SnowCondition, 4, PIXELS, fpt_SnowCondition);
			fread(Line_SnowEnd_u, 4, PIXELS, fpt_SnowEnd_u);

			for (Pix = 0; Pix < PIXELS; Pix++) {
				//			printf("%d ", Pix);

				//			if (Line == 13 && Pix == 345)
				//				Pix = Pix;

				LC = Line_LC_8U[Pix];
				LAI = Line_LAI_16S[Pix] * 0.001;
				Slope = Line_Slope[Pix];
				Aspect = Line_Aspect[Pix];

				//			SnowCondition = Line_SnowCondition[Pix] * 0.1;  //snow depth in cm. *0.1 to cm
				SnowCondition = Line_SnowCondition[Pix];        //snow end date
				SnowEnd_u = Line_SnowEnd_u[Pix];

				if (SnowCondition >= 0) IF_Data = 1;  //No data -32767
				else IF_Data = 0;

//			Landcover in HV: 11:open water, 51:Dwarf shrub, 52:Shrub/scrub, 72: Sedge, 95:herbasceous wetland

//if (Year == 2020 && Line == 64 && Pix == 448)
//	Year = Year;


				if (IF_Data == 1) {
					if (LC == 51 || LC == 52) {   //Shrub
						Type = 0;

						Get_XY_Values_for_The_Two_Locations_Fsnow(LAI, Slope, Aspect, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type],
							LAI_Avg[Type], Slope_Avg[Type], Aspect_Avg[Type], xx_LAI, xx_Slope, xx_Aspect, pL_Values_Shrub,
							pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
							pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);

						FF = SnowCondition - SnowEnd_u;
						Fsnow1 = Linear_Interpolation_Fsnow(Line, Loc_Line, LAI, Slope, Aspect, FF,
							N_Fsnow[Type], Fsnow_Avg[Type], xx_LAI, xx_Slope, xx_Aspect,
							pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
							pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);

						FF = SnowCondition + SnowEnd_u;
						Fsnow2 = Linear_Interpolation_Fsnow(Line, Loc_Line, LAI, Slope, Aspect, FF,
							N_Fsnow[Type], Fsnow_Avg[Type], xx_LAI, xx_Slope, xx_Aspect,
							pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
							pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);

						Fsnow = (Fsnow1 + Fsnow2) / 2.0;

//using mid SnowEnd
//						FF = SnowCondition;
//						Fsnow = Linear_Interpolation_Fsnow(Line, Loc_Line, LAI, Slope, Aspect, FF,
//							N_Fsnow[Type], Fsnow_Avg[Type], xx_LAI, xx_Slope, xx_Aspect,
//							pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
//							pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);
					}
					else if (LC == 72 || LC == 95) {   //Sedge
						Type = 1;

						Get_XY_Values_for_The_Two_Locations_Fsnow(LAI, Slope, Aspect, N_LAI[Type], N_Slope[Type], N_Aspect[Type], N_Fsnow[Type],
							LAI_Avg[Type], Slope_Avg[Type], Aspect_Avg[Type], xx_LAI, xx_Slope, xx_Aspect, pL_Values_Sedge,
							pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
							pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);

						FF = SnowCondition - SnowEnd_u;
						Fsnow1 = Linear_Interpolation_Fsnow(Line, Loc_Line, LAI, Slope, Aspect, FF,  //HV: Line, CLPX: Pix
							N_Fsnow[Type], Fsnow_Avg[Type], xx_LAI, xx_Slope, xx_Aspect,
							pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
							pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);

						FF = SnowCondition + SnowEnd_u;
						Fsnow2 = Linear_Interpolation_Fsnow(Line, Loc_Line, LAI, Slope, Aspect, FF,  //HV: Line, CLPX: Pix
							N_Fsnow[Type], Fsnow_Avg[Type], xx_LAI, xx_Slope, xx_Aspect,
							pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
							pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);

						Fsnow = (Fsnow1 + Fsnow2) / 2.0;

//using Mid SnowEnd
//					FF = SnowCondition;
//					Fsnow = Linear_Interpolation_Fsnow(Line, Loc_Line, LAI, Slope, Aspect, FF,  //HV: Line, CLPX: Pix
//						N_Fsnow[Type], Fsnow_Avg[Type], xx_LAI, xx_Slope, xx_Aspect,
//						pLoc2_Snow111, pLoc2_Snow112, pLoc2_Snow121, pLoc2_Snow122,
//						pLoc2_Snow211, pLoc2_Snow212, pLoc2_Snow221, pLoc2_Snow222);
					}  //if LC
					else Fsnow = 1.2;  //water
				}   //
				else  Fsnow = 1.2;  //not snowcondition data

				if (Fsnow < 0) a16S = int(Fsnow * 1000.0 - 0.5);   //
				else           a16S = int(Fsnow * 1000.0 + 0.5);
				Line_Output_16S[Pix] = 1000 - a16S;
			}  //Pix loop

			fwrite(Line_Output_16S, 2, PIXELS, fpt_Output);
		}  //Line loop

		fclose(fpt_LC);
		fclose(fpt_LAI);
		fclose(fpt_SnowCondition);
		fclose(fpt_SnowEnd_u);

	}  //year loop

	fclose(fpt_Output);

	delete[]pL_Values_Sedge;
	delete []pL_Values_Shrub;

	delete[]Line_LC_8U;
	delete[]Line_LAI_16S;
	delete[]Line_Slope;
	delete[]Line_Aspect;
	delete[]Line_SnowCondition;
	delete[]Line_SnowEnd_u;

	delete[]Line_Output_16S;

	printf("\nFinihsed! ");
}

void Get_XY_Values_for_The_Two_Locations_Snow_Status(double LAI, double Slope, double Aspect, double Fsnow, 
	int N_LAI, int N_Slope, int N_Aspect, int N_Fsnow, double* LAI_Avg, double* Slope_Avg, double* Aspect_Avg, double* Fsnow_Avg, 
	double* xx_LAI, double* xx_Slope, double* xx_Aspect, double *xx_Fsnow, float* L_Values,
	double** Loc2_Snow111, double** Loc2_Snow112, double** Loc2_Snow121, double** Loc2_Snow122,
	double** Loc2_Snow211, double** Loc2_Snow212, double** Loc2_Snow221, double** Loc2_Snow222)
{
	int i_LAI, i_LAI1, i_LAI2, i_Slope, i_Slope1, i_Slope2, i_Aspect, i_Aspect1, i_Aspect2, i_Fsnow, i_Fsnow1, i_Fsnow2;
	int N_LAI_1 = N_LAI - 1, N_Slope_1 = N_Slope - 1, N_Aspect_1 = N_Aspect - 1, N_Fsnow_1 = N_Fsnow - 1;
	int Site;
	int i, i1, i2;

	//Find xx1 and xx2 for LAI
	if (LAI <= LAI_Avg[0]) {
		i_LAI1 = 0;
		i_LAI2 = 1;
	}
	else if (LAI >= LAI_Avg[N_LAI_1]) {
		LAI = LAI_Avg[N_LAI_1];   //limit it
		i_LAI1 = N_LAI_1 - 1;
		i_LAI2 = N_LAI_1;
	}
	else {
		for (i_LAI = 1; i_LAI < N_LAI; i_LAI++) {
			if (LAI <= LAI_Avg[i_LAI]) {
				i_LAI1 = i_LAI - 1;
				i_LAI2 = i_LAI;
				break;
			}
		}
	}
	xx_LAI[0] = LAI_Avg[i_LAI1];
	xx_LAI[1] = LAI_Avg[i_LAI2];

	//Find xx1 and xx2 for Slope
	if (Slope <= Slope_Avg[0]) {
		i_Slope1 = 0;
		i_Slope2 = 1;
	}
	else if (Slope >= Slope_Avg[N_Slope_1]) {
		Slope = Slope_Avg[N_Slope_1];   //limit it
		i_Slope1 = N_Slope_1 - 1;
		i_Slope2 = N_Slope_1;
	}
	else {
		for (i_Slope = 1; i_Slope < N_Slope; i_Slope++) {
			if (Slope <= Slope_Avg[i_Slope]) {
				i_Slope1 = i_Slope - 1;
				i_Slope2 = i_Slope;
				break;
			}
		}
	}
	xx_Slope[0] = Slope_Avg[i_Slope1];
	xx_Slope[1] = Slope_Avg[i_Slope2];

	//Find xx1 and xx2 for Aspect
	if (Aspect >= Aspect_Avg[N_Aspect - 1]) {
		i_Aspect1 = N_Aspect - 1;
		i_Aspect2 = N_Aspect;

		xx_Aspect[0] = Aspect_Avg[i_Aspect1];
		xx_Aspect[1] = 360.0;
	}
	else {
		for (i_Aspect = 1; i_Aspect < N_Aspect; i_Aspect++) {
			if (Aspect <= Aspect_Avg[i_Aspect]) {
				i_Aspect1 = i_Aspect - 1;
				i_Aspect2 = i_Aspect;
				break;
			}
		}
		xx_Aspect[0] = Aspect_Avg[i_Aspect1];
		xx_Aspect[1] = Aspect_Avg[i_Aspect2];
	}

	//Find xx1 and xx2 for Fsnow
	if (Fsnow >= Fsnow_Avg[0]) {
		i_Fsnow1 = 0;
		i_Fsnow2 = 1;
	}
	else if (Fsnow <= Fsnow_Avg[N_Fsnow_1]) {
		Fsnow = Fsnow_Avg[N_Fsnow_1];   //limit it
		i_Fsnow1 = N_Fsnow_1 - 1;
		i_Fsnow2 = N_Fsnow_1;
	}
	else {
		for (i_Fsnow = 1; i_Fsnow < N_Fsnow; i_Fsnow++) {
			if (Fsnow >= Fsnow_Avg[i_Fsnow]) {
				i_Fsnow1 = i_Fsnow - 1;
				i_Fsnow2 = i_Fsnow;
				break;
			}
		}
	}
	xx_Fsnow[0] = Fsnow_Avg[i_Fsnow1];
	xx_Fsnow[1] = Fsnow_Avg[i_Fsnow2];

//find and return the modelled corresponding snow conditions pLoc2_Snow111[0 or 1][0 to N_Fsnow -1]
	int NN;
	NN = 0;
	for (Site = 0; Site < Total_Rows; Site++) {
		i1 = NN + (i_LAI1 * N_Slope * N_Aspect + i_Slope1 * N_Aspect + i_Aspect1) * N_Fsnow;  //1,1,1
		i = i1 + i_Fsnow1;
		Loc2_Snow111[Site][0] = L_Values[i];
		Loc2_Snow111[Site][1] = L_Values[i+1];

		i1 = NN + (i_LAI1 * N_Slope * N_Aspect + i_Slope1 * N_Aspect + i_Aspect2) * N_Fsnow; //1,1,2
		i = i1 + i_Fsnow1;
	    Loc2_Snow112[Site][0] = L_Values[i];
		Loc2_Snow112[Site][1] = L_Values[i+1];

		i1 = NN + (i_LAI1 * N_Slope * N_Aspect + i_Slope2 * N_Aspect + i_Aspect1) * N_Fsnow;  //1,2,1
		i = i1 + i_Fsnow1;
	    Loc2_Snow121[Site][0] = L_Values[i];
		Loc2_Snow121[Site][1] = L_Values[i+1];

		i1 = NN + (i_LAI1 * N_Slope * N_Aspect + i_Slope2 * N_Aspect + i_Aspect2) * N_Fsnow; //1,2,2
		i = i1 + i_Fsnow1;
		Loc2_Snow122[Site][0] = L_Values[i];
		Loc2_Snow122[Site][1] = L_Values[i + 1];

		i1 = NN + (i_LAI2 * N_Slope * N_Aspect + i_Slope1 * N_Aspect + i_Aspect1) * N_Fsnow; //2,1,1
		i = i1 + i_Fsnow1;
		Loc2_Snow211[Site][0] = L_Values[i];
		Loc2_Snow211[Site][1] = L_Values[i + 1];

		i1 = NN + (i_LAI2 * N_Slope * N_Aspect + i_Slope1 * N_Aspect + i_Aspect2) * N_Fsnow; //2,1,2
		i = i1 + i_Fsnow1;
		Loc2_Snow212[Site][0] = L_Values[i];
		Loc2_Snow212[Site][1] = L_Values[i + 1];

		i1 = NN + (i_LAI2 * N_Slope * N_Aspect + i_Slope2 * N_Aspect + i_Aspect1) * N_Fsnow; //2,2,1
		i = i1 + i_Fsnow1;
		Loc2_Snow221[Site][0] = L_Values[i];
		Loc2_Snow221[Site][1] = L_Values[i + 1];

		i1 = NN + (i_LAI2 * N_Slope * N_Aspect + i_Slope2 * N_Aspect + i_Aspect2) * N_Fsnow;  //2,2,2
		i = i1 + i_Fsnow1;
		Loc2_Snow222[Site][0] = L_Values[i];
		Loc2_Snow222[Site][1] = L_Values[i + 1];

		NN += N_LAI * N_Slope * N_Aspect * N_Fsnow;
	}  //Site
}

	
void Get_XY_Values_for_The_Two_Locations_Fsnow(double LAI, double Slope, double Aspect, int N_LAI, int N_Slope, int N_Aspect, int N_Fsnow,
			double *LAI_Avg, double* Slope_Avg, double* Aspect_Avg,  double* xx_LAI, double* xx_Slope, double* xx_Aspect, float* L_Values, 
		    double** Loc2_Snow111, double** Loc2_Snow112, double** Loc2_Snow121, double** Loc2_Snow122,
   		    double** Loc2_Snow211, double** Loc2_Snow212, double** Loc2_Snow221, double** Loc2_Snow222)
{ 
		int i_LAI, i_LAI1, i_LAI2, i_Slope, i_Slope1, i_Slope2, i_Aspect, i_Aspect1, i_Aspect2;
		int N_LAI_1 = N_LAI - 1, N_Slope_1 = N_Slope - 1, N_Aspect_1 = N_Aspect - 1;
		int Site;
		int i, i1, i2;

		//Find xx1 and xx2 for LAI
		if (LAI <= LAI_Avg[0]) {
			i_LAI1 = 0;
			i_LAI2 = 1;
		}
		else if (LAI >= LAI_Avg[N_LAI_1]) {
			LAI = LAI_Avg[N_LAI_1];   //limit it
			i_LAI1 = N_LAI_1 - 1;
			i_LAI2 = N_LAI_1;
		}
		else {
			for (i_LAI = 1; i_LAI < N_LAI; i_LAI++) {
				if (LAI <= LAI_Avg[i_LAI]) {
					i_LAI1 = i_LAI - 1;
					i_LAI2 = i_LAI;
					break;
				}
			}
		}
		xx_LAI[0] = LAI_Avg[i_LAI1];
		xx_LAI[1] = LAI_Avg[i_LAI2];

		//Find xx1 and xx2 for Slope
		if (Slope <= Slope_Avg[0]) {
			i_Slope1 = 0;
			i_Slope2 = 1;
		}
		else if (Slope >= Slope_Avg[N_Slope_1]) {
			Slope = Slope_Avg[N_Slope_1];   //limit it
			i_Slope1 = N_Slope_1 - 1;
			i_Slope2 = N_Slope_1;
		}
		else {
			for (i_Slope = 1; i_Slope < N_Slope; i_Slope++) {
				if (Slope <= Slope_Avg[i_Slope]) {
					i_Slope1 = i_Slope - 1;
					i_Slope2 = i_Slope;
					break;
				}
			}
		}
		xx_Slope[0] = Slope_Avg[i_Slope1];
		xx_Slope[1] = Slope_Avg[i_Slope2];

		//Find xx1 and xx2 for Aspect
		if (Aspect >= Aspect_Avg[N_Aspect-1]) {
			i_Aspect1 = N_Aspect - 1;
			i_Aspect2 = N_Aspect;

			xx_Aspect[0] = Aspect_Avg[i_Aspect1];
			xx_Aspect[1] = 360.0;
		}
		else {
			for (i_Aspect = 1; i_Aspect < N_Aspect; i_Aspect++) {
				if (Aspect <= Aspect_Avg[i_Aspect]) {
					i_Aspect1 = i_Aspect - 1;
					i_Aspect2 = i_Aspect;
					break;
				}
			}
			xx_Aspect[0] = Aspect_Avg[i_Aspect1];
			xx_Aspect[1] = Aspect_Avg[i_Aspect2];
		}

//find and return the modelled corresponding snow conditions at N_Fsnow values: pLoc2_Snow111[0 or 1][0 to N_Fsnow -1]
		int NN;
		NN = 0;
		for (Site = 0; Site < Total_Rows; Site++) {
			i1 = NN + (i_LAI1 * N_Slope * N_Aspect + i_Slope1 * N_Aspect + i_Aspect1) * N_Fsnow;  //1,1,1
			i2 = i1 + N_Fsnow;  
			for (i = i1; i < i2; i++) Loc2_Snow111[Site][i - i1] = L_Values[i];

			i1 = NN + (i_LAI1 * N_Slope * N_Aspect + i_Slope1 * N_Aspect + i_Aspect2) * N_Fsnow; //1,1,2
			i2 = i1 + N_Fsnow;   
			for (i = i1; i < i2; i++) Loc2_Snow112[Site][i - i1] = L_Values[i];

			i1 = NN + (i_LAI1 * N_Slope * N_Aspect + i_Slope2 * N_Aspect + i_Aspect1) * N_Fsnow;  //1,2,1
			i2 = i1 + N_Fsnow;  
			for (i = i1; i < i2; i++) Loc2_Snow121[Site][i - i1] = L_Values[i];

			i1 = NN + (i_LAI1 * N_Slope * N_Aspect + i_Slope2 * N_Aspect + i_Aspect2) * N_Fsnow; //1,2,2
			i2 = i1 + N_Fsnow;   
			for (i = i1; i < i2; i++) Loc2_Snow122[Site][i - i1] = L_Values[i];

			i1 = NN + (i_LAI2 * N_Slope * N_Aspect + i_Slope1 * N_Aspect + i_Aspect1) * N_Fsnow; //2,1,1
			i2 = i1 + N_Fsnow;   
			for (i = i1; i < i2; i++) Loc2_Snow211[Site][i - i1] = L_Values[i];

			i1 = NN + (i_LAI2 * N_Slope * N_Aspect + i_Slope1 * N_Aspect + i_Aspect2) * N_Fsnow; //2,1,2
			i2 = i1 + N_Fsnow;   
			for (i = i1; i < i2; i++) Loc2_Snow212[Site][i - i1] = L_Values[i];

			i1 = NN + (i_LAI2 * N_Slope * N_Aspect + i_Slope2 * N_Aspect + i_Aspect1) * N_Fsnow; //2,2,1
			i2 = i1 + N_Fsnow;   
			for (i = i1; i < i2; i++) Loc2_Snow221[Site][i - i1] = L_Values[i];

			i1 = NN + (i_LAI2 * N_Slope * N_Aspect + i_Slope2 * N_Aspect + i_Aspect2) * N_Fsnow;  //2,2,2
			i2 = i1 + N_Fsnow;  
			for (i = i1; i < i2; i++) Loc2_Snow222[Site][i - i1] = L_Values[i];

			NN += N_LAI * N_Slope * N_Aspect * N_Fsnow;

		}  //Site
}

void Read_Model_Results_for_Two_Sites_Fsnow(char* File_Name, int Num_Input_Values, int Year, int N_LAI, int N_Slope, int N_Aspect, int N_Fsnow, float* Lx_Values)
{
	char Input_File[300], Note[200];
	char Site_Name[2][10] = { "Top", "Bot" };
	int i_LAI, i_Slope, i_Aspect, i_Fsnow, i, k, n, N, Site;

	double LAI, Slope, Aspect, Fsnow;
	float Variables[YEARS_Fsnow];

//	int Output_Variable = Year - 2019;  //for calculating Fsnow, the first year is 2019
	int Output_Variable = Year - 2012;  //for calculating snow depth in April in a Ob year, the first year is 2012

	FILE* fpt_In;

	N = 0;
	for (Site = 0; Site < Total_Rows; Site++) {

		sprintf(Input_File, "%s_%s.txt", File_Name, Site_Name[Site]); //two sites HV

		if ((fpt_In = fopen(Input_File, "r")) == NULL) {
			printf("Cannot open the input file %s\n", Input_File);
			exit(0);
		}
		fgets(Note, 199, fpt_In);			//first line

		n = 0;
		for (i_LAI = 0; i_LAI < N_LAI; i_LAI++) {           //N_LAI
			for (i_Slope = 0; i_Slope < N_Slope; i_Slope++) {   //N_Slope
				for (i_Aspect = 0; i_Aspect < N_Aspect; i_Aspect++) {   //N_Aspect
					for (i_Fsnow = 0; i_Fsnow < N_Fsnow; i_Fsnow++) {   //N_Fsnow

						fscanf(fpt_In, "%d%lf%lf%lf%lf", &i, &LAI, &Slope, &Aspect, &Fsnow);
						for (k = 0; k < Num_Input_Values; k++) fscanf(fpt_In, "%f", &Variables[k]);

						if (i != n + 1) {
							printf("%s: Input data mismatch at line %d\n", Input_File, n + 1);
							exit(0);
						}
						else  	Lx_Values[N] = Variables[Output_Variable];
						n++;
						N++;
					}
				}
			}
		}
		fclose(fpt_In);
	}  //Site
}


double Linear_Interpolation_Snow_Status(int Line, double* Loc_Line, double LAI, double Slope, double Aspect, double Fsnow,
			double* xx_LAI, double* xx_Slope, double* xx_Aspect, double *xx_Fsnow,
			double** Loc2_Snow111, double** Loc2_Snow112, double** Loc2_Snow121, double** Loc2_Snow122,
			double** Loc2_Snow211, double** Loc2_Snow212, double** Loc2_Snow221, double** Loc2_Snow222)
{
	int R, i;

	double	dLAI, dSlope, dAspect, dFsnow, dLine;
	double Snow_11[2][Fsnow_NN], Snow_12[2][Fsnow_NN], Snow_21[2][Fsnow_NN], Snow_22[2][Fsnow_NN],
		Snow_1[2][Fsnow_NN], Snow_2[2][Fsnow_NN], Snow[2][Fsnow_NN], Snow_x[Fsnow_NN];
	double F, Snow_Status;

	// interpolation
	dLine = (Line - Loc_Line[0]) / (Loc_Line[1] - Loc_Line[0]);

	dLAI = (LAI - xx_LAI[0]) / (xx_LAI[1] - xx_LAI[0]);
	if (dLAI < 0.) dLAI = 0.;
	else if (dLAI > 1.) dLAI = 1.;

	dSlope = (Slope - xx_Slope[0]) / (xx_Slope[1] - xx_Slope[0]);
	if (dSlope < 0.) dSlope = 0.;
	else if (dSlope > 1.) dSlope = 1.;

	dAspect = (Aspect - xx_Aspect[0]) / (xx_Aspect[1] - xx_Aspect[0]);
	if (dAspect < 0.) dAspect = 0.;
	else if (dAspect > 1.) dAspect = 1.;

	dFsnow = (Fsnow - xx_Fsnow[0]) / (xx_Fsnow[1] - xx_Fsnow[0]);
//	if (dFsnow < 0.) dFsnow = 0.;   //allowing extropolation
//	else if (dFsnow > 1.) dFsnow = 1.;


	for (R = 0; R < Total_Rows; R++) {  //2 climate locations, 
		for (i = 0; i < 2; i++) {  //for two Fsnow
			//LAI
			Snow_11[R][i] = Loc2_Snow111[R][i] * (1. - dLAI) + Loc2_Snow211[R][i] * dLAI;
			Snow_12[R][i] = Loc2_Snow112[R][i] * (1. - dLAI) + Loc2_Snow212[R][i] * dLAI;
			Snow_21[R][i] = Loc2_Snow121[R][i] * (1. - dLAI) + Loc2_Snow221[R][i] * dLAI;
			Snow_22[R][i] = Loc2_Snow122[R][i] * (1. - dLAI) + Loc2_Snow222[R][i] * dLAI;
			//Slope
			Snow_1[R][i] = Snow_11[R][i] * (1. - dSlope) + Snow_21[R][i] * dSlope;
			Snow_2[R][i] = Snow_12[R][i] * (1. - dSlope) + Snow_22[R][i] * dSlope;
			//Aspect
			Snow[R][i] = Snow_1[R][i] * (1. - dAspect) + Snow_2[R][i] * dAspect;
		}
		Snow_x[R] = Snow[R][0] * (1. - dFsnow) + Snow[R][1] * dFsnow;

		if (Snow_x[R] < 0) Snow_x[R] = 0;

	}

	if (Total_Rows == 1) Snow_Status = Snow_x[0];
	else Snow_Status = Snow_x[0] * (1. - dLine) + Snow_x[1] * dLine;

	return Snow_Status;
}


double Linear_Interpolation_Fsnow(int Line, double* Loc_Line, double LAI, double Slope, double Aspect, double SnowCondition,
		    int N_Fsnow, double* Fsnow_Avg, double* xx_LAI, double* xx_Slope, double* xx_Aspect, 
			double** Loc2_Snow111, double** Loc2_Snow112, double** Loc2_Snow121, double** Loc2_Snow122,
			double** Loc2_Snow211, double** Loc2_Snow212, double** Loc2_Snow221, double** Loc2_Snow222)
{
		int R, i;
		int N_Fsnow_1 = N_Fsnow - 1, N_Fsnow_2 = N_Fsnow - 2;

		double	dLAI, dSlope, dAspect, dLine;
		double Snow_11[2][Fsnow_NN], Snow_12[2][Fsnow_NN], Snow_21[2][Fsnow_NN], Snow_22[2][Fsnow_NN],
			Snow_1[2][Fsnow_NN], Snow_2[2][Fsnow_NN], Snow[2][Fsnow_NN], Snow_x[Fsnow_NN];
		double F, Fsnow;

		// interpolation
		dLine = (Line - Loc_Line[0]) / (Loc_Line[1] - Loc_Line[0]);

		dLAI = (LAI - xx_LAI[0]) / (xx_LAI[1] - xx_LAI[0]);
		if (dLAI < 0.) dLAI = 0.;
		else if (dLAI > 1.) dLAI = 1.;

		dSlope = (Slope - xx_Slope[0]) / (xx_Slope[1] - xx_Slope[0]);
		if (dSlope < 0.) dSlope = 0.;
		else if (dSlope > 1.) dSlope = 1.;

		dAspect = (Aspect - xx_Aspect[0]) / (xx_Aspect[1] - xx_Aspect[0]);
		if (dAspect < 0.) dAspect = 0.;
		else if (dAspect > 1.) dAspect = 1.;


		for (i = 0; i < N_Fsnow; i++) {
			for (R = 0; R < Total_Rows; R++) {  //2 climate locations, 
				//LAI
				Snow_11[R][i] = Loc2_Snow111[R][i] * (1. - dLAI) + Loc2_Snow211[R][i] * dLAI;
				Snow_12[R][i] = Loc2_Snow112[R][i] * (1. - dLAI) + Loc2_Snow212[R][i] * dLAI;
				Snow_21[R][i] = Loc2_Snow121[R][i] * (1. - dLAI) + Loc2_Snow221[R][i] * dLAI;
				Snow_22[R][i] = Loc2_Snow122[R][i] * (1. - dLAI) + Loc2_Snow222[R][i] * dLAI;
				//Slope
				Snow_1[R][i] = Snow_11[R][i] * (1. - dSlope) + Snow_21[R][i] * dSlope;
				Snow_2[R][i] = Snow_12[R][i] * (1. - dSlope) + Snow_22[R][i] * dSlope;
				//Aspect
				Snow[R][i] = Snow_1[R][i] * (1. - dAspect) + Snow_2[R][i] * dAspect;
			}
			if (Total_Rows == 1) Snow_x[i] = Snow[0][i];
			else Snow_x[i] = Snow[0][i] * (1. - dLine) + Snow[1][i] * dLine;
		}

//find Fsnow based on Snow condition at the pixel
		if (SnowCondition < Snow_x[0]) {
			F = (Fsnow_Avg[1] - Fsnow_Avg[0]) / (Snow_x[1] - Snow_x[0]);
			Fsnow = Fsnow_Avg[0] + F * (SnowCondition - Snow_x[0]);
			if (Fsnow > 0.99) Fsnow = 0.99;
		}
		else if (SnowCondition > Snow_x[N_Fsnow_1]) {
			F = (Fsnow_Avg[N_Fsnow_1] - Fsnow_Avg[N_Fsnow_2]) / (Snow_x[N_Fsnow_1] - Snow_x[N_Fsnow_2]);
			Fsnow = Fsnow_Avg[N_Fsnow_1] + F * (SnowCondition - Snow_x[N_Fsnow_1]);
		}
		else {
			for (i = 0; i < N_Fsnow_1; i++) {
				if (SnowCondition >= Snow_x[i] && SnowCondition <= Snow_x[i+1]) {
					F = (Fsnow_Avg[i+1] - Fsnow_Avg[i]) / (Snow_x[i+1] - Snow_x[i]);
					Fsnow = Fsnow_Avg[i] + F * (SnowCondition - Snow_x[i]);
					break;
				}
			}
		}

		return Fsnow;
}

