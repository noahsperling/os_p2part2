#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <random>
#include <chrono>
#include <thread>
#include <unistd.h>

/*
* @authors Noah Sperling and Alexander Nowlin
* @version 04/19/2018
*/

using namespace std;

#define NUMBER_OF_CUSTOMERS 2
#define NUMBER_OF_RESOURCES 2

// mutex lock for synchronization
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// data structures for bankers algorithm
int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {{2,2},{2,2}};
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {{1,0},{1,1}};
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {{1,2},{1,1}};


// implements bankers algorithm to check and see if the system is in a safe state
bool safety() {

  // data structures for algorithm
  int work[NUMBER_OF_RESOURCES];
  bool finish[NUMBER_OF_CUSTOMERS];

  // initializes work array
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    work[i] = available[i];
  }

  // initializes finish array
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    finish[i] = false;
  }

  // whether an index has been found s.t. need <= work
  bool temp1 = true;

  // loops until an no index exists such that need <= work
  while (temp1) {

    // no index has been found yet this iteration
    temp1 = false;

    // loops through indices to find one where need <= work
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
      bool temp = false;

      // if this one has already been assessed, continue
      if (finish[i]) {
        continue;
      }

      // checks an index to see if need <= work
      for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        if (need[i][j] <= work[j]) {
          temp = true;
        }
        else {
          temp = false;
          break;
        }
      }

      // at this point, it has been determined that need <= work
      if (temp) {
        // updates work
        for (int k = 0; k < NUMBER_OF_RESOURCES; k++) {
          work[k] = work[k] + allocation[i][k];
        }
        // update finish
        finish[i] = true;
        temp1 = true;
        break;
      }
    }
  }

  // since no index has been found such that need <= work, we check the finish
  // array and return
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    if (!finish[i]) {
      // if system is in unsafe state
      return false;
    }
  }
  // if system is in safe state
  return true;
}


// customer requests specified number of resources or each type
int resource_request(int customer, int request[]){

  // if the customer is asking for more than it needs, return -1
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    if (request[i] > need[customer][i]) {
      return -1;
    }
  }

  // checks to see if the thread is requesting more resources than are availabe
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    if (request[i] > available[i]) {
      return -1;
    }
  }

  // stores old values
  int old_available[NUMBER_OF_RESOURCES];
  int old_allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
  int old_need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
      if (i == 0) {
        old_available[j] = available[j];
      }
      old_allocation[i][j] = allocation[i][j];
      old_need[i][j] = need[i][j];
    }
  }

  // updates values for bankers algorithm to check
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    available[i] = available[i] - request[i];
    allocation[customer][i] = allocation[customer][i] + request[i];
    need[customer][i] = need[customer][i] - request[i];
  }

  // checks to see if the request will put the system in an unsafe state
  bool is_safe = safety();

  // if the system will not be in a safe state, reset old values
  if (!is_safe) {
    cout << "System is not safe, resetting values" << endl;
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
      for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        if (i == 0) {
          available[j] = old_available[j];
        }
        allocation[i][j] = old_allocation[i][j];
        need[i][j] = old_need[i][j];
      }
    }
    return 0;
  }

  // otherwise, new values are left and function returns
  return 1;
}


// customer releases specified number of resources of each type
int resource_release(int customer, int release[]) {

  // if the customer is trying to release more resources than they have, fails
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    if (allocation[customer][i] < release[i]) {
      cout << "Invalid release request" << endl;
      return -1;
    }
  }

  // if the request is valid, releases resources
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    allocation[customer][i] -=  release[i];
  }
  cout << "Resources released" << endl;
  return 1;
}


// simulates customer, making random resource requests and randomly releasing
// held resources
void *customer(void *arg) {
  int *temp = (int *)arg;
  int index = *temp;

  cout << "Starting while loop for customer " << index << endl;
  while (true) {

    // generates randon number to decide whether to request or release
    int r_or_r = rand() % 10;

    if (r_or_r > 5) {
      //request
      cout << "customer " << index << " making a request" << endl;

      // generates array for request
      int r[NUMBER_OF_RESOURCES];
      for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (need[index][i] > 1) {
          r[i] = rand() % need[index][i];
        } else if (need[index][i] == 1) {
          r[i] = 1;
        }
      }

      // if the other customers aren't making a request or release, request
      int temp = pthread_mutex_trylock(&mutex);
      if (temp != 0) {
        continue;
      }
      resource_request(index, r);
      pthread_mutex_unlock(&mutex);

    } else {
      //release
      cout << "customer " << index << " making a release" << endl;

      // generates array for release
      int r[NUMBER_OF_RESOURCES];
      for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (allocation[index][i] > 1) {
          r[i] = rand() % allocation[index][i];
        } else if (allocation[index][i] == 1) {
          r[i] = 1;
        }
      }

      // if the other customers aren't making a request or release, release
      int temp = pthread_mutex_trylock(&mutex);
      if (temp != 0) {
        cout << "Mutex locked, waiting" << endl;
        continue;
      }
      resource_request(index, r);
      pthread_mutex_unlock(&mutex);
    }
    // sleeps so threads aren't constantly requesting and releasing
    sleep(1.5);
  }
}


// main method
int main(int argc, char* argv[]) {

  // checks for valid command line arguments
  if (argc != NUMBER_OF_RESOURCES + 1) {
    cout << "Invalid number of arguments" << endl;
    return 0;
  }

  // seed random function
  srand(time(NULL));

  // handle command line arguments
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    available[i] = atoi(argv[i+1]);
  }

  // customer threads
  pthread_t customers[NUMBER_OF_CUSTOMERS];

  // starts customer threads
  cout << "Initializing threads" << endl;
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    int * temp = (int *)malloc(sizeof(int));
    *temp = i;
    cout << "Initializing thread " << i << endl;
    int return_val = pthread_create(&customers[i], NULL, customer, (void *)(temp));
  }

  // waits a bit so customers can do their thing
  sleep(10);

  //kills customer threads and returns
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    pthread_cancel(customers[i]);
  }
  return 1;
}
