/*
 * File: plotmaker.cc
 * Author: Achilleas Moustakis
 * Copyright (c) 2016 National and Kapodistrian University of Athens, Greece
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/config-store.h"
#include <ns3/buildings-module.h>
#include <ns3/netanim-module.h>
#include "ns3/gnuplot.h"

#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace ns3;
using namespace std;

Gnuplot2dDataset** CalculateThroughputperNode(string statsFileName, uint32_t nUe)
{
/*The stats file we are using contains the start/end time in columns 1/2,
 * the IMSI in column 4 and Bytes Received in column 10. Therefore, we
 * extract the according tokens from every line of the file.*/

	uint32_t num;

	//Create an array of datasets (one for each node) and hold a pointer of the array (DYNAMIC ALLOCATION)
	Gnuplot2dDataset** datasetArray = new Gnuplot2dDataset*[nUe];
	for (num = 0; num < nUe; num++)
		datasetArray[num] = new Gnuplot2dDataset();

	//Read stats file line by line and tokenize
	string line;
	ifstream myfile(statsFileName.c_str());
	if (myfile)
	{
		getline(myfile,line);
		vector<string> tokens;
		while (getline(myfile,line)) {
			string delimiter = "	";
			size_t pos = 0;
			string token;
			while ((pos = line.find(delimiter)) != string::npos) {
				token = line.substr(0, pos);
				tokens.push_back(token);
				line.erase(0, pos + delimiter.length());
			}
			datasetArray[atoi(tokens.at(3).c_str())-1]->Add(atof(tokens.at(1).c_str()),
			  atof(tokens.at(9).c_str())*8/1000/(atof(tokens.at(1).c_str())-atof(tokens.at(0).c_str()))); //(End time, Kbps)
			tokens.clear();
		}
		myfile.close();
	}
	return datasetArray;
}

Gnuplot2dDataset** CalculateSINRperNode(string statsFileName, uint32_t nUe)
{
/*The stats file we are using contains the measurement timestamp in column 1,
 * the IMSI in column 3 and the SINR in column 6. Therefore, we
 * extract the according tokens from every line of the file.*/

	uint32_t num;

	//Create an array of datasets (one for each node) and hold a pointer of the array (DYNAMIC ALLOCATION)
	Gnuplot2dDataset** datasetArray = new Gnuplot2dDataset*[nUe];
	for (num = 0; num < nUe; num++)
		datasetArray[num] = new Gnuplot2dDataset();

	//Read stats file line by line and tokenize
	string line;
	ifstream myfile(statsFileName.c_str());
	if (myfile)
	{
		getline(myfile,line);
		vector<string> tokens;
		while (getline(myfile,line)) {
			string delimiter = "	";
			size_t pos = 0;
			string token;
			while ((pos = line.find(delimiter)) != string::npos) {
				token = line.substr(0, pos);
				tokens.push_back(token);
				line.erase(0, pos + delimiter.length());
			}
			tokens.push_back(line);
			line.clear();
			datasetArray[atoi(tokens.at(2).c_str())-1]->Add(atof(tokens.at(0).c_str()), atof(tokens.at(5).c_str())); //(timestamp, SINR)
			tokens.clear();
		}
		myfile.close();
	}
	return datasetArray;
}

void Create2DPlotFile (string fileName, double simTime, string axisXname, string axisYname, Gnuplot2dDataset dataset)
{
  std::string fileNameWithNoExtension = fileName;
  std::string graphicsFileName        = fileNameWithNoExtension + ".png";
  std::string plotFileName            = fileNameWithNoExtension + ".plt";
  std::string plotTitle               = fileName;
  std::string dataTitle               = fileName;

  // Instantiate the plot and set its title.
  Gnuplot plot (graphicsFileName);
  plot.SetTitle (plotTitle);

  // Make the graphics file, which the plot file will create when it
  // is used with Gnuplot, be a PNG file.
  plot.SetTerminal ("png");

  // Set the labels for each axis.
  plot.SetLegend (axisXname, axisYname);

  // Set the range for the x axis
  std::stringstream sstm;
  sstm << simTime;
  plot.AppendExtra ("set xrange [0:" + sstm.str() + "]");

  // Instantiate the dataset, set its title, and make the points be
  // plotted along with connecting lines.
  dataset.SetTitle (dataTitle);
  dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

  // Add the dataset to the plot.
  plot.AddDataset (dataset);

  // Open the plot file.
  std::ofstream plotFile (plotFileName.c_str());

  // Write the plot file.
  plot.GenerateOutput (plotFile);
  system(("gnuplot < " + plotFileName).c_str());

  // Close the plot file.
  plotFile.close ();
}

void PlotStatistics(uint32_t nUe, double simTime){

	//Create throughput datasets for every node and plot statistics
	Gnuplot2dDataset** datasets = CalculateThroughputperNode("DlRlcStats.txt", nUe);
	string plotName;
	uint32_t num;
	for (num = 0; num < nUe; num++)
	{
		std::stringstream sstm;
		sstm << "Throughput-" << num+1;
		plotName = sstm.str();
		Create2DPlotFile(plotName, simTime, "Seconds", "Kbps", *datasets[num]);
		plotName.clear();
		delete datasets[num];
	}
	delete datasets;

	//Create SINR datasets for every node and plot statistics
	datasets = CalculateSINRperNode("DlRsrpSinrStats.txt", nUe);
	for (num = 0; num < nUe; num++)
	{
		std::stringstream sstm;
		sstm << "SINR-" << num+1;
		plotName = sstm.str();
		Create2DPlotFile(plotName, simTime, "Seconds", "Linear SINR", *datasets[num]);
		plotName.clear();
		delete datasets[num];
	}
delete datasets;
}

int main (int argc, char *argv[])
{
/*In order to run properly, please provide the number of User Equipments (nUe)
 * and the simulation time (simTime) before execution.*/

	uint32_t nUe = 5;
	double simTime = 1.0;
	PlotStatistics(nUe, simTime);
}
