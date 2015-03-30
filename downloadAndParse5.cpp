// compile with: g++ downloadAndParse5.cpp -o matchParse.cgi -Llibboost_iostreams.so.1.49.0 -lcurl

// currently working, saving state

#include "precompileTest.hpp"
#include <string>
#include <set>
#include <map>
#include <exception>
#include <iostream>
#include <iterator>
#include <iostream>
using std::ostream;
using std::cout;
using std::endl;
using std::iterator;
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
	std::set<std::string> dm_playerID; // change to ints later maybe
	// make a map, <string , vector of strings>? idk.

	std::string dm_duration;

	std::string dm_matchID;



	//std::map
	//void load(const std::string &filename);
};

struct player
{
	std::string accountID;
	std::string playerSlot;
	std::string heroID;
};

struct match
{
	std::vector<player> players;
	std::string matchID;
	std::string startTime;
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
	
	int i = 0;
	
	
	BOOST_FOREACH (boost::property_tree::ptree::value_type& result, pt)
	{
		matchesLeft = result.second.get<std::string>("results_remaining");
		cout << "there are " << matchesLeft << " matches left" << endl;
		BOOST_FOREACH (boost::property_tree::ptree::value_type& matches, result.second.get_child("matches"))
		{
			//cout << "right before making a new match" << endl;
			match tempMatch;
			BOOST_FOREACH (boost::property_tree::ptree::value_type& players, matches.second.get_child("players"))
			{
				//cout << "right before making a new player" << endl;
				player tempPlayer;
				tempPlayer.accountID = players.second.get<std::string>("account_id");
				tempPlayer.playerSlot = players.second.get<std::string>("player_slot");
				tempPlayer.heroID = players.second.get<std::string>("hero_id");
				tempMatch.players.push_back(tempPlayer);
				
			}
			tempMatch.startTime = matches.second.get<std::string>("start_time");
			tempMatch.matchID = matches.second.get<std::string>("match_id");
			currentOldestMatch = tempMatch.matchID;
			cout << "currentOldestMatch: " << currentOldestMatch << endl;
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



// current problem: not fetching second file, just keeps on loading the first one over and over
	// possible solutions:
		// 1. different filenames 
		// 2. make sure file downloads, the link is right but it doesnt want to work idk // its ok
		// 3. throw out the old file before the new one comes int
		// 4. maybe my arguments are in the wrong order for the request? // probably isnt this\
		
		// problem fixed, there was a space in the url that curl did not cut out


// now there is the problem of a wasted motion, when i request the match it grabs the last one, ill have to decriment the number for the last seen match
	// ok fixed it i think

// now to store the data




int main()
{

	std::ifstream fin;
	std::ifstream fout;
	std::string key = "";
	fin.open("key");
	if (!fin.good()) throw "I/O error";
	getline(fin, key);
	std::string playerId = "76561197961499967";

	std::ifstream fout;

	fout.open("76561197961499967.csv")

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
				
				rec.load("test.json");

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
					//sprintf(startFromPosition.c_str(),"%d",value);
					

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
		


		
		// free memory, put this inside the destructor later
		
		/*
		cout << "trying to free memory now" << endl;
		for(int i = 0; i < rec.matchList.size(); i++)
		{
			for(int j = 0; j < rec.matchList[i].players.size(); j++)
			{
				//cout << "trying to delete a player" << endl;
				//cout << "player id: " << rec.matchList[i].players[j].accountID << endl;
				//delete &(rec.matchList[i]->players[j]);
				//cout << "player deleted" << endl;
			}
			//cout << "trying to delete a match" << endl;
			//delete &rec.matchList[i];
		}
		*/

		/*
		dotaMatch dm;
		
		dm.load("test.json");
		std::cout << "hope it worked!" << std::endl;
		//std::cout << "value: " << "[expected to be false]" << std::endl;
		//std::cout << "value: " << dm.dm_winner << std::endl;
		std::cout << "player ids: " << std::endl;
		int test = 0;

		for(std::set<std::string>::iterator i = dm.dm_playerID.begin(); i != dm.dm_playerID.end(); i++) // can use boost foreach later
		{
			std::cout << *i << std::endl;
			std::cout << test << std::endl;
			test++;
		}*/
	}
	catch (std::exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
	
	
	
	

}
