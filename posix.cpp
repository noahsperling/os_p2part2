#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <random>

/*
* @authors Noah Sperling and Alexander Nowlin
* @version 04/21/2018
*/

using namespace std;

// semaphore to lock bridge traveling
static bool semaphore;
static int prev_dir = 0; // 0 for north, 1 for south

// number of farmers for north and south
static int num_north;
static int num_south;

// function to sleep for a random time, simulating crossing the bridge
void sleep_rand_time() {
  int sec = rand() % 2;
  cout << "sleeping " << sec << " seconds"<< endl;
  sleep(sec);
  return;
}

// function for a north farmer crossing the bridge
void north() {
  while (semaphore) {
    // if brige not locked and previous farmer that crossed was south farmer
    // or there are no south farmers waiting to cross
    if (!semaphore && (prev_dir == 1 || num_south == 0)) {
      // lock the bridge
      semaphore = true;
      break;
    }
  }
  // north farmer crossing bridge
  cout << "Going North!" << endl;
  sleep_rand_time();

  // decrease number of north farmers
  num_north--;

  // unlock bridge
  semaphore = false;
  return;
}

// function for a south farmer crossing the bridge
void south() {
  while (semaphore) {
    // if bridge not locked and previous farmer that crossed was north farmer
    // or there are no norhter farmers waiting to cross
    if (!semaphore && (prev_dir == 0 || num_north == 0)) {
      // lock the bridge
      semaphore = true;
      break;
    }
  }
  // south farmer crossing bridge
  cout << "Going South!" << endl;
  sleep_rand_time();

  // decrease number of south farmers
  num_south--;

  // unlock the bridge
  semphore = false;
  return;
}

int main(int argc, char *argv[]) {

  // check for valid number of arguments
  if (argc != 3) {
    cout << "Invalid Arguments.  Re-run with the number of northbound farmers and then the number of southbound farmers." << endl;
  }

  // store the arguments as the number of north and south farmers
  num_north = atoi(argv[1]);
  num_south = atoi(argv[2]);

  // initialize bridge to unlocked
  semaphore = false;

  // seeds the rand() function
  srand(time(NULL));

  // array of threads to represent the north and south farmers
  pthread_t north_threads[num_north];
  pthread_t south_threads[num_south];

  // index for north and south farmer arrays
  cur_north_index = num_north - 1;
  cur_south_index = num_south - 1;
  return_val;

  while(num_north != 0 || num_south != 0) {

    // if north farmer just went
    if(prev_dir == 0) {
      // if there are south farmers waiting to cross
      if (num_south > 0) {
        // send south farmer across bridge
        return_val = pthread_create(&south_threads[cur_south_index], NULL, south, NULL);

        // decrement the index to represent that farmers have crossed the bridge
        cur_south_index--;
      } else if (num_north > 0) {
        // no south farmers waiting, but north farmers waiting

        // send north farmer across bridge
        return_val = pthread_create(&north_threads[cur_north_index], NULL, north, NULL);

        // decrement the index to represent that farmers have crossed the bridge
        cur_north_index--;
      }
    } else if(prev_dir == 1) {
      // if south farmer just went

      // if there are north farmers waiting to cross
      if (num_north > 0) {
        // send north farmer across bridge
        return_val = pthread_create(&north_threads[cur_north_index], NULL, north, NULL);

        // decrement the index to represent that farmers have crossed the bridge
        cur_north_index--;
      } else if (num_south > 0) {
        // no norht farmers waiting, but south farmers waiting

        // send south farmer across bridge
        return_val = pthread_create(&south_threads[cur_south_index], NULL, south, NULL);

        // decrement the index to represent that farmers have crossed the bridge
        cur_south_index--;
      }
    }

    // check that threads were created successfully
    if(return_val) {
      cout << "\nerror, unable to create thread " << return_val << endl;
      exit(-1);
    }
  }

  // program finished, all farmers crossed
  cout << "All farmers crossed bridge!" << endl;


}
