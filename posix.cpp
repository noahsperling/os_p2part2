#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <chrono>
#include <thread>
#include <unistd.h>

/*
* @authors Noah Sperling and Alexander Nowlin
* @version 04/21/2018
*/

using namespace std;

// mutex to lock bridge traveling
pthread_mutex_t bridge_mutex = PTHREAD_MUTEX_INITIALIZER;

static int prev_dir = 0; // 0 for north, 1 for south

// number of farmers for north and south
static int num_north;
static int num_south;

static int running_threads;


// function to sleep for a random time, simulating crossing the bridge
void sleep_rand_time() {
  int sec = rand() % 5;
  if (sec == 0) {
    sec = 1;
  }
  cout << "sleeping " << sec << " seconds"<< endl;
  sleep(sec);
  return;
}


// function for a north farmer crossing the bridge
void *north(void *arg) {
  while (true) {
    // if brige not locked and previous farmer that crossed was south farmer
    // or there are no south farmers waiting to cross

    int temp = pthread_mutex_trylock(&bridge_mutex);
    if (temp != 0) {
      continue;
    } else {
      if (prev_dir == 1 || num_south == 0) {
        break;
      } else {
        pthread_mutex_unlock(&bridge_mutex);
        continue;
      }
    }
  }
  // north farmer crossing bridge
  cout << "Going North!" << endl;
  sleep_rand_time();

  running_threads--;

  // decrease number of northbound farmers
  num_north--;

  prev_dir = 0;

  // unlock bridge
  pthread_mutex_unlock(&bridge_mutex);
  return 0;
}


// function for a south farmer crossing the bridge
void *south(void* arg) {
  while (true) {
    // if bridge not locked and previous farmer that crossed was north farmer
    // or there are no norhter farmers waiting to cross
    int temp = pthread_mutex_trylock(&bridge_mutex);
    if (temp != 0) {
      continue;
    } else {
      if (prev_dir == 0 || num_north == 0) {
        break;
      } else {
        pthread_mutex_unlock(&bridge_mutex);
        continue;
      }
    }
  }
  // south farmer crossing bridge
  cout << "Going South!" << endl;
  sleep_rand_time();

  running_threads--;

  // decrease number of southbound farmers
  num_south--;

  prev_dir = 1;

  // unlock the bridge
  pthread_mutex_unlock(&bridge_mutex);
  return 0;
}


int main(int argc, char *argv[]) {
  cout << "Main called" << endl;

  // check for valid number of arguments
  if (argc != 3) {
    cout << "Invalid Arguments.  Re-run with the number of northbound farmers and then the number of southbound farmers." << endl;
    return -1;
  }

  // store the arguments as the number of north and south farmers
  num_north = atoi(argv[1]);
  num_south = atoi(argv[2]);

  if (num_north < 0 || num_south < 0) {
    cout << "Negative numbers of farmers are invalid!" << endl;
    return 0;
  } else if (num_north == 0 && num_south == 0) {
    cout << "No farmers created!" << endl;
    return 0;
  }

  cout << "Northbound Farmers: " << num_north << endl;
  cout << "Southbound Farmers: " << num_south << endl;

  // seeds the rand() function
  srand(time(NULL));

  // array of threads to represent the north and south farmers
  pthread_t north_threads[num_north];
  pthread_t south_threads[num_south];

  // index for north and south farmer arrays
  int cur_north_index = num_north - 1;
  int cur_south_index = num_south - 1;
  int return_val;

  pthread_mutex_lock(&bridge_mutex);
  while(cur_north_index >= 0 || cur_south_index >= 0) {
    // if there are south farmers waiting to cross
    if (cur_south_index >= 0) {
      // send south farmer across bridge
      return_val = pthread_create(&south_threads[cur_south_index], NULL, south, (void *)NULL);
      running_threads++;

      // decrement the index to represent that farmers have crossed the bridge
      cur_south_index--;
    } else if (cur_north_index >= 0) {
      // no south farmers waiting, but north farmers waiting

      // send north farmer across bridge
      return_val = pthread_create(&north_threads[cur_north_index], NULL, north, (void *)NULL);
      running_threads++;

      // decrement the index to represent that farmers have crossed the bridge
      cur_north_index--;
    }

    // check that threads were created successfully
    if(return_val) {
      cout << "\nerror, unable to create thread " << return_val << endl;
      exit(-1);
    }
  }
  cout << "North and Southbound farmers created, unlocking bridge" << endl;
  pthread_mutex_unlock(&bridge_mutex);

  // waits until all threads are done
  while(running_threads != 0) {
    sleep(1);
  }

  // program finished, all farmers crossed
  cout << "All farmers crossed bridge!" << endl;
  return 1;
}
