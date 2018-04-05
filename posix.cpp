#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <random>

/*
* @authors Noah Sperling and Alexander Nowlin
* @version 04/05/2018
*/

using namespace std;

static bool semaphore;
static int prev_dir = 0; // 0 for north, 1 for south
static int num_north;
static int num_south;

void sleep_rand_time() {
  int sec = rand() % 2;
  cout << "sleeping " << sec << " seconds"<< endl;
  sleep(sec);
  return;
}

void north() {
  while (semaphore) {
    if (!semaphore && (prev_dir == 1 || num_south == 0)) {
      semaphore = true;
      break;
    }
  }
  sleep_rand_time();
  num_north--;
  semaphore = false;
  return;
}

void south() {
  while (semaphore) {
    if (!semaphore && (prev_dir == 0 || num_north == 0)) {
      semaphore = true;
      break;
    }
  }
  sleep_rand_time();
  num_south--;
  semphore = false;
  return;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cout << "Invalid Arguments.  Re-run with the number of northbound farmers and then the number of southbound farmers." << endl;
  }


  num_north = atoi(argv[1]);
  num_south = atoi(argv[2]);

  semaphore = false;

  // seeds the rand() function
  srand(time(NULL));


}
