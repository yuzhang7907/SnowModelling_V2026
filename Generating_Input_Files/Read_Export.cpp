#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char *Pick_Name(char *A0);

void Generating_Input_Files_Based_On_A_Template();

#define LAI_NN    7
#define Slope_NN  14
#define Aspect_NN 24
#define Fsnow_NN  32

int main()
{
	Generating_Input_Files_Based_On_A_Template();

  return 0;
}

void Generating_Input_Files_Based_On_A_Template()
{
	char Path_Template[] = "e:\\Snow_Alaska\\Modelling_2026\\Model_Input_Output\\Shrub\\Input\\";  //Shrub, Sedge
	char Path_Out[] =      "e:\\Snow_Alaska\\Modelling_2026\\Model_Input_Output\\Shrub\\Input\\";  //Shrub 
	char Path_In_N[] =     "e:\\Snow_Alaska\\Modelling_2026\\Model_Input_Output\\Shrub\\Input\\";  //Shurb

	char FileName_Template[] = "Shrub_Template.txt";  //sedge, Shrub
	char Site_Name[10] = "Site";

	char String[200], Note[200];
	char Climate_File_Name[] = "Not_used"; // "ImnavaitCreek_Ob_Precip_2010_2025.txt";

	int File_Num, N, File_No;
	int Line, i, aInt, Int[20];
	double aFloat, Float[20];
	double Latitude, Longitude;

	char Veg_Type[] = "Shrub";  //Sedge, Shrub

	FILE* fpt_In, *fpt_Template, * fpt;

	int N_LAI, N_Slope, N_Aspect, N_Fsnow, i_LAI, i_Slope, i_Aspect, i_Fsnow;
	double LAI_Avg[LAI_NN], Slope_Avg[Slope_NN], Aspect_Avg[Aspect_NN], Fsnow_Avg[Fsnow_NN];

//read LAI
	sprintf(String, "%s%s_LAI_N.txt", Path_In_N, Veg_Type);
	if ((fpt_In = fopen(String, "r")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}
	fscanf(fpt_In, "%d", &N_LAI);   fgets(String, 199, fpt_In);
	fgets(String, 199, fpt_In);			//Site note

	for (N = 0; N < N_LAI; N++) {   //
		fscanf(fpt_In, "%d%lf", &i_LAI, &LAI_Avg[N]);
	}
	fclose(fpt_In);

//Read Slope
	sprintf(String, "%s%s_Slope_N.txt", Path_In_N, Veg_Type);
	if ((fpt_In = fopen(String, "r")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}
	fscanf(fpt_In, "%d", &N_Slope);   fgets(String, 199, fpt_In);
	fgets(String, 199, fpt_In);			//Site note

	for (N = 0; N < N_Slope; N++) {   //
		fscanf(fpt_In, "%d%lf", &i_Slope, &Slope_Avg[N]);
	}
	fclose(fpt_In);

	//Read Aspect
	sprintf(String, "%s%s_Aspect_N.txt", Path_In_N, Veg_Type);
	if ((fpt_In = fopen(String, "r")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}
	fscanf(fpt_In, "%d", &N_Aspect);   fgets(String, 199, fpt_In);
	fgets(String, 199, fpt_In);			//Site note

	for (N = 0; N < N_Aspect; N++) {   //
		fscanf(fpt_In, "%d%lf", &i_Aspect, &Aspect_Avg[N]);
	}
	fclose(fpt_In);

	//Read Fsnow
	sprintf(String, "%s%s_Fsnow_N.txt", Path_In_N, Veg_Type);
	if ((fpt_In = fopen(String, "r")) == NULL) {
		printf("Cannot open file %s\n", String);
		exit(0);
	}
	fscanf(fpt_In, "%d", &N_Fsnow);   fgets(String, 199, fpt_In);
	fgets(String, 199, fpt_In);			//Site note

	for (N = 0; N < N_Fsnow; N++) {   //
		fscanf(fpt_In, "%d%lf", &i_Fsnow, &Fsnow_Avg[N]);
	}
	fclose(fpt_In);

	//----------------------------------------------------------------------------------------------------
	File_No = 0;
	for (i_LAI = 0; i_LAI < N_LAI; i_LAI++) {                        //N_LAI
		for (i_Slope = 0; i_Slope < N_Slope; i_Slope++) {			 //N_Slope
			for (i_Aspect = 0; i_Aspect < N_Aspect; i_Aspect++) {	 //N_Aspect
				for (i_Fsnow = 0; i_Fsnow < N_Fsnow; i_Fsnow++) {	 //N_Fsnow

					File_No++;

					// open and read template 
					sprintf(String, "%s%s", Path_Template, FileName_Template);
					if ((fpt_Template = fopen(String, "r")) == NULL) {
						printf("Cannot open the template file %s\n", String);
						exit(0);
					}

					sprintf(String, "%s%s_%d.txt", Path_Out, Site_Name, File_No);   //
					printf("%d ", File_No);

					if ((fpt = fopen(String, "w")) == NULL) {
						printf("Cannot create file %s\n", String);
						exit(0);
					}

					fgets(String, 199, fpt_Template);			//Site note
					fprintf(fpt, "%.2lf %.0lf %.0lf %.2lf\t: %s_%d LAI, Slop, Aspect, Fsnow\n",  
						LAI_Avg[i_LAI], Slope_Avg[i_Slope], Aspect_Avg[i_Aspect], Fsnow_Avg[i_Fsnow], Site_Name, File_No);

					fgets(String, 199, fpt_Template);			//Climate file name
//					fprintf(fpt, "%s", String);
					fprintf(fpt, "%s\t..Climate file name\n", Climate_File_Name);           //modified

					fscanf(fpt_Template, "%d", &aInt);  fgets(String, 199, fpt_Template); //..The first year
					fprintf(fpt, "%d%s", aInt, String);

					fscanf(fpt_Template, "%d", &aInt);  fgets(String, 199, fpt_Template); //..The number of years
					fprintf(fpt, "%d%s", aInt, String);

					fscanf(fpt_Template, "%d%d", &Int[0], &Int[1]);  fgets(String, 199, fpt_Template); //..min and max numbers of years for initailization
					fprintf(fpt, "%d  %d%s", Int[0], Int[1], String);

					fgets(String, 199, fpt_Template); //the line for the Beginning letters for output files
					fprintf(fpt, "%s", String);

//					fprintf(fpt, "%s\t..The beginning of output file\n", Pick_Name(String));  //, File_No

					fscanf(fpt_Template, "%d%d", &Int[0], &Int[1]);  fgets(String, 199, fpt_Template); //..First year and the last years for Daily output
					fprintf(fpt, "%d  %d%s", Int[0], Int[1], String);

					fscanf(fpt_Template, "%d%d", &Int[0], &Int[1]);  fgets(String, 199, fpt_Template); //..First year and the last years for Monthly output
					fprintf(fpt, "%d  %d%s", Int[0], Int[1], String);

					fscanf(fpt_Template, "%d%d", &Int[0], &Int[1]);  fgets(String, 199, fpt_Template); //..First year and the last years for annual output
					fprintf(fpt, "%d  %d%s", Int[0], Int[1], String);

					fscanf(fpt_Template, "%d%d", &Int[0], &Int[1]);  fgets(String, 199, fpt_Template); //..the first and last layer for soil Temperaure output
					fprintf(fpt, "%d  %d%s", Int[0], Int[1], String);
					//------------------------------------------
					fscanf(fpt_Template, "%lf", &aFloat);  fgets(String, 199, fpt_Template); //..Latitude
					fprintf(fpt, "%.4lf%s", aFloat, String);

					fscanf(fpt_Template, "%lf", &aFloat);  fgets(String, 199, fpt_Template); //..Slope
					fprintf(fpt, "%.4lf%s", Slope_Avg[i_Slope], String);         //fprintf(fpt, "%.4lf%s", aFloat, String);

					fscanf(fpt_Template, "%lf", &aFloat);  fgets(String, 199, fpt_Template); //..Aspect		fprintf(fpt, "%.4lf\t\t%s", aFloat, String);
					fprintf(fpt, "%.4lf%s", Aspect_Avg[i_Aspect], String);        //fprintf(fpt, "%.4lf%s", aFloat, String);
					//---------------------------
					fgets(String, 199, fpt_Template);			//a note line
					fprintf(fpt, "%s", String);

					for (i = 0; i < 8; i++) fscanf(fpt_Template, "%lf", &Float[i]);  //viewshed 1-8
					fgets(String, 199, fpt_Template);

					for (i = 0; i < 8; i++)  fprintf(fpt, "%.2lf ", Float[i]);
					fprintf(fpt, "%s", String);

					for (i = 0; i < 8; i++) fscanf(fpt_Template, "%lf", &Float[i]);  //viewshed 9-16
					fgets(String, 199, fpt_Template);
					for (i = 0; i < 8; i++)  fprintf(fpt, "%.2lf ", Float[i]);
					fprintf(fpt, "%s", String);
					//---------------------
					fgets(String, 199, fpt_Template);			//a note line for soil profiles
					fprintf(fpt, "%s", String);

					//read ground profile
					double Last_Depth, Depth, Psom, Otex, Pice, Pstone;
					int Mtex;
					double Peat_Depth, Peat_Depth_x, F;
					int IF_LastPeat_Layer;

					/*
								Peat_Depth = Peat_Avg[i_Peat];
								Last_Depth = 0.;
								Otex = 0.0;

								Line = 0;
								fscanf(fpt_Template, "%lf", &Depth);
								while (Depth > 0) {
									fscanf(fpt_Template, "%d%lf%lf%lf%lf", &Mtex, &aFloat, &aFloat, &aFloat, &Pstone);  //Depth and Mtex used. No others
									fgets(String, 199, fpt_Template);

									if (Peat_Depth >= Depth) {  //peat
										Psom = 100.0;
										Otex += 0.2;
										if (Otex > 2.5) Otex = 2.5;

										if (Depth > 100)  Pice = 50;   //add extral ice in peat layer bleow 100cm
										else Pice = 0;

					//					Pstone = 0;
									}
									else {
										if (Peat_Depth >= Last_Depth) {  //1st mineral layer
											Peat_Depth_x = Peat_Depth - Last_Depth;
											F = Peat_Depth_x / (Depth - Last_Depth);
											if (Peat_Depth < 10.0)  Psom = (0.1 + 0.9 * F) * 100.;  //no peat layer
											else                    Psom = (0.5 + 0.5 * F) * 100.;  //at least 1 peat layer
											Otex += 0.3 + 0.3 * (1.0 - F);
											if (Otex > 2.9) Otex = 2.9;
										}
										else {
											Psom *= 0.5/((Depth-Last_Depth)/10.0);  //reduce half each 20cm
											Otex += 0.3;
											if (Otex > 2.9) Otex = 2.9;
										}

										Pice = 0;
					//					Pstone = 0;
									}
									fprintf(fpt, "%.2lf\t%d\t%.1lf\t%.3lf\t%.1lf\t%.1lf%\t..Note\n", Depth, Mtex, Psom, Otex, Pice, Pstone);

									Last_Depth = Depth;
									fscanf(fpt_Template, "%lf", &Depth);
									Line++;
								}
					*/
					fscanf(fpt_Template, "%lf", &Depth);
					while (Depth > 0) {
						fscanf(fpt_Template, "%d%lf%lf%lf%lf", &Mtex, &Psom, &Otex, &Pice, &Pstone);  //Depth and Mtex used. No others 
						fgets(String, 199, fpt_Template);

						fprintf(fpt, "%.2lf\t%d\t%.1lf\t%.3lf\t%.1lf\t%.1lf%\t..Note\n", Depth, Mtex, Psom, Otex, Pice, Pstone);

						fscanf(fpt_Template, "%lf", &Depth);
					}

					fgets(String, 199, fpt_Template); //end of soil profile
					fprintf(fpt, "-1%s", String);

					//---------generate ground profile
					fscanf(fpt_Template, "%lf", &aFloat);  fgets(String, 199, fpt_Template); //..Fraction of quartz in soil minerals
					fprintf(fpt, "%.4lf%s", aFloat, String);

					fscanf(fpt_Template, "%lf", &aFloat);  fgets(String, 199, fpt_Template); //..Thermal conductivity of rock (J/(cm.k.s) )
					fprintf(fpt, "%.4lf%s", aFloat, String);

					fscanf(fpt_Template, "%lf", &aFloat);  fgets(String, 199, fpt_Template); //..Geothermal heat Flux (w/m2)
					fprintf(fpt, "%.4lf%s", aFloat, String);

					fscanf(fpt_Template, "%lf", &aFloat);  fgets(String, 199, fpt_Template); //..Snow compacting factor
					fprintf(fpt, "%.4lf%s", aFloat, String);

					fscanf(fpt_Template, "%lf", &aFloat);  fgets(String, 199, fpt_Template); //..Minimum effective thermal conductivity of snow (0.00027-0.01), W/(cm k)
					fprintf(fpt, "%.4lf%s", aFloat, String);

					fscanf(fpt_Template, "%lf", &aFloat);  fgets(String, 199, fpt_Template); //..surface inflow
					fprintf(fpt, "%.4lf%s", aFloat, String);

					fscanf(fpt_Template, "%lf%lf", &Float[0], &Float[1]);  fgets(String, 199, fpt_Template); //..Ground inflow: minimum depth(cm) and rate
					fprintf(fpt, "%.2lf %.4lf%s", Float[0], Float[1], String);

					fscanf(fpt_Template, "%lf%lf", &Float[0], &Float[1]);  fgets(String, 199, fpt_Template); //..Surface outflow: minimum depth(cm) and rate
					fprintf(fpt, "%.2lf %.4lf%s", Float[0], Float[1], String);

					fscanf(fpt_Template, "%lf%lf", &Float[0], &Float[1]);  fgets(String, 199, fpt_Template); //..Ground outflow: minimum depth(cm) and rate
					fprintf(fpt, "%.2lf %.4lf%s", Float[0], Float[1], String);

					fgets(String, 199, fpt_Template);			//a note line for vegetation
					fprintf(fpt, "%s", String);

					Line = 0;
					fscanf(fpt_Template, "%d", &aInt);  //year
					while (aInt > 0) {
						fscanf(fpt_Template, "%d%lf%lf%lf%lf%lf%d", &Int[0], &Float[0], &Float[1], &Float[2], &Float[3], &Float[4], &Int[1]);  //Line 0
						fgets(String, 199, fpt_Template);

						Float[0] = LAI_Avg[i_LAI];
						if(Int[0] == 4) Float[1] = 0.3 + LAI_Avg[i_LAI] * 0.5;  //shrub height
						else  			Float[1] = 0.2 + LAI_Avg[i_LAI] * 0.3;  //sedge height

						Float[2] = Fsnow_Avg[i_Fsnow];    //snow drift

						fprintf(fpt, "%d\t%d\t%.4lf\t%.4lf\t%.4lf\t%.3lf\t%.2lf\t%d%s", aInt, Int[0], Float[0], Float[1], Float[2], Float[3], Float[4], Int[1], String);

						Line++;
						fscanf(fpt_Template, "%d", &aInt);  //year
					}
					fgets(String, 199, fpt_Template); //end of the file
					fprintf(fpt, "-1%s", String);

					fclose(fpt_Template);
					fclose(fpt);
				}   //i_LAI
			}    //i_Slope
		}  //Aspect
	}	//Fsnow
}




char *Pick_Name(char *A0)  {
  char A1[201];
  int i, n;

  n = 0;
  for(i=1; i<200; i++) {
	  if( int(A0[i]) == 32 || int(A0[i]) == 9) {
		  n = i;
		  break;
	  }
  }

  strncpy(A1, A0, n);
  strncpy(A1 + n,"\0",1);

  return A1;
}


/*
void Hourly_To_Daily() {
//read Cweed hourly data and transfer to daily

	int i, I, N;

	int Year, Month, Day, Hour, Day0;
	int Rad0[24], Rad[24], T[24], Tdew[24], Wind[24], Cloud[24];
	double Rad0_D, Rad_D, T_D, Tmax, Tmin, Tdew_D, Wind_D, Cloud_D, Vap_D;
	double F;

//	char Path[] = "E:\\Cweeds\\MANITOBA\\ChurchillA_1953-2005\\";
//	char Path[] = "E:\\Cweeds\\UnZipped_data\\INuvikUA_1958-2005\\";
//	char Path[] = "E:\\Cweeds\\UnZipped_data\\Ottawa\\OttawaNRC_1953-2005\\";
//	char Path[] = "E:\\Cweeds\\UnZipped_data\\FortResolutionA_1960-1969\\";
//	char Path[] = "E:\\Cweeds\\UnZipped_data\\VancouverUBC_1953-2005\\";
//	char Path[] = "E:\\Cweeds\\UnZipped_data\\Cartwright_1964-2005\\";
//	char Path[] = "E:\\Cweeds\\UnZipped_data\\YellowknifeA_1953-2005\\";
//	char Path[] = "E:\\Cweeds\\UnZipped_data\\WhitehorseA_1953-2005\\";
//	char Path[] = "E:\\Cweeds\\UnZipped_data\\TeslinA_1955-2005\\";
	char Path[] = "E:\\Cweeds\\UnZipped_data\\Resolute_1963-2005\\";

	char A[200], B[50],string[100];
  
	FILE *fpt_In, *fpt_Out;

//0        10        20        30        40        50        60        70        80        90        100       110  
//0--------90--------90--------90--------90--------90--------90--------90--------90--------90--------90--------90--------9
//15901A1953010101   0   0M    0Q    0M    0Q   0Q   0Q99999 0   60 6000   48 00000005  9935 -211 -211 315  156 10 10 99


sprintf(string,"%sResolute_Daily.txt", Path);
if((fpt_Out = fopen(string, "w")) == NULL) {
	printf("Cannot open file %s\n", string);
	exit(0);
}
fprintf(fpt_Out, "Year Month Day0 Rad0_D Rad_D Tmin Tmax T_D Tdew_D Vap_D Wind_D Cloud_D Rad_D/Rad0\n");

sprintf(string,"%s17901.WY2", Path);
if((fpt_In = fopen(string, "r")) == NULL) {
	printf("Cannot open file %s\n", string);
	exit(0);
}

Day0 = 0;
while ( (fgets(A, 200, fpt_In) != NULL)) {
	strncpy(B, A+12, 2);
	strncpy(B+2,"\0",1);
	Day = atoi(B);

	
	if(Day != Day0) {
		if(Day0 !=0) {  //not the 1st
			Rad0_D = 0;
			for(i=0; i<24; i++) {
				if(Rad0[i]<9998) {
					N++;
					Rad0_D += Rad0[i];
				}
			}
			if(N>20) Rad0_D = Rad0_D*0.001;  //mj/m
			else Rad0_D = -9999;

			Rad_D = 0;
			for(i=0; i<24; i++) {
				if(Rad[i]<9998) {
					N++;
					Rad_D += Rad[i];
				}
			}
			if(N>20) Rad_D = Rad_D*0.001;  //mj/m
			else Rad_D = -9999;

			N = 0;
			T_D = 0;
			Tmax = -1000;
			Tmin = 1000;
			for(i=0; i<24; i++) {
				if(T[i]<9998) {
					N++;
					T_D += T[i];
					if(T[i]>Tmax) Tmax = T[i];
					if(T[i]<Tmin) Tmin = T[i];
				}
			}
			if(N>20) {
				T_D = T_D/N*0.1;  //degree
				Tmax *= 0.1;
				Tmin *= 0.1;
			}
			else {
				T_D = -9999;
				Tmax = -9999;
				Tmin = -9999;
			}

			N = 0;
			Vap_D = 0;
			Tdew_D =0;
			for(i=0; i<24; i++) {
				if(Tdew[i]<9998) {
					N++;
					Vap_D +=  6.11*exp(17.27*Tdew[i]*0.1/(Tdew[i]*0.1+237.3));
					Tdew_D += Tdew[i];
				}
			}
			if(N>20) {
				Tdew_D = Tdew_D/N*0.1;  //degree
				Vap_D /= N;
			}
			else {
				Tdew_D = -9999;
				Vap_D = -9999;
			}

			N = 0;
			Wind_D = 0;
			for(i=0; i<24; i++) {
				if(Wind[i]<9998) {
					N++;
					Wind_D += Wind[i];
				}
			}
			if(N>=1) Wind_D = Wind_D/N * 0.1;  //  >= 1  m/s
			else Wind_D = -9999;

			N = 0;
			Cloud_D = 0;
			for(i=0; i<24; i++) {
				if(Cloud[i]<98 && Rad0[i]>0) {
					N++;
					Cloud_D += Cloud[i];
				}
			}
			if(N>20) Cloud_D = Cloud_D/N*0.1;  
			else Cloud_D = -9999;

			if(Rad_D > -9990 && Rad0_D>-9990) F =  Rad_D/(Rad0_D+0.0001);
			else F = -9999;

		    fprintf(fpt_Out, "%d %d %d %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf  %.3lf  %.4lf\n", Year, Month, Day0, 
				Rad0_D, Rad_D, Tmin, Tmax, T_D, Tdew_D, Vap_D, Wind_D, Cloud_D, F);

		}

		for (i=0; i<24; i++) {
	      Rad0[i] = 9999;
		  Rad[i]  = 9999;
		  T[i]    = 9999;
		  Tdew[i] = 9999;
		  Wind[i] = 9999;
		  Cloud[i]= 99;
		}
		Day0 = Day;
	}

	strncpy(B, A+6, 4);
	strncpy(B+4,"\0",1);
	Year = atoi(B);

	strncpy(B, A+10, 2);
	strncpy(B+2,"\0",1);
	Month = atoi(B);

	strncpy(B, A+14, 2);
	strncpy(B+2,"\0",1);
	Hour = atoi(B);

	I = Hour -1;

//Extraterrestrial irradiance, kJ/m2
	strncpy(B, A+16, 4);
	strncpy(B+4,"\0",1);
	Rad0[I] = atoi(B);

//Global horizontal irradiance, kJ/m2
	strncpy(B, A+20, 4);
	strncpy(B+4,"\0",1);
	Rad[I] = atoi(B);

//Dry bulb temperature, 0.1 °C
	strncpy(B, A+91, 4);
	strncpy(B+4,"\0",1);
	T[I] = atoi(B);

//Dew temperature, 0.1 °C
	strncpy(B, A+96, 4);
	strncpy(B+4,"\0",1);
	Tdew[I] = atoi(B);

//wind speed, 0.1m/s
	strncpy(B, A+105, 4);
	strncpy(B+4,"\0",1);
	Wind[I] = atoi(B);

//cloud cover
	strncpy(B, A+110, 2);
	strncpy(B+2,"\0",1);
	Cloud[I] = atoi(B);

}
//last day
			Rad0_D = 0;
			for(i=0; i<24; i++) {
				if(Rad0[i]<9998) {
					N++;
					Rad0_D += Rad0[i];
				}
			}
			if(N>20) Rad0_D = Rad0_D*0.001;  //mj/m
			else Rad0_D = -9999;

			Rad_D = 0;
			for(i=0; i<24; i++) {
				if(Rad[i]<9998) {
					N++;
					Rad_D += Rad[i];
				}
			}
			if(N>20) Rad_D = Rad_D*0.001;  //mj/m
			else Rad_D = -9999;

			N = 0;
			T_D = 0;
			Tmax = -1000;
			Tmin = 1000;
			for(i=0; i<24; i++) {
				if(T[i]<9998) {
					N++;
					T_D += T[i];
					if(T[i]>Tmax) Tmax = T[i];
					if(T[i]<Tmin) Tmin = T[i];
				}
			}
			if(N>20) {
				T_D = T_D/N*0.1;  //degree
				Tmax *= 0.1;
				Tmin *= 0.1;
			}
			else {
				T_D = -9999;
				Tmax = -9999;
				Tmin = -9999;
			}
          
			N = 0;
			Vap_D = 0;
			Tdew_D =0;
			for(i=0; i<24; i++) {
				if(Tdew[i]<9998) {
					N++;
					Vap_D +=  6.11*exp(17.27*Tdew[i]*0.1/(Tdew[i]*0.1+237.3));
					Tdew_D += Tdew[i];
				}
			}
			if(N>20) {
				Tdew_D = Tdew_D/N*0.1;  //degree
				Vap_D /= N;
			}
			else {
				Tdew_D = -9999;
				Vap_D = -9999;
			}

			N = 0;
			Wind_D = 0;
			for(i=0; i<24; i++) {
				if(Wind[i]<9998) {
					N++;
					Wind_D += Wind[i];
				}
			}
			if(N>=1) Wind_D = Wind_D/N * 0.1;  // >= 1 m/s
			else Wind_D = -9999;

			N = 0;
			Cloud_D = 0;
			for(i=0; i<24; i++) {
				if(Cloud[i]<98 && Rad0[i]>0) {
					N++;
					Cloud_D += Cloud[i];
				}
			}
			if(N>20) Cloud_D = Cloud_D/N*0.1;  
			else Cloud_D = -9999;

			if(Rad_D > -9990 && Rad0_D>-9990) F =  Rad_D/(Rad0_D+0.0001);
			else F = -9999;

		    fprintf(fpt_Out, "%d %d %d %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf  %.3lf  %.4lf\n", Year, Month, Day0, 
				Rad0_D, Rad_D, Tmin, Tmax, T_D, Tdew_D, Vap_D, Wind_D, Cloud_D, F);

fclose(fpt_In);
fclose(fpt_Out);

}



float max(float a, float b)
{
	float c;
   if(a>b) c=a;
   else c=b;
   return c;
}
*/
