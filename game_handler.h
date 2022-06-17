#ifndef SDL_MAIN_HANDLED
#include "systems.h"
#endif

#define HANDLER

// initalize the game with background value
game_handler::game_handler(int x = -1)
{
	if(SDL_Init( IMG_INIT_JPG | IMG_INIT_PNG | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) >= 0)
	{
		//set texture filtering to linear
		//sets render quality to where images aren't blurry when resized
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
				
		// create window 
		window = SDL_CreateWindow( "Graphics", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if(window != NULL)
		{
			// creates renderer
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); 
					
			//Initialize renderer color
			SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF );
					
			//Initialize PNG loading
			int imgFlags = IMG_INIT_PNG;
			if( !( IMG_Init( imgFlags ) & imgFlags ))
			{
				printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
				success = false;
			}
					
			//Initialize text handling 
			if( TTF_Init() == -1 )
			{
				printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
				success = false;
			}
						
					//Initialize music handling 
			if( Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024*2 ) < 0 )
			{
				printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
				success = false;
			}
		}
	}
	
	// load game background assets 
	if(x == -1)
		b = 1;
	else
		b = x;
	loadBackground(b);
	
	// start framekeeper timer for limiting frame 
	framekeeper.start();
	
	// set up main text handler 
	displayText = text(renderer,0);
};
		
// loads background assets 
void game_handler::loadBackground(int type)
{
	if(background_assets != NULL)
		delete background_assets;
	loadIn = true;
	megaAlpha = 0;
}

// switching backgrounds 
void game_handler::switchBackground(int newOne)
{
	switchOut = true;
	switchTo = newOne;
}

// display background 
void game_handler::background(bool down)
{
	if(switchOut)
	{
		if (colors[0] < 255)
			colors[0] = colors[0] + 1;
		if (colors[1] < 255)
			colors[1] = colors[2] + 1;
		if (colors[2] < 255)
			colors[2] = colors[2] + 1;
	}
		
	colors[0] = 24;
	colors[1] = 2;
	colors[2] = 152;
	
	if(loadIn && megaAlpha == 255)
		loadIn = false;
	else if(loadIn)
		megaAlpha+=5;
	else if(switchOut && megaAlpha == 0)
	{
		b = switchTo;
		megaAlpha = 255;
		switchOut = false;
		loadBackground(b);
	}
	else if(switchOut)
	{
		megaAlpha-=5;
	}
}

// run the current game system 
void game_handler::runSystem()
{
	// clears display 
		SDL_SetRenderDrawColor(renderer, colors[0], colors[1], colors[2], colors[3]);
		SDL_RenderClear(renderer);
		capkeeper.start();
			
	// display background
		if((int)(colors[0]) < 100)
			down = false;
		else if((int)(colors[0]) > 175)
			down = true;
		if(backgroundOn)
			background(down);

	// display game system 
		if(currentGame != NULL)
			currentGame->display();
			
		SDL_SetRenderDrawColor(renderer, colors[0], colors[1], colors[2], colors[3]);
		
	// getting input 
		input.update();
			
	// handle game system 
		if(currentGame != NULL)
			currentGame->handler();	
			
	// displaying images 
		SDL_RenderPresent(renderer);
		++(frames); // adds to frame tally
		
		int framet = capkeeper.getTicks(); // for capping frame rate
		if( framet < SCREEN_TICK_PER_FRAME)
			SDL_Delay( SCREEN_TICK_PER_FRAME - framet );
	
	// clears current game pointer if the current game system ended
		if(currentGame != NULL && currentGame->endSystem)
			currentGame = NULL;
}	
	
// ending game, deconstructor  
void game_handler::close()
{
	if(background_assets != NULL)
		delete background_assets;
	// destroy used texture
	SDL_DestroyTexture(texture);
	texture = NULL;
	
	//Destroy window and renderer 
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;
			
	// close handlers 
	Mix_Quit();
	IMG_Quit();
		
	// close main SDL extension 
	SDL_Quit();
			
	exit(0);	
}