// Code_Call_exe_file.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS 1

//#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include  <math.h>
#include  <string.h>
#include <windows.h>
#include <thread>

#define Threat_Num 8   //sedge 8, Shrub 15
//#define Threat_Num 15   //Shrub 15

void Run_The_Model(char* String);


int main()
{
    char String[Threat_Num][150];
    char Path[] = "E:\\Snow_Alaska\\Modelling_2026\\Codes\\TheCode_NEST_Snow_Console\\Release\\";
    char exe_File_Name[] = "TheCode_NEST_Read1Climate_Console.exe";

    int NN, Climate_Loc, Veg_Type, Site_Range[2];

    Climate_Loc = 4; //1,2 (CLPX),3,4 (HV)
    Veg_Type = 1;  //1:Sedge, 2: Shrub

    Site_Range[0] = 1;

    NN = 0;
    for (NN = 0; NN <= Threat_Num-2; NN++) {   
        Site_Range[1] = Site_Range[0] + 4999;  

        sprintf(String[NN], "%s%s %d %d %d %d", Path, exe_File_Name, Veg_Type, Climate_Loc, Site_Range[0], Site_Range[1]);
        printf("%d %s\n", NN, String[NN]);

        Site_Range[0] = Site_Range[1] + 1;
    }

    Site_Range[1] = 37632;  //sedge:37,632
//  Site_Range[1] = 75264;  //shrub 75264
    sprintf(String[NN], "%s%s %d %d %d %d", Path, exe_File_Name, Veg_Type, Climate_Loc, Site_Range[0], Site_Range[1]);
    printf("%d %s\n", NN, String[NN]);

    std::thread t_0(Run_The_Model, String[0]);
    std::thread t_1(Run_The_Model, String[1]);
    std::thread t_2(Run_The_Model, String[2]);
    std::thread t_3(Run_The_Model, String[3]);
    std::thread t_4(Run_The_Model, String[4]);
    std::thread t_5(Run_The_Model, String[5]);
    std::thread t_6(Run_The_Model, String[6]);
    std::thread t_7(Run_The_Model, String[7]);
/*
//for shrub
    std::thread t_8(Run_The_Model, String[8]);
    std::thread t_9(Run_The_Model, String[9]);
    std::thread t_10(Run_The_Model, String[10]);
    std::thread t_11(Run_The_Model, String[11]);
    std::thread t_12(Run_The_Model, String[12]);
    std::thread t_13(Run_The_Model, String[13]);
    std::thread t_14(Run_The_Model, String[14]);
*/
    t_0.join();     t_1.join();     t_2.join();     t_3.join();     t_4.join();
    t_5.join();     t_6.join();     t_7.join();

//for Shrub
//    t_8.join();     t_9.join();     t_10.join();     t_11.join();     t_12.join();
//   t_13.join();    t_14.join();  
}

void Run_The_Model(char *String)
{
    int Result;
    Result = system(String);

 //   if (Result != 0) {
 //       printf("Error in running %s\n", String);
 //   }
}
