/*
 *  sharedSharedOrderVector.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 12/9/08.
 *  Copyright 2008 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

using namespace std;


#include "sharedordervector.h"


/***********************************************************************/

SharedOrderVector::SharedOrderVector() : DataVector(), maxRank(0), numBins(0), numSeqs(0)  {}

/***********************************************************************/

SharedOrderVector::SharedOrderVector(string id, vector<individual>  ov) : 
											DataVector(id), data(ov)
{
		updateStats();
}

/***********************************************************************/
//This function is used to read a .shared file for the collect.shared, rarefaction.shared and summary.shared commands
//if you don't use a list and groupfile.  

SharedOrderVector::SharedOrderVector(ifstream& f) : DataVector() {
	try {
		globaldata = GlobalData::getInstance();
		maxRank = 0; numBins = 0; numSeqs = 0;
		
		if (globaldata->gGroupmap == NULL) {  groupmap = new GroupMap(); }
		
		int num, inputData, pos, count;
		count = 0;  numSeqs = 0;
		string holdLabel, nextLabel, groupN;
		individual newguy;
		
		//read in first row since you know there is at least 1 group.
		f >> label >> groupN >> num;
		holdLabel = label;
		
		if (globaldata->gGroupmap == NULL) { 
			//save group in groupmap
			groupmap->namesOfGroups.push_back(groupN);
			groupmap->groupIndex[groupN] = 0;
		}
		
		for(int i=0;i<num;i++){
			f >> inputData;
			
			for (int j = 0; j < inputData; j++) {
				push_back(i+1, i+1, groupN);
				numSeqs++;
			}
		}
		
		//save position in file in case next line is a new label.
		pos = f.tellg();
		
		if (f.eof() != true) { f >> nextLabel; }
		
		//read the rest of the groups info in
		while ((nextLabel == holdLabel) && (f.eof() != true)) {
			f >> groupN >> num;
			count++;
			
			if (globaldata->gGroupmap == NULL) { 
				//save group in groupmap
				groupmap->namesOfGroups.push_back(groupN);
				groupmap->groupIndex[groupN] = count;
			}
			
			for(int i=0;i<num;i++){
				f >> inputData;
				
				for (int j = 0; j < inputData; j++) {
					push_back(i+1, i+1, groupN);
					numSeqs++;
				}
			}
			
			//save position in file in case next line is a new label.
			pos = f.tellg();
	
			if (f.eof() != true) { f >> nextLabel; }

		}
		
		//put file pointer back since you are now at a new distance label
		f.seekg(pos, ios::beg);
	
		if (globaldata->gGroupmap == NULL) { globaldata->gGroupmap = groupmap; }
	
		updateStats();
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the SharedOrderVector class Function SharedOrderVector. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the SharedOrderVector class function SharedOrderVector. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
}
/***********************************************************************/

int SharedOrderVector::getNumBins(){
	if(needToUpdate == 1){	updateStats();	}
	return numBins;
}

/***********************************************************************/

int SharedOrderVector::getNumSeqs(){
	if(needToUpdate == 1){	updateStats();	}
	return numSeqs;
}

/***********************************************************************/

int SharedOrderVector::getMaxRank(){
	if(needToUpdate == 1){	updateStats();	}
	return maxRank;
}


/***********************************************************************/



void SharedOrderVector::set(int index, int binNumber, int abund, string groupName){
	
	data[index].group = groupName;
	data[index].bin = binNumber;
	data[index].abundance = abund;
	needToUpdate = 1;
	
}

/***********************************************************************/

individual SharedOrderVector::get(int index){
	return data[index];			
}


/***********************************************************************/

void SharedOrderVector::push_back(int binNumber, int abund, string groupName){
	individual newGuy;
	newGuy.group = groupName;
	newGuy.abundance = abund;
	newGuy.bin = binNumber;
	data.push_back(newGuy);
	needToUpdate = 1;
	
}

/***********************************************************************/

void SharedOrderVector::print(ostream& output){
	try {
		output << label << '\t' << numSeqs << '\t';
	
		for(int i=0;i<data.size();i++){
			output << data[i].bin << '\t';
		}
		output << endl;
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the SharedOrderVector class Function print. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the SharedOrderVector class function print. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
}


/***********************************************************************/

void SharedOrderVector::resize(int){
	cout << "resize() did nothing in class SharedOrderVector";
}

/***********************************************************************/


vector<individual>::iterator SharedOrderVector::begin(){
	return data.begin();	
}

/***********************************************************************/

vector<individual>::iterator SharedOrderVector::end(){
	return data.end();		
}

/***********************************************************************/

int SharedOrderVector::size(){
	return data.size();					
}

/***********************************************************************/

RAbundVector SharedOrderVector::getRAbundVector(){
	try {
		RAbundVector rav(data.size());
	
		for(int i=0;i<numSeqs;i++){
			rav.set(data[i].bin, rav.get(data[i].bin) + 1);
		}	
		sort(rav.rbegin(), rav.rend());
		for(int i=numSeqs-1;i>=0;i--){
			if(rav.get(i) == 0){	rav.pop_back();	}
			else{
				break;
			}
		}
		rav.setLabel(label);

		return rav;
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the SharedOrderVector class Function getRAbundVector. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the SharedOrderVector class function getRAbundVector. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}	
}
/***********************************************************************/

OrderVector SharedOrderVector::getOrderVector(map<string,int>* nameMap = NULL) {
	try {
		OrderVector ov;
	
		for (int i = 0; i < data.size(); i++) {
			ov.push_back(data[i].bin);
		}
		
		random_shuffle(ov.begin(), ov.end());

		ov.setLabel(label);	
		return ov;
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the SharedOrderVector class Function getOrderVector. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the SharedOrderVector class function getOrderVector. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
}


/***********************************************************************/

SAbundVector SharedOrderVector::getSAbundVector(){
	
	RAbundVector rav(this->getRAbundVector());
	return rav.getSAbundVector();

}
/***********************************************************************/
SharedRAbundVector SharedOrderVector::getSharedRAbundVector(string group) {
	try {
		SharedRAbundVector sharedRav(data.size());
		
		sharedRav.setLabel(label);
		sharedRav.setGroup(group);
		
		for (int i = 0; i < data.size(); i++) {
			if (data[i].group == group) {
				sharedRav.set(data[i].abundance, sharedRav.getAbundance(data[i].abundance) + 1, data[i].group);
			}
		}
		return sharedRav;
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the SharedOrderVector class Function getSharedRAbundVector. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the SharedOrderVector class function getSharedRAbundVector. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	
}
/***********************************************************************/
SharedSAbundVector SharedOrderVector::getSharedSAbundVector(string group) {
	try {
		
		SharedRAbundVector sharedRav(this->getSharedRAbundVector(group));
		return sharedRav.getSharedSAbundVector();
				
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the SharedOrderVector class Function getSharedRAbundVector. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the SharedOrderVector class function getSharedRAbundVector. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	
}

/***********************************************************************/

SharedOrderVector SharedOrderVector::getSharedOrderVector(){
	return *this;			
}

/***********************************************************************/

void SharedOrderVector::updateStats(){
	try {
		needToUpdate = 0;
		numSeqs = 0;
		numBins = 0;
		maxRank = 0;
	
		for(int i=0;i<data.size();i++){
			if(data[i].bin != -1){
				numSeqs++;
			}
		}
	
		vector<individual> hold(numSeqs);
	
		for(int i=0;i<numSeqs;i++){
			if(data[i].bin != -1){
				hold[data[i].bin].bin = hold[data[i].bin].bin+1;
			}
		}	

		for(int i=0;i<numSeqs;i++){
			if(data[i].bin > numBins) { numBins = data[i].bin;	} 
			if(data[i].abundance > maxRank)	{	maxRank = data[i].abundance;	}
		}
		numBins++; //if you have 10 bins largest .bin is 9 since we start at 0.
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the SharedOrderVector class Function updateStats. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the SharedOrderVector class function updateStats. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}	
}

/***********************************************************************/


