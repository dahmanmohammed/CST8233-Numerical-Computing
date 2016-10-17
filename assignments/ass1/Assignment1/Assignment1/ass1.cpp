/**********************************************************************
Filename:			ass1.cpp
Version: 			1.0
Author:				Lucas Estienne
Student No:  		040 819 959
Course Name/Number:	Numerical Computing CST8233
Lab Sect: 			300
Assignment #:		1
Assignment name:	Ring Tone
Due Date:			October 16 2016
Submission Date:	October 16 2016
Professor:			Andrew Tyler
Purpose:			Implements functions to calculate a MacLaurin series for a user defined number of terms,
					and displays the results.
*********************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC	// need this to get the line identification
//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF); // in main, after local declarations
//NB must be in debug build
#include <crtdbg.h>
#include <iostream>
#include <math.h>

using namespace std;

void evalSeries();
double * calcTIncrements(double, int);
double ** calcTIncrementExponents(double [], int, int);
double * calcSeries(int, double **, double [], int);
double * calcExact(int, double []);
double * calcRExactE(int, double [], double []);
double * calcRSerE(int, double[], double **, double);
int getInputNumTerms(int [], int);
double getInputRange(double, double);
bool inArray(int, int [], int);
void printResults(int, double[], double[], double[], double[], double[]);
double calcTerm(double, double);

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
		cout << endl << "Evaluate the Maclaurin Series approximation to (1-exp(-t))*sin(t)" << endl;
		cout << "1. Evaluate the series" << endl;
		cout << "2. Quit" << endl;
		cout << "CHOICE: ";
		cin >> response;
		switch (response)
		{
		case '1': evalSeries(); break;
		case '2': return 0;
		default:cout << "Please enter a valid option\n";
		}
		cout << endl;
	}
}

/********************************************************************
Function name:		evalSeries
Purpose:			Evaluates and then prints the results for the series (1 – exp(-t))*sin(t).
In parameters:		none
Out parameters:		void
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
void evalSeries(){
	// expansion is 0 + ((0/1!)*t) + ((2/2!)*t^2) + ((-3/3!)*t^3) + ((0/4!)*t^4) + ((5/5!)*t^5) + ((-8/6!)*t^6) + ((7/7!)*t^7)
	//			    0        0          nonzero        nonzero            0           nonzero            0             0
	cout << endl << "Evaluating the series" << endl;
	int validTerms[4] = { 1, 2, 3, 4 };
	double minRange = 0.0, maxRange = 1.0;
	int numIncrements = 11;

	int numTerms = getInputNumTerms(validTerms, sizeof(validTerms));
	double rangeToEval = getInputRange(minRange, maxRange);

	double seriesFrac[7] = { 0.0 / 1.0, 2.0/2.0, -3.0/6.0, 0.0/24.0,  5.0 / 120.0, -8.0/720.0, 7.0/5040.0}; /* including first two 0 terms */

	double * tIncrements = calcTIncrements(rangeToEval, numIncrements);
	double ** seriesT = calcTIncrementExponents(tIncrements, numIncrements, numTerms);

	double * ftSeries = calcSeries(numIncrements, seriesT, seriesFrac, numTerms);
	double * ftExact = calcExact(numIncrements, tIncrements);
	double * rExactE = calcRExactE(numIncrements, ftExact, ftSeries);
	double * rSerE = calcRSerE(numIncrements, ftSeries, seriesT, seriesFrac[6]);

	printResults(numIncrements, tIncrements, ftSeries, ftExact, rExactE, rSerE);

	/* free */
	delete[] tIncrements;
	delete[] seriesT[0];
	delete[] seriesT;
	delete[] ftSeries;
	delete[] ftExact;
	delete[] rExactE;
	delete[] rSerE;
}

/********************************************************************
Function name:		calcTerm
Purpose:			Calculates a specific term. (i.e. last truncated term)
In parameters:		fra(first half of a series expansion), t(t^2 half of the series expansion)
Out parameters:		double
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
double calcTerm(double fra, double t){
	return fra*t;
}

/********************************************************************
Function name:		calcTIncrements
Purpose:			Returns an array of all the t/10 increments
In parameters:		t(t to calculate increments for), increments(number of increments)
Out parameters:		double[]
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
double * calcTIncrements(double t, int increments){
	double * tIncrements = new double[increments];
	int i;

	for (i = 0; i < increments; i++){
		tIncrements[i] = t/10*i;
	}

	return tIncrements;
}

/********************************************************************
Function name:		calcTIncrementsExponents
Purpose:			Returns a multidimensional array containing all the second halves of the expanded maclaurin terms
In parameters:		t[](array of t increments), increments(number of increments), numTerms(number of terms)
Out parameters:		double[][]
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
double ** calcTIncrementExponents(double t[], int increments, int numTerms){
	double ** tExps = new double*[increments];
	tExps[0] = new double[increments * 7]; /* 7 is the num of terms including 0s */
	int i, j;

	for (i = 1; i < increments; i++)
		tExps[i] = tExps[i - 1] + 7; /* 7 is the num of terms including 0s */

	for (i = 0; i < increments; i++){
		tExps[i][0] = t[i];
		for (j = 1; j < 7; j++){ /* 7 is the num of terms including 0s */
			tExps[i][j] = tExps[i][j - 1] * t[i];
		}
	}
	

	return tExps;
}

/********************************************************************
Function name:		calcSeries
Purpose:			Returns an array containing the series values for all the increments
In parameters:		timesToCalc(number of increments), **t(2d array of second halves for maclaurin series), 
					seriesFrac[](first halves of maclaurin series), numTerms(number of terms to calc for)
Out parameters:		double[]
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
double * calcSeries(int timesToCalc, double **t, double seriesFrac[], int numTerms){
	double * ftSeries = new double[timesToCalc];
	double curTerm;
	int i, j;

	for (j = 0; j < timesToCalc; j++){
		ftSeries[j] = 0;
		for (i = 0; i < numTerms + 3; i++){	
			while (seriesFrac[i] == 0 && j != 0){
				i++;
			}
			if (i >= numTerms + 3) break;
			curTerm = seriesFrac[i] * t[j][i];
			ftSeries[j] += curTerm;
		}
	}

	return ftSeries;
}

/********************************************************************
Function name:		calcExact
Purpose:			Calculates the exact values of the (1 – exp(-t))*sin(t) function using the math lib
In parameters:		timesToCalc(number of increments), t[](array of t increments)
Out parameters:		double[]
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
double * calcExact(int timesToCalc, double t[]){
	double * ftExact = new double[timesToCalc];
	int i;

	for (i = 0; i < timesToCalc; i++){
		ftExact[i] = (1 - exp(-t[i]))*sin(t[i]);
	}
	return ftExact;
}

/********************************************************************
Function name:		calcRExactE
Purpose:			Calculates % Relative Exact Error (%RExactE) =100*(exact value – series value)/exact value
In parameters:		timesToCalc(number of increments), ftExact[](array of exact values), ftSeries[](array of series values)
Out parameters:		double[]
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
double * calcRExactE(int timesToCalc, double ftExact[], double ftSeries[]){
	double * rExactE = new double[timesToCalc];
	int i;
	
	for (i = 0; i < timesToCalc; i++){
		if (i > 0){
			rExactE[i] = 100 * (ftExact[i] - ftSeries[i]) / ftExact[i];
		}
		else{ rExactE[i] = 0; }
	}
	return rExactE;
}

/********************************************************************
Function name:		calcRSerE
Purpose:			Calculates % Relative Series Error (%RSerE) =100*first truncated term/series value
In parameters:		timesToCalc(number of increments), ftSeries[](array of series values), 
					**t(2d array of second halves for maclaurin series),
					fra(first half of a series expansion for last term)
Out parameters:		double[]
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
double * calcRSerE(int timesToCalc, double ftSeries[], double **t, double fra){
	double * rSerE = new double[timesToCalc];
	double truncTerm;
	int i;

	for (i = 0; i < timesToCalc; i++){
		if (i > 0){
			truncTerm = calcTerm(fra, t[i][6]);
			rSerE[i] = 100 * truncTerm / ftSeries[i];
		}
		else {
			rSerE[i] = 0;
		}
	}

	return rSerE;
}

/********************************************************************
Function name:		getInputNumTerms
Purpose:			Gets number of terms to expand for from user
In parameters:		validTerms[](array of valid terms), s(length of that array)
Out parameters:		int
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
int getInputNumTerms(int validTerms[], int s){
	int inputNumTerms;
	bool validInputNumTerms = false;

	while (!validInputNumTerms){
		cout << "Please enter the number of (non-zero) terms in the series (1, 2, 3 or 4): ";
		cin >> inputNumTerms;

		if (!inArray(inputNumTerms, validTerms, s)){ /* number of terms is not valid */
			cout << endl << "Please enter a valid number of terms" << endl;
		}
		else{ validInputNumTerms = true; }
	}

	return inputNumTerms;
}

/********************************************************************
Function name:		getInputRange
Purpose:			Gets range ( 0.0 < t < +1.0) from user
In parameters:		minRange, maxRange
Out parameters:		double
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
double getInputRange(double minRange, double maxRange){
	double inputRange;
	bool validInputRange = false;

	while (!validInputRange){
		cout << "Please enter the range of t to evaluate in 10 equal increments( 0.0 < t < +1.0): ";
		cin >> inputRange;

		if (inputRange < minRange || inputRange > maxRange){ /* number of terms is not valid */
			cout << endl << "Please enter a valid range" << endl;
		}
		else{ validInputRange = true; }
	}

	return inputRange;
}

/********************************************************************
Function name:		inArray
Purpose:			Helper function to check if an int is in an array. Returns true/false
In parameters:		toCheck(int to check), arr[](array to check in), s(length of that array)
Out parameters:		bool
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
bool inArray(int toCheck, int arr[], int s){
	unsigned int i, arrsize = s/sizeof(int);
	for (i = 0; i < arrsize; i++){
		if (toCheck == arr[i]) return true;
	}
	return false;
}

/********************************************************************
Function name:		printResults
Purpose:			Outputs the results from all the calculations in a table format
In parameters:		timesCalced(number of increments), tArr[](array of all the increments),
					ftSeries[](array of series values), ftExact[](array of exact values),
					rExactE[](array of %RExactE), rSerE[](array of %RSerE)
Out parameters:		void
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
void printResults(int timesCalced, double tArr[], double ftSeries[], double ftExact[], double rExactE[], double rSerE[]){
	int i;
	cout << endl << "MACLAURIN SERIES" << endl;
	cout << "  t              F(t) Series     F(t) Exact      %RExactE        %RSerE" << endl;
	for (i = 0; i < timesCalced; i++){
		cout << scientific << tArr[i] << "   " << ftSeries[i] << "   " << ftExact[i] << "   " << rExactE[i] << "   " << rSerE[i] << endl;
	}
}