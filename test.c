#include <formatio.h>
#include <cvirte.h>		
#include <userint.h>
#include "test.h"
#include <stdio.h>
#include <stdlib.h>

static int panelHandle;
static char FileName[300];
static double FileData[1000];
static double Xarray[1000];
static double Yarray[1000];
static int flag;
static int firstplothandle;
static double mainarea;

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "test.uir", PANEL)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}

int CVICALLBACK Bye (int panel, int control, int event,
					 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK fileSelectPopup (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			FileSelectPopup ("C:\\Users\\Public\\Arathi Suraj & Bijin Joseph", "*.txt", "", "", VAL_LOAD_BUTTON, 0, 0, 1, 0, FileName);
			//printf(FileName);
			FILE *FilePoint;
			int y;
			float z;
			flag=0;

		
			FilePoint = fopen(FileName, "r");
			while( fscanf(FilePoint, "%d\t%f", &y, &z) > 0){
				flag++;
			}
			fclose(FilePoint);
			
			
			FileToArray (FileName, FileData, VAL_DOUBLE, flag*2, 2, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			int elem;
			for(elem = 0; elem<flag; elem++){
				Xarray[elem] = FileData[elem];
				//printf("%f\t", Xarray[elem]);
				Yarray[elem] = FileData[elem+flag];
				//printf("%f\n", Yarray[elem]);
			}
			//printf(flag);
			
			//PlotXY (panelHandle, PANEL_GRAPH, Xarray, Yarray, flag, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
			firstplothandle = PlotXY (panelHandle, PANEL_GRAPH, Xarray, Yarray, flag, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
			break;
	}
	return 0;
}

int CVICALLBACK subtractbaseline (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			double start;
			double end;
			double baseline;
			GetCtrlVal(panelHandle, PANEL_NUMERIC, &start);
			GetCtrlVal(panelHandle, PANEL_NUMERIC_3, &end);
			int elem;
			int sum=0;
			for(elem=start; elem<end; elem++){
				sum = sum + Yarray[elem];
			}
			double avg;
			avg = sum/(end-start);
			baseline = avg;
			int elem2;
			for(elem2=0; elem2<flag; elem2++){
				Yarray[elem2] = Yarray[elem2] - baseline;
			}
			SetCtrlVal(panelHandle, PANEL_NUMERIC_4, baseline);
			DeleteGraphPlot (panelHandle, PANEL_GRAPH, firstplothandle, VAL_IMMEDIATE_DRAW);
			firstplothandle = PlotXY (panelHandle, PANEL_GRAPH, Xarray, Yarray, flag, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
			break;
	}
	return 0;
}

/*int CVICALLBACK add_one (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			int baseline;
			GetCtrlVal (panelHandle, PANEL_NUMERIC, &baseline);
			x = x + 1;
			SetCtrlVal (panelHandle, PANEL_NUMERIC, x);
			break;
	}
	return 0;
}*/

int CVICALLBACK normalizegraph (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			int elem;
			float area=0;
			for(elem=0; elem<flag-1; elem++){
				area = area + (Yarray[elem] * 1);
			}
			mainarea = area;
			SetCtrlVal(panelHandle, PANEL_NUMERIC_2, area);
			DeleteGraphPlot (panelHandle, PANEL_GRAPH, firstplothandle, VAL_IMMEDIATE_DRAW);
			int elem2;
			if (area == 0){
			printf("Cannot plot because area is 0.");}
			
			if (area != 0){
			for(elem2=0; elem2<flag; elem2++){
				Yarray[elem2] = Yarray[elem2]/area;
			}}
			//DeleteGraphPlot (panelHandle, PANEL_GRAPH, firstplothandle, VAL_IMMEDIATE_DRAW);
			firstplothandle = PlotXY (panelHandle, PANEL_GRAPH, Xarray, Yarray, flag, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
			break;
	}
	return 0;
}


int CVICALLBACK findcentroid (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			float area=0;
			int elem;
			for(elem=0; elem<flag-1; elem++){
				area = area + (Yarray[elem] * 1);
				if (area >= 0.5){

					SetCtrlVal(panelHandle, PANEL_NUMERIC_5, elem);
					break;
				}
			}
			SetCtrlVal(panelHandle, PANEL_NUMERIC_2, area);
			break;
	}
	return 0;
}

int CVICALLBACK savetofile (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			char filenm[1000];
			FileSelectPopup ("C:\\Users\\Public\\Arathi Suraj & Bijin Joseph", "*.*", "", "", VAL_OK_BUTTON, 0, 0, 1, 0, filenm);
			double mainarray[flag*2];
			int elem;
			for(elem=0; elem<flag; elem++){
				mainarray[elem] = Xarray[elem];
				mainarray[elem+flag] = Yarray[elem];
			}
			ArrayToFile (filenm, mainarray, VAL_DOUBLE, flag*2, 2, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_CONST_WIDTH, 10, VAL_ASCII, VAL_TRUNCATE);
			break;
	}
	return 0;
}
