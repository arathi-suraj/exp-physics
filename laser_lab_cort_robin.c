#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include <formatio.h>
#include "laser_lab_cort_robin.h"
#include "tktds2xx.h"


static int plothandle;
static ViInt16 arrayreadbuffer[10800];
int arrayBuffer[10800];
static ViInt16 numbytesread[256];
static ViChar preamble[512];
static ViInt32 bytesread;
static ViChar readbuffer[10800];
static ViSession instrumenthandle;
static int data[2500];

static int panelHandle,i;

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "laser_lab_cort_robin.uir", PANEL)) < 0)
		return -1;
		tktds2xx_init ("GPIB0::1", VI_ON, VI_OFF, 9600, 0, 20000, &instrumenthandle);
	DisplayPanel (panelHandle);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}

int CVICALLBACK quit_program (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			QuitUserInterface (0);
			break;
	}
	return 0;
}


int CVICALLBACK datafromscope (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
	   
			 tktds2xx_readInstrData (instrumenthandle, 50, readbuffer, &bytesread);
			 tktds2xx_readWfmFile (instrumenthandle, 0, 1, 2500, "c:\\anyfile.wav");
			 tktds2xx_readInstrSettings (instrumenthandle, 4, readbuffer);
			 tktds2xx_readWfmArray (instrumenthandle, 0, 1, 2500, preamble, arrayreadbuffer, numbytesread);
			//printf("%s\n",preamble);
			
			   for (i=0;i<2500;i++){
			     data[i]= arrayreadbuffer[i]*500/256; }
			     //printf("%dd\n",arrayreadbuffer[i]);   }
				 if (plothandle){DeleteGraphPlot (panelHandle, PANEL_GRAPH, plothandle,
								  VAL_IMMEDIATE_DRAW);  }
			     
				 plothandle = PlotWaveform (panelHandle, PANEL_GRAPH, data, 2500, VAL_INTEGER, 1.0, 0.0, 0.0, 1.0, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
			

			break;
	}
	return 0;
}



int CVICALLBACK savedata (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			char filenm[1000];
			FileSelectPopup("C:\\Users\\Public\\Cort and Robin", "*.*", "", "", VAL_OK_BUTTON, 0, 0, 1, 0, filenm);
			ArrayToFile (filenm, data, VAL_INTEGER, 2500, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_CONST_WIDTH, 10, VAL_ASCII, VAL_TRUNCATE);
			break;
	}
	return 0;
}
