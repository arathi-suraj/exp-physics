#include <formatio.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "Lab 2.h"
#include <NIDAQmx.h>
#include <stdio.h>
#include <DAQmxIOctrl.h>
#include <analysis.h>

static int panelHandle;
static double start_time;
static double stop_time;
static double sample_rate;
static TaskHandle task_handle;
static int num_samps;
static double volt_values[9000];
static int32 samps_read_real;
static int firstplothandle;
static int secplothandle;
static double timearray[9000];
double *for_fft;
double *freq_array;
double nyquist;

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "Lab 2.uir", PANEL)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}

int CVICALLBACK Exit (int panel, int control, int event,
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

int CVICALLBACK AcquireData (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panelHandle, PANEL_NUMERIC, &start_time);
			GetCtrlVal(panelHandle, PANEL_NUMERIC_2, &stop_time);
			GetCtrlVal(panelHandle, PANEL_NUMERIC_3, &sample_rate);
			num_samps = (stop_time - start_time) * sample_rate;
			DAQmxCreateTask("volt_grab", &task_handle);
			DAQmxCreateAIVoltageChan(task_handle, "Dev1/ai0", "", DAQmx_Val_Diff, -10.0, +10.0, DAQmx_Val_Volts, NULL);
			DAQmxCfgSampClkTiming(task_handle, "OnboardClock", sample_rate, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, num_samps);
			DAQmxStartTask(task_handle);
			DAQmxReadAnalogF64 (task_handle, DAQmx_Val_Auto, -1, DAQmx_Val_GroupByChannel, volt_values, num_samps, &samps_read_real, 0);
			DAQmxStopTask(task_handle);
			DAQmxClearTask(task_handle);
			int number;
			double time=0;
			
			for(number=0; number<=num_samps; number++){
				time = time + 1/sample_rate;
				timearray[number] = time;
			}
			
		
			firstplothandle = PlotXY (panelHandle, PANEL_GRAPH, timearray, volt_values, 1000, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
			break;
	}
	return 0;
}

int CVICALLBACK F_it (int panel, int control, int event,
					  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//double for_fft[num_samps];
			//double freq_array[num_samps];
			for_fft = (double*)malloc(num_samps*(sizeof(double)));
			nyquist = 0.5*sample_rate;
			freq_array = (double*)malloc(num_samps*(sizeof(double)));
			
			int num;
			for(num=0;num<num_samps;num++){
				for_fft[num] = volt_values[num];
			}
			Spectrum(for_fft, num_samps);
			int num2;
			double freq=0;
			double increment;
			increment = sample_rate/num_samps;
			for(num2=0; num2<num_samps;num2++){
				freq_array[num2] = freq;
				freq = freq + increment;
			}
			
			secplothandle = PlotXY(panelHandle, PANEL_GRAPH_2, freq_array, for_fft, num_samps, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
			break;
	}
	return 0;
}

int CVICALLBACK save_file (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:			
			char filenm[1000];
			FileSelectPopup ("C:\\Users\\Public\\Arathi Suraj & Bijin Joseph", "*.*", "", "", VAL_OK_BUTTON, 0, 0, 1, 0, filenm);
			double maindataarray[num_samps*4];
			int elem;
			for(elem=0; elem<num_samps; elem++){
				maindataarray[elem] = timearray[elem];
				maindataarray[elem+num_samps] = volt_values[elem];
				maindataarray[elem+(2*num_samps)] = freq_array[elem];
				maindataarray[elem+(3*num_samps)] = for_fft[elem];
			}
			ArrayToFile (filenm, maindataarray, VAL_DOUBLE, num_samps*4, 4, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_CONST_WIDTH, 10, VAL_ASCII, VAL_TRUNCATE);
			break;
			
			break;
	}
	return 0;
}
