// Merge sort Simulation
// By Amine Rehioui
// http://www.aui.ma/~A.Rehioui

#include <stdlib.h>
#include <math.h>
#include "SDLFONT_foo.h"
#include "SDL.h"
#include "SDL_thread.h"

SDL_Surface *screen;
int time_start, time_end=0; // needed for timing stats
bool MergeSortThreadDone = false; // to know when thread is done
int insertion_sort_trigger = 0; // needed for simulation
int best_trigger = -1;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480


// 2D Routines /////////////////////////////

void PutPixel32(SDL_Surface*,int,int,int);
void VLine32(SDL_Surface*,int,int,int,int);
void HLine32(SDL_Surface*,int,int,int,int);

////////////////////////////////////////////




// Merge sort funcs //////////////////////////////////////////
void Merge(int *arr, int left_half_start, int left_half_size, 
		   int right_half_start, int right_half_size, 
		   /*just for seeing effect*/int latency);

// Actual recursive func
void MergeSort(int *arr, int left, int right, int latency);
// end of Merge sort funcs //////////////////////



// Thread code - needed for seeing sorting in real time ////
struct MergeSortThreadData
{
  int *arr_to_sort;
  int arr_size;
  int latency_of_sorting;
};

int MergeSortThread(void *data); // Thread func
// end of Thread code //////////////////////////////



// Insertion Sort /////////////////////////////

void InsertionSort(int *arr, int size, int latency);

/////////////////////////////////////////////



// Simulation funcs ///////////////////////

void PlotStats(SDL_Surface* s, int *stats, int stats_size);

//////////////////////////////////////////




// main ////////////////////
int main(int argc, char *argv[])
{
	int mode;
	bool rendering = false;
	SDL_Thread *CurrentThread=NULL;
	MergeSortThreadData ThreadData;
	int arr_to_sort[SCREEN_WIDTH]; // array to sort
	int sim_threads, current_sim_thread = 0;
	int *time_table=NULL; // for simulation mode	

	SDL_Rect TopRegion, BottomRegion; // regions to clear
	TopRegion.x = 0; TopRegion.y = 0; 
	TopRegion.w = SCREEN_WIDTH; TopRegion.h = SCREEN_HEIGHT/2;
	BottomRegion.x = 0; BottomRegion.y = SCREEN_HEIGHT/2; 
	BottomRegion.w = SCREEN_WIDTH; BottomRegion.h = SCREEN_HEIGHT/2;	
	
	SDLFONT_foo font; // used for drawing on SDL surface
	if(!font.LoadMap("fontmap.bmp", 16, 16, 1)) goto error; // init font
	
	// vars	
	int latency;

	printf("Welcome to MergeSort simulation.\n");
	printf("By Amine Rehioui, http://www.aui.ma/~A.Rehioui\n\n");
	system("pause");

	// main loop
	while(1)
	{
		system("cls");			

		// randomize arr_to_sort
		for(int i=0; i<SCREEN_WIDTH; i++) arr_to_sort[i] = rand()%(SCREEN_HEIGHT/2); 

		if(time_table) delete[] time_table;

		insertion_sort_trigger = 0;
		
		while(1)
		{
			if(best_trigger>=0) 
			{
			  printf("LAST SIMULATION RESULTS:\n");
			  printf("BEST TRIGGER WAS %d.\n\n", best_trigger);
			}
			printf("Enter mode:\n");
			printf("1 - Simple mode.\n");
			printf("2 - Simulation mode.\n");
			printf("3 - quit\n");
			scanf("%d", &mode);
			if(mode == 1)
			{
				system("cls");
				while(1)
				{
					printf("Enter latency amount (ms): ");
					scanf("%d", &latency);
					if(latency >= 0) break;
					printf("(Negative value refused)\n");
				}			
				
				break;
			}
			else if(mode == 2)
			{
				system("cls");
				while(1)
				{
					printf("Enter latency amount (ms):\n");
					printf("(RECOMMENDED FOR SIMULATION: 1ms): ");
					scanf("%d", &latency);
					if(latency >= 0) break;
					printf("(Negative value refused)\n");
				}	

				while(1)
				{
					printf("Enter number of MergeSorts (630 maximum):\n");
					printf("(RECOMMENDED FOR SIMULATION: around 150 sorts): ");
					scanf("%d",&sim_threads);
					if(sim_threads > 0 && sim_threads <= 630) break;
					printf("Invalid value\n");
				}
				time_table = new int[sim_threads];

				printf("Starting simulation mode.\n");	
				printf("Merge sorts will be sequentially launched.\n");
				printf("Sorts will have an increasing insertion sort trigger.\n");
				printf("Sorting time will be recorded for each one and a time curve\n");
				printf("will be plotted at the end.\n");
				printf("THIS MAY TAKE LONG TIME.\n");				
				system("pause");
				
				break;
			}
			else if(mode == 3)	goto no_error;				
			system("cls");
			printf("Invalid choice.\n");
		} // end while 

		// data stored. Now start graphics
		printf("initializing SDL....\n");
		if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) 
		{
			fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
			goto error;
		}		

		//  create a 640x480 window with 32bit pixels.
		screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);   
		if ( screen == NULL ) 
		{
			printf("Unable to set 640x480 video: %s\n", SDL_GetError());
			goto error;
		}  
		
		SDL_FillRect(screen, NULL, 0x000000); // clear to black

		if(mode == 1)
		{
			// create simple sorting thread
			MergeSortThreadDone = false;
			ThreadData.arr_to_sort = arr_to_sort;
			ThreadData.arr_size = SCREEN_WIDTH;
			ThreadData.latency_of_sorting = latency;
			CurrentThread = SDL_CreateThread(&MergeSortThread, (void*)&ThreadData);
		}
		else 
		{
			current_sim_thread = 0;			

			// create sim thread
			MergeSortThreadDone = false;
			ThreadData.arr_to_sort = arr_to_sort;
			ThreadData.arr_size = SCREEN_WIDTH;
			ThreadData.latency_of_sorting = latency; 
			CurrentThread = SDL_CreateThread(&MergeSortThread, (void*)&ThreadData);
			current_sim_thread++;

			// fill stats area
			if (SDL_MUSTLOCK(screen))
				if (SDL_LockSurface(screen) < 0) goto error; 

			HLine32(screen,SCREEN_HEIGHT-20, 10, SCREEN_WIDTH, 0x0000ff);
			VLine32(screen,10,SCREEN_HEIGHT/2,SCREEN_HEIGHT-18, 0x0000ff);
			font.DrawText(screen,20,SCREEN_HEIGHT/2+(SCREEN_HEIGHT-20-SCREEN_HEIGHT/2)/2,0,"Waiting Threads to finish..");
			font.DrawText(screen,0,SCREEN_HEIGHT-20, 0, 
			"Time VERSUS Threads");

			if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
		}		

		// render loop
		rendering = true;
		while(rendering)
		{
			// Lock surface if needed
			if (SDL_MUSTLOCK(screen))
				if (SDL_LockSurface(screen) < 0) goto error; 

			SDL_FillRect(screen, &TopRegion, 0x000000); // clear upper half
			
			if(mode == 1)
			{
				// Render lines
				for(int i=0; i<SCREEN_WIDTH; i++)
					VLine32(screen,i,SCREEN_HEIGHT/2,SCREEN_HEIGHT/2-arr_to_sort[i],0xff7f00);
			
				if(!MergeSortThreadDone)
					font.DrawText(screen,0,SCREEN_HEIGHT/2,0,"Time elaspsed: %d ms", SDL_GetTicks()-time_start);
				else 
					font.DrawText(screen,0,SCREEN_HEIGHT/2,0,
				    "Time elapsed: %d ms (%.6f) SECONDS\nPress escape or click on 'close'.", time_end, (float)time_end/1000.0f);
			}
			else
			{
				// Render lines
				for(int i=0; i<SCREEN_WIDTH; i++)
					VLine32(screen,i,SCREEN_HEIGHT/2,SCREEN_HEIGHT/2-arr_to_sort[i],0xff7f00);
			
				font.DrawText(screen,0,SCREEN_HEIGHT/2+font.FontHeight(),0,"Merge Sort # %d/%d, trigger: %d", current_sim_thread, sim_threads, insertion_sort_trigger);

				if(!MergeSortThreadDone)
					font.DrawText(screen,0,SCREEN_HEIGHT/2,0,"Time elaspsed: %d ms", SDL_GetTicks()-time_start);
				else 
				{	
					font.DrawText(screen,0,SCREEN_HEIGHT/2,0,"Time elapsed: %d ms (%.6f) SECONDS", time_end, (float)time_end/1000.0f);
					
					if(current_sim_thread < sim_threads)
					{
						// randomize arr_to_sort
						for(int i=0; i<SCREEN_WIDTH; i++) arr_to_sort[i] = rand()%(SCREEN_HEIGHT/2); 					

						time_table[current_sim_thread-1] = time_end; // store time value
						insertion_sort_trigger ++;

						// create sim thread
						MergeSortThreadDone = false;
						ThreadData.arr_to_sort = arr_to_sort;
						ThreadData.arr_size = SCREEN_WIDTH;
						ThreadData.latency_of_sorting = latency;
						CurrentThread = SDL_CreateThread(&MergeSortThread, (void*)&ThreadData);
						current_sim_thread++;
					}
					else
					{
						time_table[current_sim_thread-1] = time_end; // store time value
						
						// plot stats
						SDL_FillRect(screen,&BottomRegion,0);						

						font.DrawText(screen,0,0,0,"Press escape or click on 'close'.");
						HLine32(screen,SCREEN_HEIGHT-20, 10, SCREEN_WIDTH, 0x0000ff);
						VLine32(screen,10,SCREEN_HEIGHT/2,SCREEN_HEIGHT-18, 0x0000ff);
						font.DrawText(screen,0,SCREEN_HEIGHT-20, 0, "Time VERSUS Threads");

						PlotStats(screen, time_table, sim_threads);
					}
				}
			}
			
			// Unlock if needed
			if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
			SDL_UpdateRect(screen, 0, 0, 640, 480);  
			/////////

			// Poll and handle for events
			SDL_Event event;
			while (SDL_PollEvent(&event)) 
			{				
				if(event.type == SDL_KEYUP) 
				{
					if (event.key.keysym.sym == SDLK_ESCAPE) 
					{ 
						if(CurrentThread) SDL_KillThread(CurrentThread); 
						rendering = false; break;
					}
				}
				else if(event.type == SDL_QUIT) 
				{
					printf("hit close\n");
					if(CurrentThread) SDL_KillThread(CurrentThread);
					rendering = false; break;
				}
				else
				{

				}           
			} // end while (event)

		} // end while (render)

		SDL_Quit(); // close SDL window, go back to console		
		
	} // end while (!done)

	goto no_error;

error:

	printf("%s.\n", SDL_GetError() ? SDL_GetError() : "Unknown error occured" );
	system("pause");
	
no_error:

	font.Destroy();

	SDL_Quit();

	system("cls");
	printf("Program terminated normally.\n");
	printf("Thanks for using this.\n");
	printf("Amine Rehioui, http://www.aui.ma/~A.Rehioui\n");

	system("pause");

	return 0;
}
// end of main //////////////////////////



// 2D Routines //////////////////////////////

// putpixel on a 32-bit surface
void PutPixel32(SDL_Surface* s, int x, int y, int color)
{
	((unsigned int*)s->pixels)[x + y * (s->pitch / 4)] = color; // pitch is in bytes, 
	// assume 4bytes (32bit) surface
}

void VLine32(SDL_Surface* s, int x, int y1, int y2, int color)
{
	int incr = y1<y2 ? 1 : -1;
	for(int i=y1; i!=y2; i+=incr)
     PutPixel32(s,x,i,color);	  
}

void HLine32(SDL_Surface* s, int y, int x1, int x2, int color)
{
	int incr = x1<x2 ? 1 : -1;
	for(int i=x1; i!=x2; i+=incr)
     PutPixel32(s,i,y,color);	  
}
////////////////////////////////////////////////




// Merge sort funcs //////////////////////////////////////
void Merge(int *arr, int left_half_start, int left_half_size, 
		   int right_half_start, int right_half_size, 
		   /*just for seeing effect*/int latency)
{
	int *temp = new int[left_half_size+right_half_size]; // temp array needed
	int dest_index=0;
	int left_index = left_half_start;
	int right_index = right_half_start;

	if(latency) SDL_Delay(latency);

	while(1)
	{
		if(left_index >= left_half_start+left_half_size)
		{
			for(int i=right_index; i<right_half_start+right_half_size; i++) temp[dest_index++] = arr[i]; break;
		} // reached end of left part, break
		if(right_index >= right_half_start+right_half_size)
		{
			for(int i=left_index; i<left_half_start+left_half_size; i++) temp[dest_index++] = arr[i]; break;
		} // reached end of right part, break

		temp[dest_index++] = (arr[left_index]<arr[right_index])?arr[left_index++]:arr[right_index++];
	} // end while

	// copy temp to original array
	for(int i=left_half_start; i<left_half_start+left_half_size+right_half_size; i++)
	{  				 
		arr[i] = temp[i-left_half_start];
	}
	delete[] temp;
}

// Actual recursive func
void MergeSort(int *arr, int left, int right, int latency)
{
	if(right == left) return; // stop if arr has 1 elem

	// added code for simulation
    if(right - left <= insertion_sort_trigger ) return;
	// end of added code //////////////////////////

	int mid = (left + right) / 2;
	MergeSort(arr, left, mid, latency); // sort left part
	MergeSort(arr, mid+1, right, latency); // sort right part
	Merge(arr, left, mid-left+1, mid+1, right-mid, latency); // merge them
}
// end of Merge sort funcs //////////////////////




// Thread code - needed for seeing sorting in real time ////
int MergeSortThread(void *data) // Thread func
{
  time_start = SDL_GetTicks();

  MergeSortThreadData _data = *((MergeSortThreadData*) data);
  
  MergeSort(_data.arr_to_sort, 0, _data.arr_size-1, _data.latency_of_sorting);

  // Do the insertion sort pass
  if(insertion_sort_trigger)
	InsertionSort(_data.arr_to_sort, _data.arr_size, _data.latency_of_sorting);

  time_end = SDL_GetTicks() - time_start;

  MergeSortThreadDone = true;

  return 0;
}
// end of Thread code //////////////////////////////



// Insertion sort //////////////////////
void InsertionSort(int *arr, int size, int latency)
{
   int t;

   for (int i = 1; i <= size-1; i++)
   {
        t = arr[i];

        // shift until insertion point
        for (int j = i-1; j >= 0 && arr[j] > t; j--)
            arr[j+1] = arr[j];
        
		if(latency) SDL_Delay(latency);
        arr[j+1] = t;
    }
}
////////////////////////////////////


// Simulation funcs ///////////////////////

void PlotStats(SDL_Surface* s, int *stats, int stats_size)
{
	// get max value
	int max=stats[0];	
	for(int i=1; i<stats_size; i++) if(stats[i] > max) max = stats[i];
	
	// get min value (for best trigger)
	int min=0;
	for(int i=1; i<stats_size; i++) if(stats[i] < stats[min]) min = i;
	best_trigger = min;

	// plot pixels
	for(i=0; i<stats_size-1; i++)
	{
		PutPixel32(s,10+i,SCREEN_HEIGHT - 20 - (int)((float)SCREEN_HEIGHT/2.3f*(float)stats[i]/(float)max), 0x00ffff);
	}
}

//////////////////////////////////////////
