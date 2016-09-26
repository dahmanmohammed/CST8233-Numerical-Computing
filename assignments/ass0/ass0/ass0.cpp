/**********************************************************************
Filename:			ass0.cpp
Version: 			1.0
Author:				Lucas Estienne
Student No:  		040 819 959
Course Name/Number:	Numerical Computing CST8233 / C++ Programming CST8219
Lab Sect: 			300
Assignment #:		0
Assignment name:	Vector Graphics
Due Date:			September 25 2016
Submission Date:	September 25 2016
Professor:			Andrew Tyler
Purpose:			Until the user quits, it reads a validated radius from keyboard stores a number of Graphic Elements into a Vector Graphics
					and display the raw data.
*********************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC

enum{ RUNNING = 1 };

struct Point
{
	int x, y;
};

struct Line
{
	Point start;
	Point end;
};

struct GraphicElement
{
	enum{ SIZE = 256 };
	unsigned int numLines;
	Line* pLines;
	char name[SIZE];
};

typedef struct
{
	unsigned int numGraphicElements;
	GraphicElement* pElements;
}VectorGraphic;

void InitVectorGraphic(VectorGraphic*);
void AddGraphicElement(VectorGraphic*);
void ReportVectorGraphic(VectorGraphic*);
void CleanUpVectorGraphic(VectorGraphic*);

VectorGraphic Image;



/********************************************************************
Function name:		main
Purpose:			main function for Assignment 0, calls assignment functions based on user input
In parameters:		none
Out parameters:		int 0 for successful execution
Version:			1.0
Author:				Prof Andrew Tyler
**********************************************************************/
int main()
{
	char response;
	InitVectorGraphic(&Image);

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	while (RUNNING)
	{
		printf("\nPlease select an option:\n");
		printf("1. Add a Graphic Element\n");
		printf("2. List the Graphic Elements\n");
		printf("q. Quit\n");
		printf("CHOICE: ");
		fflush(stdin);
		scanf("%c", &response);

		switch (response)
		{
		case '1':AddGraphicElement(&Image); break;
		case '2':ReportVectorGraphic(&Image); break;
		case 'q':CleanUpVectorGraphic(&Image); return 0;
		default:printf("Please enter a valid option\n");
		}
		printf("\n");
	}
}

/********************************************************************
Function name:		InitVectorGraphic
Purpose:			Initializes the pElements member of a VectorGraphic struct
In parameters:		VectorGraphic * pImage
Out parameters:		void
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
void InitVectorGraphic(VectorGraphic* pImage){
	pImage->pElements = (GraphicElement*)malloc(pImage->numGraphicElements * sizeof(GraphicElement));
}

/********************************************************************
Function name:		AddGraphicElement
Purpose:			Adds a graphic element to a VectorGraphic struct.
						Prompts the user for a GraphicElement name, and validates it. Must be between 1 and 255 
						inclusive, as to leave space for the string termination char '\0'. Then prompts the user
						for a number of lines in the element (must be > 0), and the coordinates of the points in
						the line. The function then appends the newly created GraphicElement to the passed
						VectorGraphic struct.
In parameters:		VectorGraphic * pImage
Out parameters:		void
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
void AddGraphicElement(VectorGraphic* pImage){
	GraphicElement newGraphicElement;
	VectorGraphic vectorBuffer;
	char inputBuffer[1024];
	int isValidName = 0, isValidNumLines = 0, isValidCoords = 0;
	int numLines, xCoord, yCoord, i;
	unsigned int j;

	printf("\nAdding A Graphic Element\n");

	while (!isValidName){
		printf("Please enter the name of the new GraphicElement(<256 characters): ");
		fflush(stdin);
		scanf("%s", &inputBuffer);

		if (strlen(inputBuffer) > 255 || strlen(inputBuffer) < 1) { // Leave 1 char to append line termination '\0'.
			printf("Invalid name length.\n");
		} else { isValidName = true;}

	}

	while (!isValidNumLines){
		printf("How many lines are there in the new GraphicElement? ");
		fflush(stdin);
		scanf("%d", &numLines); /* scanf will discard non digit garbage here */

		if (numLines < 1) { /* empty, -1 or input contains no integers*/
			printf("\nInvalid input, please input an integer number.");
		} else { isValidNumLines = true; }

	}

	newGraphicElement.numLines = numLines;
	newGraphicElement.pLines = (Line*)malloc(newGraphicElement.numLines * sizeof(Line));

	for (i = 0; i < numLines; i++) {

		printf("Please enter the x coord of the start point of line index %d: ", i);
		fflush(stdin);
		scanf("%d", &xCoord); /*scanf will discard non digit garbage here */

		printf("Please enter the y coord of the start point of line index %d: ", i);
		fflush(stdin);
		scanf("%d", &yCoord); /*scanf will discard non digit garbage here */

		(newGraphicElement.pLines[i]).start.x = xCoord;
		(newGraphicElement.pLines[i]).start.y = yCoord;

		printf("Please enter the x coord of the end point of line index %d: ", i);
		fflush(stdin);
		scanf("%d", &xCoord); /*scanf will discard non digit garbage here */

		printf("Please enter the y coord of the end point of line index %d: ", i);
		fflush(stdin);
		scanf("%d", &yCoord); /*scanf will discard non digit garbage here */

		(newGraphicElement.pLines[i]).end.x = xCoord;
		(newGraphicElement.pLines[i]).end.y = yCoord;

	}

	strcpy(newGraphicElement.name, inputBuffer); /* copies input buffer to newly initialized GraphicElement if it's valid */
	vectorBuffer.numGraphicElements = (pImage->numGraphicElements > 0) ? pImage->numGraphicElements : 0;

	InitVectorGraphic(&vectorBuffer); /* initializes vectorBuffer to hold old pImage GraphicElements*/

	for (j = 0; j < vectorBuffer.numGraphicElements; j++) { /* copy pImage to buffer so we can reinit pImage */
		memcpy(&vectorBuffer.pElements[j], &pImage->pElements[j], sizeof(GraphicElement));
	}

	

	free(pImage->pElements); /* frees the pImage pElements but NOT the pLines inside the elements */

	pImage->numGraphicElements += 1; /* increase the number of graphic elements*/
	InitVectorGraphic(pImage); /* and reallocate it with new size*/

	for (j = 0; j < vectorBuffer.numGraphicElements; j++) {
		memcpy(&pImage->pElements[j], &vectorBuffer.pElements[j], sizeof(GraphicElement)); /* copy back to pImage*/
	}

	free(vectorBuffer.pElements); /* frees the vectorBuffer pElements */

	memcpy(&pImage->pElements[j], &newGraphicElement, sizeof(GraphicElement)); /* add new element */
}

/********************************************************************
Function name:		ReportVectorGraphic
Purpose:			Uses printf() to report the GraphicElement objects inside a VectorGraphic
					struct in a user-readable format. If no GraphicElements are inside the struct,
					the function reports that there are no elements to be displayed.
In parameters:		VectorGraphic * pImage
Out parameters:		void
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
void ReportVectorGraphic(VectorGraphic* pImage){
	unsigned int i, j;
	printf("\nVectorGraphic Report\n");
	if (pImage->numGraphicElements < 1){ /* are the GraphicElements in the array?*/
		printf("No Graphic Elements to report.");
		return;
	}
	for (i = 0; i < pImage->numGraphicElements; i++){
		printf("Reporting element #%d\n", i);
		printf("Graphic Element name: %s\n", (pImage->pElements[i]).name);
		for (j = 0; j < (pImage->pElements[i]).numLines; j++){
			printf("Line #%d start x: %d\n", j, pImage->pElements[i].pLines[j].start.x);
			printf("Line #%d start y: %d\n", j, pImage->pElements[i].pLines[j].start.y);
			printf("Line #%d end x: %d\n", j, pImage->pElements[i].pLines[j].end.x);
			printf("Line #%d end y: %d\n", j, pImage->pElements[i].pLines[j].end.y);
		}

	}

}

/********************************************************************
Function name:		CleanUpVectorGraphic
Purpose:			Frees the dynamically allocated memory inside a passed VectorGraphic struct
					by first freeing the lines inside the pElements GraphicElement array and then freeing
					the pElements array itself.
In parameters:		VectorGraphic * pImage
Out parameters:		void
Version:			1.0
Author:				Lucas Estienne
**********************************************************************/
void CleanUpVectorGraphic(VectorGraphic* pImage){
	Line* tl;
	unsigned int i;

	for (i = 0; i < pImage->numGraphicElements; i++){ /* frees every pLines line array in the GraphicElements */
		tl = pImage->pElements[i].pLines;
		free(tl);
		tl = NULL; /* dangling pointer */
	}
	if (pImage->pElements) free(pImage->pElements); /* frees pElements */
	pImage->pElements = NULL; /* dangling pointer */
	pImage = NULL; /* dangling pointer */
}