// compile with: g++ downloadAndParse5.cpp -o matchParse.cgi -Llibboost_iostreams.so.1.49.0 -lcurl

// currently working, saving state

#include "precompileTest.hpp"
#include <string>
#include <set>
#include <map>
#include <exception>
#include <iostream>
#include <iterator>
using std::ostream;
using std::ios;
using std::cout;
using std::endl;
using std::iterator;

#include <iomanip>
#include <curl/curl.h>
#include <curl/easy.h>
#include <fstream>



struct playerData
{

};


struct dotaMatch
{	
	/**
 	 * string to hold the winner
 	 */
	std::string dm_winner;
	/**
 	 * set of strings to hold player ids
 	 */
	std::set<std::string> dm_playerID; 

	std::string dm_duration;

	std::string dm_matchID;



};

struct player
{
	std::string account_id;
	std::string player_slot;
	std::string heroID;
};

struct match
{
	std::vector<player> players;
	std::string matchID;
	std::string startTime;
	std::string match_seq_num;
};

struct playersMatches
{
	std::vector<match> matchList;
	void load(const std::string &filename, ostream &out);
	std::string matchesLeft;
	std::string currentOldestMatch;
};


void playersMatches::load(const std::string &filename, ostream &out)
{
	using boost::property_tree::ptree;
	ptree pt;

	read_json(filename, pt);
		
	
	BOOST_FOREACH (boost::property_tree::ptree::value_type& result, pt)
	{
		matchesLeft = result.second.get<std::string>("results_remaining");
		cout << "there are " << matchesLeft << " matches left" << endl;
		BOOST_FOREACH (boost::property_tree::ptree::value_type& matches, result.second.get_child("matches"))
		{
			match tempMatch;
			tempMatch.startTime = matches.second.get<std::string>("start_time");
			tempMatch.matchID = matches.second.get<std::string>("match_id");
			tempMatch.match_seq_num = matches.second.get<std::string>("match_seq_num");

			BOOST_FOREACH (boost::property_tree::ptree::value_type& players, matches.second.get_child("players"))
			{
				player tempPlayer;

				tempPlayer.account_id = players.second.get<std::string>("account_id");
				tempPlayer.player_slot = players.second.get<std::string>("player_slot");
				tempPlayer.heroID = players.second.get<std::string>("hero_id");
				tempMatch.players.push_back(tempPlayer);
				out << tempMatch.matchID << "," << tempMatch.startTime << "," << tempMatch.match_seq_num << "," << tempPlayer.account_id << "," << tempPlayer.player_slot << "," << tempPlayer.heroID << std::endl;
				
			}
			
			currentOldestMatch = tempMatch.matchID;
			matchList.push_back(tempMatch);
		
		}
	}
	
}


//}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}








int main()
{

	std::ifstream fin;
	std::string key = "";
	fin.open("key");
	try{
		if (!fin.good()) throw "I/O error";
	}
	catch (std::exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
	getline(fin, key);
	std::string playerId = "76561197961499967";

	// open file for output
	cout << "trying to open file" << endl;
	std::ofstream fout("76561197961499967.csv");



	std::string finalUrl;
	
	int currentMatch = 0;

	std::string base_url = "https://api.steampowered.com/IDOTA2Match_570/GetMatchHistory/V001/?key=";

	finalUrl = base_url + key;

	std::string match_request_string = "&matches_requested=";
	std::string amountOfMatchesToFetch = "100";
	
	finalUrl = finalUrl + "&account_id=" + playerId;
	std::string actualString = finalUrl + match_request_string + amountOfMatchesToFetch;
	std::string useString = actualString;

	// start from

	std::string startFrom = "&start_at_match_id=";
	std::string startFromPosition = "";
	std::string combineStart = "";

	bool continueToFetch = true;

	// write output headers
	cout << "attempting to write headers" << endl;
	//fout << "match_id,match_seq_num,start_time,account_id,player_slot,hero_id" << endl;
	
	

	
	try
	{
	


		playersMatches rec;

		// fetch first batch

		

    	// prepare first string

    	int skip = 7;


    	while (continueToFetch)
		{
			CURL *curl;
    		FILE *fp;
    		CURLcode res;
			cout << useString << endl;
			skip--;
			const char *url = useString.c_str();
			char outfilename[FILENAME_MAX] = "test.json";

			curl = curl_easy_init();
			if (curl) 
			{
				fp = fopen(outfilename,"w");
				curl_easy_setopt(curl, CURLOPT_URL, url);
				curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
				fclose(fp);
				
				// parse file
				
				rec.load("test.json", fout);

				char tempString[50];
				
				// logic for checking if done
				cout << "checking if finished" << endl;
 				if(rec.matchesLeft == "0")
				{
					//continueToFetch = false;
					cout << "it says there are none left! im going to keep on going" << endl;
					// this is stuff copied from below
					amountOfMatchesToFetch = "100";
					startFromPosition = rec.currentOldestMatch;
					int value = ((atoi(startFromPosition.c_str())));
					value--;
					
					//sprintf(startFromPosition.c_str(),"%d",value);
					
					sprintf(tempString,"%d",value);

					cout << "starting from: " << tempString << endl;
					useString = actualString + startFrom + tempString;
				}
				else if((atoi(rec.matchesLeft.c_str())) >= 100)
				{
					cout << "inside second condition" << endl;
					amountOfMatchesToFetch = "100";
					startFromPosition = rec.currentOldestMatch;
					long int value = ((atoi(startFromPosition.c_str())));
					value--;
					cout << "just decrimented the value" << endl;
					

					sprintf(tempString,"%d",value);

					startFromPosition = tempString;

					cout << "starting from: " << startFromPosition << endl;
					useString = actualString + startFrom + startFromPosition;
					cout << "done making string" << endl;

				}
				else
				{
					amountOfMatchesToFetch = "100";
					startFromPosition = rec.currentOldestMatch;
					long int value = ((atoi(startFromPosition.c_str())));
					value--;
					
					sprintf(tempString,"%d",value);
					useString = actualString + startFrom + tempString;
				}
			}
			if (skip == 0)
			{
				break;
			}
		}
		


		
		
	}
	catch (std::exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
	fin.close();
	fout.close();
	
	
	

}
