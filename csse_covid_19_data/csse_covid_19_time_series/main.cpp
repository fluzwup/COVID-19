#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>

int main(int argc, char **argv)
{
	try
	{
		const int bufsize = 4096;
		char buffer[bufsize];
		FILE *fp = fopen(argv[1], "r");
		if(fp == NULL) throw "Could not open input file.";
	
		std::vector<std::string> location;
		std::vector<std::vector<int> > cases;
		int index = -1;
		while(!feof(fp))
		{
			if(NULL == fgets(buffer, bufsize, fp)) break;
			// skip header line
			if(index < 0) 
			{
				++index;
				continue;	
			}

			// in a CSV file, commas in a quoted string don't count; replace all
			//  other commas with vertical bars, so we can use vertical bars to delimit
			int buflen = strlen(buffer);
			bool inquotes = false;
			for(int i = 0; i < buflen; ++i)
			{
				if(buffer[i] == '"')
				{
					inquotes = !inquotes;
					buffer[i] = ' ';
					continue;
				}

				if(buffer[i] == ',')
				{
					if(!inquotes)
						buffer[i] = '|';
				}
			}
			if(inquotes) printf("Warning!  Unmatched quote found on line %i:  %s\n", 
					index, buffer);

			const char *tmp;
			std::string loc;	// build up the location from the first two fields

			tmp = strtok(buffer, "|");

			// if first character is a comma, tmp points to second column
			if(buffer[0] == '|')
			{
				loc = tmp; // country is all we have
			}
			else
			{
				loc = tmp;
				// get the next column
				tmp = strtok(NULL, "|");
				// put region first, then comma and country
				loc = std::string(tmp) + ", " + loc;
			}

			// trim spaces from ends of name (may be from removal of quotes)
			while(loc[0] == ' ') loc = loc.substr(1);
			while(loc.back() == ' ') loc = loc.substr(0, loc.length() - 1);

			location.push_back(loc);
			// add location, allocate space for new cases vector
			cases.resize(index + 1);

			// skip lat and long
			strtok(NULL, "|");
			strtok(NULL, "|");

			// now loop througth all the columns of cumulative cases
			while(true)
			{
				tmp = strtok(NULL, "|\n");
				if(tmp == NULL) break;
				cases[index].push_back(atoi(tmp));
			}

			printf("Line %i final count for location %s:  %i\n", 
					index, location[index].c_str(), cases[index].back());

			++index;
		}
		fclose(fp);

		// calculate the first derivative by taking the difference between
		//  successive days; express in percentage increase
		std::vector<std::vector<float> > der1;
		der1.resize(cases.size());
		for(int i = cases.size() - 1; i >= 0; --i)
		{
			der1[i].resize(cases[i].size());
			for(int j = cases[i].size() - 1; j >= 1; --j)
			{
				// ignore until we have at least 10 cases, otherwise it's too noisy
				if(cases[i][j - 1] > 10)
					der1[i][j] = 100.0 * (float)(cases[i][j] - cases[i][j - 1]) /
						(float)cases[i][j - 1];
				else
					der1[i][j] = 0;
			}
		}

		fp = fopen("FirstDerivative.csv", "w");
		for(int i = 0; i < der1.size(); ++i)
		{
			fprintf(fp, "\"%s\",", location[i].c_str());
			for(int j = 0; j < der1[i].size(); ++j)
			{
				fprintf(fp, "%10.2f,", der1[i][j]);
			}
			fprintf(fp, "\n");
		}

		// now calculate the weighted average of the 1st derivative using a 20% decay function
		std::vector<std::vector<float> > decayAvg;
		decayAvg.resize(der1.size());
		for(int i = 0; i < der1.size(); ++i)
		{
			decayAvg[i].resize(der1[i].size());
			// start with an average of zero
			float avg = 0.0;
			for(int j = 0; j < der1[i].size(); ++j)
			{
				avg = 0.8 * avg + 0.2 * der1[i][j];
				decayAvg[i][j] = avg;
			}
		}

		fp = fopen("DecayingAverage.csv", "w");
		fprintf(fp, "Location, current cases, day ");
		for(int i = 0; i < decayAvg[0].size(); ++i)
			fprintf(fp, "%i,", i);
		fprintf(fp, "\n");
		for(int i = 0; i < decayAvg.size(); ++i)
		{
			fprintf(fp, "\"%s\",%i,", location[i].c_str(), cases[i].back());
			for(int j = 0; j < decayAvg[i].size(); ++j)
			{
				fprintf(fp, "%10.2f,", decayAvg[i][j]);
			}
			fprintf(fp, "\n");
		}

	}
	catch(const char *err)
	{
		printf("Caught exception %s\n", err);
	}
	catch(std::exception &e)
	{
		printf("Caught STL exception %s\n", e.what());
	}
	catch(...)
	{
		printf("Caught unknown exception.\n");
	}
	return 0;
}

