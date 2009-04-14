/*
 *  geom.cpp
 *  Mothur
 *
 *  Created by Thomas Ryabin on 2/23/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "geom.h"

/***********************************************************************/

double Geom::kEq(double k, double spec){
	return k/(1-k)*pow(1-k, spec)/(1-pow(1-k, spec));
}

RAbundVector Geom::getRAbundVector(SAbundVector* rank){
		vector <int> rData;
		int mr = 1;
		int nb = 0;
		int ns = 0;
		
		for(int i = rank->size()-1; i > 0; i--)
		{
			int cur = rank->get(i);
			if(mr == 1 && cur > 0)
				mr = i;
			nb += cur;
			ns += i*cur;
			for(int j = 0; j < cur; j++)
				rData.push_back(i);
		}
		
		RAbundVector rav = RAbundVector(rData, mr, nb, ns);
		rav.setLabel(rank->getLabel());
		return rav;
}

/***********************************************************************************/

/***********************************************************************************/
EstOutput Geom::getValues(SAbundVector* rank){
	try {
		data.resize(2,0);
		
		rdata = getRAbundVector(rank);
		int numInd = rdata.getNumSeqs();
		int numSpec = rdata.getNumBins();
		int min = rdata.get(rdata.size()-1);
		double k = .5;
		double step = .49999;
		
		while(fabs(min - numInd*kEq(k, (double)numSpec)) > .0001) //This uses a binary search to find the value of k.
		{
			if(numInd*kEq(k, numSpec) > min)
				k += step;
			else
				k -= step;
			step /= 2;
		}
		
		double cK = 1/(1-pow(1-k, numSpec));
		double sumExp = 0;
		double sumObs = 0;
		double maxDiff = 0;

		for(int i = 0; i < rdata.size(); i++)
		{
			sumObs += rdata.get(i);
			sumExp += numInd*cK*k*pow(1-k, i);
			double diff = fabs(sumObs-sumExp);
			if(diff > maxDiff)
				maxDiff = diff;
		}

		double DStatistic = maxDiff/numInd;
		double critVal = 0;
		/*cout << "Geom:\n";
		cout << "D-Statistic = " << DStatistic << "\n";
		cout << "Critical value for 95% confidence interval = ";*/
		if(rdata.size() > 20)
		{
			critVal = .886/sqrt(rdata.size());
		}
		else
		{
			KOSTable table;
			critVal = table.getConfLimit(numSpec);
		}
		/*cout << critVal << "\n";
		cout << "If D-Statistic is less than the critical value then the data fits the Geometric Series model w/ 95% confidence.\n\n";*/
		
		data[0] = DStatistic;
		data[1] = critVal;
		
		if (isnan(data[0]) || isinf(data[0])) { data[0] = 0; }
		if (isnan(data[1]) || isinf(data[1])) { data[1] = 0; }
		
		return data;
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the NPShannon class Function getValues. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the NPShannon class function getValues. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}	
}

/***********************************************************************/







