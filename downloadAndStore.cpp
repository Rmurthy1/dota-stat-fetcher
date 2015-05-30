// compile with: g++ downloadAndStore.cpp -o downloadAndStore.cgi -Llibboost_iostreams.so.1.49.0 -lcurl

// currently working, saving state

#include "precompileTest.hpp"
#include <string>
#include <vector>
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
using std::string;

#include <iomanip>
#include <curl/curl.h>
#include <curl/easy.h>
#include <fstream>
#include <queue>

#include <unistd.h>

// top level
// collection of matches (aggregate calcs performed here)

// mid level
// matches with a collection of players

// low level
// player data for a single match

class MyGames;
class Game;

// maybe get rid of some of these friendships


/**
 * class to hold each player's data
 */
class Player
{
	friend class Game;
	friend class MyGames;
	string m_account_id;
	string m_player_slot;
	string m_hero_id;

public:
	Player(string account_id, string player_slot, string hero_id);
	string getAccountId() {return m_account_id;}
	
	bool operator<(const Player& rhs) const
	{
		int comp = m_account_id.compare(rhs.m_account_id);
		if (comp < 0)
		{
			return true;
		}
		return false;
	}
};

Player::Player(string account_id, string player_slot, string hero_id)
{
	m_account_id = account_id;
	m_player_slot = player_slot;
	m_hero_id = hero_id;
}


class comparisonForFrequency
{
public:
	bool operator ()(const std::pair<Player, int> &a, const std::pair<Player, int> &b) const
	{
		if (a.second < b.second)
		{
			return true;
		}
		return false;
	}
};


/**
 * class to hold each game's data (includes a collection of players)
 */
class Game
{
	friend class MyGames;
	//bool dire_win;
	//bool did_i_win;
	string m_lobby_type;
	string m_match_id;
	string m_start_time;
	std::vector<Player> m_players_in_game;
public:
	Game (std::vector<Player>, string, string, string);
};

Game::Game(std::vector<Player> players, string lobby_type, string match_id, string start_time)
{
	m_players_in_game = players;
	m_lobby_type = lobby_type;
	m_match_id = match_id;
	m_start_time = start_time;
}

/**
 * class to hold a player's game history
 */
class MyGames
{
	std::vector<Game> m_allGames;
	bool compareForPlayerFrequency(const std::pair<Player, int>&, const std::pair<Player, int>&) const;
	comparisonForFrequency *comp;
public:
	void addGames(Game);
	void load(const std::string &filename);
	void mostSeen();

};



bool MyGames::compareForPlayerFrequency(const std::pair<Player, int> &a, const std::pair<Player, int> &b) const
{
	if (a.second > b.second)
	{
		return true;
	}
	return false;
}


void MyGames::mostSeen()
{
	std::map<Player, int> seenPlayers;
	
	for (int i = 0; i < m_allGames.size(); i++)
	{
		for (int j = 0; j < m_allGames[i].m_players_in_game.size(); j++)
		{
			
			//std::cout << "writing alot of lines!!!" << std::endl; // test
			
			// seenPlayers[m_allGames[i].m_players_in_game[j]]++;

			// swap this count with the .find(key) and see if its not equal to .end() later!!!
			// otherwise im just doing double the work kinda
			Player key = m_allGames[i].m_players_in_game[j];
			if (seenPlayers.count(key) == 1) 
			{
                seenPlayers[key] = seenPlayers[key] + 1;
            }
            else 
            {
                seenPlayers[key] = 1;
            }

		}
	}
	// map now populated with counts
	//typedef std::map<std::string, std::map<std::string, std::string>>::iterator it_type;
	/*
	for (std::map<Player, int>::iterator i = seenPlayers.begin(); i != seenPlayers.end(); i++)
	{
		//std::std::vector<pair<> v;
		//lets just print the map
		std::cout << "element: " << i->first << " count: " << i->second << std::endl;

	}
	*/
	// whole lotta copying going on here, gotta switch this up later
		// in this im making a pair and then throwing the pair in a priorty queue with a custom compare
	std::priority_queue<std::pair<Player, int>, std::vector<std::pair<Player, int> >, comparisonForFrequency> pqueue;
	for (std::map<Player,int>::iterator it=seenPlayers.begin(); it!=seenPlayers.end(); ++it)
	{
    	//std::cout << it->first.m_account_id << " => " << it->second << '\n';

    	//std::pair<Player, int> tempPair = *it;
    	pqueue.push(*it);//maybe switch to &

    }

    for (int i = 0; i < 20; i++)
    {
    	std::cout << pqueue.top().first.m_account_id << "=>" << pqueue.top().second << std::endl;
    	pqueue.pop();
    }


}

void MyGames::addGames(Game g)
{
	m_allGames.push_back(g);
}

// this is for later!! once things get cleaned up
// need to add some more fields to MyGames to make this work, also some accessors for those fields
// at first glance i need
	// currentOldestMatch
	// matchesLeft

/*
void MyGames::load(const std::string &filename)
{
	using boost::property_tree::ptree;
	ptree pt;

	read_json(filename, pt);
		
	
	BOOST_FOREACH (boost::property_tree::ptree::value_type& result, pt)
	{
		cout << "there are " << result.second.get<std::string>("results_remaining") << " matches left" << endl;
		BOOST_FOREACH (boost::property_tree::ptree::value_type& matches, result.second.get_child("matches"))
		{
			// inside of each match, make a vector of players
			std::vector<Player> tempPlayerVec; // may need to make this a player pointer, idk

			BOOST_FOREACH (boost::property_tree::ptree::value_type& players, matches.second.get_child("players"))
			{
				player tempPlayer; // struct (old way)
				tempPlayer.account_id = players.second.get<std::string>("account_id");
				tempPlayer.player_slot = players.second.get<std::string>("player_slot");
				tempPlayer.heroID = players.second.get<std::string>("hero_id");
				tempMatch.players.push_back(tempPlayer);

				// remove the _2 when the structs are removed
				Player tempPlayer_2(players.second.get<std::string>("account_id"), players.second.get<std::string>("player_slot"), players.second.get<std::string>("hero_id"));
				tempPlayerVec.push_back(tempPlayer_2); // add the player to the vector of players


				//out << tempMatch.matchID << "," << tempMatch.startTime << "," << tempMatch.match_seq_num << "," << tempPlayer.account_id << "," << tempPlayer.player_slot << "," << tempPlayer.heroID << std::endl;
				
			}
			// temp game
			Game tempGame(tempPlayerVec, matches.second.get<std::string>("lobby_type"), matches.second.get<std::string>("match_id"), matches.second.get<std::string>("start_time"));

			currentOldestMatch = tempMatch.matchID;
			matchList.push_back(tempMatch);
			gameList.addGames(tempGame); // add the match to the collection of matches
		
		}
	}
	
}
*/


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

struct player_old
{
	std::string account_id;
	std::string player_slot;
	std::string heroID;
};

struct match
{
	std::vector<player_old> players;
	std::string matchID;
	std::string startTime;
	std::string match_seq_num;
};

struct playersMatches
{
	std::vector<match> matchList;
	void load(const std::string &filename, ostream &out, MyGames &gameList);
	std::string matchesLeft;
	std::string currentOldestMatch;
};






// refactor this to be part of the new class
void playersMatches::load(const std::string &filename, ostream &out, MyGames &gameList)
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
			// the struct
			match tempMatch;

			tempMatch.startTime = matches.second.get<std::string>("start_time");
			tempMatch.matchID = matches.second.get<std::string>("match_id");
			tempMatch.match_seq_num = matches.second.get<std::string>("match_seq_num");

			// inside of each match, make a vector of players
			std::vector<Player> tempPlayerVec; // may need to make this a player pointer, idk

			BOOST_FOREACH (boost::property_tree::ptree::value_type& players, matches.second.get_child("players"))
			{
				player_old tempPlayer; // struct (old way)
				tempPlayer.account_id = players.second.get<std::string>("account_id");
				tempPlayer.player_slot = players.second.get<std::string>("player_slot");
				tempPlayer.heroID = players.second.get<std::string>("hero_id");
				tempMatch.players.push_back(tempPlayer);

				// remove the _2 when the structs are removed
				Player tempPlayer_2(players.second.get<std::string>("account_id"), players.second.get<std::string>("player_slot"), players.second.get<std::string>("hero_id"));
				tempPlayerVec.push_back(tempPlayer_2); // add the player to the vector of players


				//out << tempMatch.matchID << "," << tempMatch.startTime << "," << tempMatch.match_seq_num << "," << tempPlayer.account_id << "," << tempPlayer.player_slot << "," << tempPlayer.heroID << std::endl;
				
			}
			// temp game
			Game tempGame(tempPlayerVec, matches.second.get<std::string>("lobby_type"), matches.second.get<std::string>("match_id"), matches.second.get<std::string>("start_time"));

			currentOldestMatch = tempMatch.matchID;
			matchList.push_back(tempMatch);
			gameList.addGames(tempGame); // add the match to the collection of matches
		
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
		MyGames games;

		// fetch first batch

		

    	// prepare first string



    	while (continueToFetch)
		{
			// wait for one second before hammering the servers
			usleep(1000000);

			CURL *curl;
    		FILE *fp;
    		CURLcode res;
			cout << useString << endl;
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
				
				rec.load("test.json", fout, games);

				char tempString[50];
				
				// logic for checking if done
				cout << "checking if finished" << endl;
 				if(rec.matchesLeft == "0")
				{
					//continueToFetch = false;
					cout << "it says there are none left! im going to quit!" << endl;
					break;

				}
				else //if((atoi(rec.matchesLeft.c_str())) >= 100)
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
				/*
				else
				{
					amountOfMatchesToFetch = "100";
					startFromPosition = rec.currentOldestMatch;
					long int value = ((atoi(startFromPosition.c_str())));
					value--;
					
					sprintf(tempString,"%d",value);
					useString = actualString + startFrom + tempString;
					
					std::cout << "fart" << std::endl;
				}
				*/
			}
		}
		
		// test the mostSeen function
		std::cout << "trying to test most seen" << endl;
		games.mostSeen();

		
		
	}
	catch (std::exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
	fin.close();
	fout.close();
	
	
	

}
