/**********************************************************************
Filename:			ass2.cpp
Version: 			1.0
Author:				Lucas Estienne
Student No:  		040 819 959
Course Name/Number:	Numerical Computing CST8233
Lab Sect: 			302
Assignment #:		2
Assignment name:	World Population
Due Date:			November 13 2016
Submission Date:	November 13 2016
Professor:			Andrew Tyler
Purpose:			Implements functions to extrapolate/interpolate the world population.
*********************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC	// need this to get the line identification
//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF); // in main, after local declarations
//NB must be in debug build
#include <crtdbg.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <map>
using namespace std;

bool worldPop();
void fitTransformFromFile(map<string, double>&);
void popArrayFromFile(double**&, int&);
void transformPop(double **, int);
void leastSquaresFit(double**, int, map<string, double>&);
void doInterpExtrap(map<string, double>& equ);

enum{ RUNNING = 1 };

/********************************************************************
Function name:		main
Purpose:			Main function, menu for the program
In parameters:		none
Out parameters:		int
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
int main()
{
	char response;
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	while (RUNNING)
	{
		cout << endl << "*******************************************************" << endl;
		cout << "Exponential fit by Linear Regression" << endl;
		cout << "1. Read Data from File" << endl;
		cout << "2. Quit" << endl;
		cout << "*******************************************************" << endl;
		cout << "Select an option: ";
		cin >> response;
		switch (response)
		{
		case '1': if (!worldPop()){ return 0; }; break; // quit program if user chooses to quit in worldPop()
		case '2': return 0;
		default:cout << "Please enter a valid option\n";
		}
		cout << endl;
	}
}

/********************************************************************
Function name:		worldPop
Purpose:			Calls the fitTransformFromFile function to get values for 
					the N0 and b variables and then provides a menu for the user
					to interpolate/extrapolate the world population for a given year.
In parameters:		none
Out parameters:		bool
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
bool worldPop()
{
	char response;
	map<string, double> equation;

	fitTransformFromFile(equation);

	while (RUNNING)
	{
		cout << endl << "*******************************************************" << endl;
		cout << "Exponential Interpolation/Extrapolation" << endl;
		cout << "1. Interpolation/Extrapolation of World Population" << endl;
		cout << "2. Quit Interpolation/Extrapolation" << endl;
		cout << "*******************************************************" << endl;
		cout << "Select an option: ";
		cin >> response;
		switch (response)
		{
		case '1': doInterpExtrap(equation); break;
		case '2': return false; // quit
		default:cout << "Please enter a valid option\n";
		}
		cout << endl;
	}
}

/********************************************************************
Function name:		fitTransformFromFile
Purpose:			Gets population values from a file and then transforms & fits
					that data. Sets the b, C and N0 variables of the equation associative array.
					Also outputs those variables to stdout.
In parameters:		map<string, double>& associative array to contain the b, C and N0 variables
Out parameters:		none
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
void fitTransformFromFile(map<string, double>& equ){
	int numRecords;
	double ** popArray = nullptr; // init so it can be passed

	popArrayFromFile(popArray, numRecords);
	cout << endl << "File read into memory" << endl;

	cout << endl << "EXPONENTIAL FUNCTION" << endl;
	cout << "Fit data to double exponential:  N = N0*exp(b*(exp(t*t))" << endl;

	transformPop(popArray, numRecords);
	leastSquaresFit(popArray, numRecords, equ);

	equ["N0"] = exp(equ["C"]); // C = Ln(N0) therefore N0 = exp(C)

	delete[] popArray[0]; // free up memory
	delete[] popArray;

	cout << endl << "DOUBLE EXPONENTIAL:  y = " << equ["N0"] << "*exp(" << equ["b"] << "*exp(t*t))" << endl;
}

/********************************************************************
Function name:		popArrayFromFile
Purpose:			Prompts the user for a file to get year/population values
					from. Populates the passed array with those values and sets
					the passed number of records to the proper value.
In parameters:		double**& pointer to a (nullptr) 2D array to contain population data, int& number of records
Out parameters:		none
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
void popArrayFromFile(double **&popArray, int &numRecords){
	char fname[256];
	unsigned int i = 0;
	int j;

	while (RUNNING){
		cout << "Please enter the name of the file to open: ";
		cin >> fname;
		ifstream popfile(fname); // try to open file the user input

		if (popfile.is_open()){ // successfully opened the file
			cout << endl << "FILE OPENED FOR READING" << endl;

			numRecords = (int)count(istreambuf_iterator<char>(popfile), istreambuf_iterator<char>(), '\n');
			popfile.unget(); // go back 1 char to check for newline
			if (popfile.get() != '\n') ++numRecords; // add one in case last line doesn't have \n

			cout << "There are " << numRecords << " records." << endl;

			popArray = new double*[numRecords]; //initialize 2D double array
			popArray[0] = new double[numRecords * 2]; // world pop file has two columns, N and t
			for (j = 1; j < numRecords; j++)
				popArray[j] = popArray[j - 1] + 2; // two columns, N and t

			popfile.clear(); // clears the EOF state
			popfile.seekg(0, ios::beg); // go back to beginning of file after we counted

			while ((popfile >> popArray[i][0]) && (popfile >> popArray[i][1])) ++i; // read data

			popfile.close(); // close file
			break; // break out of while
		} else { // file didn't exist or couldn't be opened for some reason
			cout << endl << "Couldn't open population file";
		}
	}
}


/********************************************************************
Function name:		transformPop
Purpose:			Transforms the data in the passed population array
					so that it can be fitted properly.
In parameters:		double** 2D array containing population data, int number of records in that array
Out parameters:		none
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
void transformPop(double ** popAray, int numRecords){
	int i;

	for (i = 0; i < numRecords; i++){
		popAray[i][0] = exp(popAray[i][0] * popAray[i][0]); // X =  exp(t*t)
		popAray[i][1] = log(popAray[i][1]); // Y = Ln(N)
	}
}

/********************************************************************
Function name:		leastSquaresFit
Purpose:			Uses the least square method of linear regression to derive
					the C and b variables in the Y = bX + C equation.
In parameters:		double** 2D array containing population data, int number of records in that array,
					map<string, double>& associative array to contain the b, C and N0 variables
Out parameters:		none
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
void leastSquaresFit(double** popArray, int numRecords, map<string, double>& equ){
	double sigmaX = 0, sigmaY = 0, sigmaXY = 0, sigmaX2 = 0, denominator;
	int i;

	for (i = 0; i < numRecords; i++){
		sigmaX += popArray[i][0]; // sum the transformed year values
		sigmaY += popArray[i][1]; // sum the transformed population values
		sigmaXY += popArray[i][0] * popArray[i][1]; // sum the transformed year values times the transformed population values
		sigmaX2 += popArray[i][0] * popArray[i][0]; // sum the transformed year values squared
	}
	denominator = numRecords*sigmaX2 - sigmaX*sigmaX; // the denominator is the same for both getting m and b in y = mx+b

	equ["C"] = (sigmaX2*sigmaY - sigmaXY*sigmaX) / denominator;
	equ["b"] = (numRecords*sigmaXY - sigmaX*sigmaY) / denominator;
}

/********************************************************************
Function name:		doInterpExtrap
Purpose:			Prompts the user for a year to inter/extrapolate world population for
					and then outputs the result to stdout.
In parameters:		map<string, double>& associative array containing b and N0 variables
Out parameters:		none
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
void doInterpExtrap(map<string, double>& equ){
	int year;
	double t, population;

	cout << "Interpolation/Extrapolation of World Population" << endl;
	cout << "Please enter the year (e.g. 2100): ";
	cin >> year;

	t = double(year) / 1000; // out input data is formatted in thousands of years.
	population = equ["N0"]*exp(equ["b"]*exp(t*t));
	cout << "World population at " << year << " = " << population << " billion" << endl;
}