#include "game_handler.h"
#include <curl/curl.h>

//g++ connector.cpp -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lcurl -static-libstdc++ -std=c++11 -Wl,-rpath,./ -o connector

class connection_system : public system_handler
{
	public:
		connection_system(game_handler * g)
		{
			main_game = g;
		}
	
		void handler() override
		{
			CURL *curl;
			CURLcode res;
	   	    string cookie= "koekje.txt";
			string url = "http://127.0.0.1:8000/";
   
			std::string readBuffer;
			string response_string;
			
			//check connection 
			curl = curl_easy_init();
			if(curl) 
			{
				curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
				curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);
	
				// perform POST request 
				res = curl_easy_perform(curl);
    
				// check error
				if(res != CURLE_OK)
					connected = false;
				else 
					connected = true;
				curl_easy_cleanup(curl);
			}
			
			curl = curl_easy_init();
			switch(main_game->input.state)
			{
				case TYPING:
				if(renderText)
					messageText += main_game->input.typedText();
				break;
				case CANCEL:
				if(messageText.length() > 0)
					messageText.pop_back();
				break;
				case SELECT:
				if(renderText)
				{
					renderText = false;
					
					SDL_StopTextInput();
				}
				else
				{
					renderText = true;
					SDL_StartTextInput();
				}
				break;
				case UP: // POST ON UP
				if(curl) 
				{
					struct curl_slist *header_chunk = NULL;  
	
					curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	
					// POST request
					curl_easy_setopt(curl, CURLOPT_POSTFIELDS, messageText.c_str());
	
					// perform POST request 
					res = curl_easy_perform(curl);
    
					// check error
					if(res != CURLE_OK)
						fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
	
					curl_easy_cleanup(curl);
				}
				break;
				case DOWN: // GET ON DOWN 
				if(curl) 
				{
					curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	
					// perform GET request 
					res = curl_easy_perform(curl);
		
					// check for errors 
					if(res != CURLE_OK)
						fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
 
					curl_easy_cleanup(curl);
				}
				break;
			}
		}
		
		void display() override
		{
			if(connected)
				main_game->displayText.display("Server status: Connected",10,10);
			else
				main_game->displayText.display("Server status: Disconnected",10,10);
				
			main_game->displayText.display(messageText,200,200);
		}
		
	private:
		string messageText = "";

		bool connected = false;
		bool renderText = false;
		string cookie= "koekje.txt";
		string url = "http://127.0.0.1:8000/";
};

int main(int argc, char *argv[])
{
	srand((unsigned)time(NULL));
	
	game_handler game = game_handler(4);
	
	connection_system test = connection_system(&game);
	game.currentGame = &test;
	
	while(game.input.state != EXIT)
	{
		game.runSystem();
	}
	
	game.close();
	return 0;
}