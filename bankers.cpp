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


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int available[NUMBER_OF_RESOURCES];

int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {{2,2},{2,2}};

int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {{1,0},{1,1}};

int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {{1,2},{1,1}};

bool safety() {
  int work[NUMBER_OF_RESOURCES];
  bool finish[NUMBER_OF_CUSTOMERS];

  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    work[i] = available[i];
  }

  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    finish[i] = false;
  }

  /*
  cout << "finish array : ";
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    if(i != NUMBER_OF_CUSTOMERS - 1) {
      cout << finish[i] << ", ";
    } else {
      cout << finish[i] << endl;
    }
  }
  */

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
          // cout << "need is not less than work at this index!" << endl;
          break;
        }
      }

      // at this point, it has been determined that need <= work
      if (temp) {
        // cout << "updating work and finish!" << endl;
        // updates work
        for (int k = 0; k < NUMBER_OF_RESOURCES; k++) {
          work[k] = work[k] + allocation[i][k];
        }
        // update finish
        finish[i] = true;

        // we found an index
        // cout << "We found an index where need <= work, so iterate again!" << endl;
        temp1 = true;
        break;
      }
    }
  }
  cout << "finish array : ";
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    if(i != NUMBER_OF_CUSTOMERS - 1) {
      cout << finish[i] << ", ";
    } else {
      cout << finish[i] << endl;
    }
  }

  // since no index has been found such that need <= work, we check the finish
  // array and return
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    if (!finish[i]) {
      return false;
    }
  }
  return true;
}

int resource_request(int customer, int request[]){
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    if (request[i] > need[customer][i]) {
      return -1;
    }
  }

  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    if (request[i] > available[i]) {
      return -1;
    }
  }

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


  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    available[i] = available[i] - request[i];
    allocation[customer][i] = allocation[customer][i] + request[i];
    need[customer][i] = need[customer][i] - request[i];
  }

  bool is_safe = safety();
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

  return 1;
}

int resource_release(int customer, int release[]) {
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    if (allocation[customer][i] < release[i]) {
      cout << "Invalid release request" << endl;
      return -1;
    }
  }
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    allocation[customer][i] -=  release[i];
  }
  cout << "Resources released" << endl;
  return 1;
}


void *customer(void *arg) {
  int *temp = (int *)arg;
  int index = *temp;

  cout << "Starting while loop for customer " << index << endl;
  while (true) {
    int r_or_r = rand() % 10;
    if (r_or_r > 5) {
      //request
      cout << "customer " << index << " making a request" << endl;
      int r[NUMBER_OF_RESOURCES];
      for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (need[index][i] > 1) {
          r[i] = rand() % need[index][i];
        } else if (need[index][i] == 1) {
          r[i] = 1;
        }
      }
      int temp = pthread_mutex_trylock(&mutex);
      if (temp != 0) {
        continue;
      }
      resource_request(index, r);
      pthread_mutex_unlock(&mutex);
    } else {
      //release
      cout << "customer " << index << " making a release" << endl;
      int r[NUMBER_OF_RESOURCES];
      for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (allocation[index][i] > 1) {
          r[i] = rand() % allocation[index][i];
        } else if (allocation[index][i] == 1) {
          r[i] = 1;
        }
      }
      int temp = pthread_mutex_trylock(&mutex);
      if (temp != 0) {
        cout << "Mutex locked, waiting" << endl;
        continue;
      }
      resource_request(index, r);
      pthread_mutex_unlock(&mutex);
    }
    sleep(1.5);
  }
}

int main(int argc, char* argv[]) {
  if (argc != NUMBER_OF_RESOURCES + 1) {
    cout << "Invalid number of arguments" << endl;
    return 0;
  }

  cout << "Hello" << endl;

  // seed random function
  srand(time(NULL));

  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    available[i] = atoi(argv[i+1]);
  }

  pthread_t customers[NUMBER_OF_CUSTOMERS];

  cout << "Initializing threads" << endl;
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    int * temp = (int *)malloc(sizeof(int));
    *temp = i;
    cout << "Initializing thread " << i << endl;
    int return_val = pthread_create(&customers[i], NULL, customer, (void *)(temp));
  }

  sleep(10);

  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    pthread_cancel(customers[i]);
  }
  /*
  int request[NUMBER_OF_RESOURCES] = {1,0};

  int release = resource_release(0, request);

  cout << "System is Safe: " << resource_request(0, request) << endl;
  //pthread_t customers[NUMBER_OF_CUSTOMERS];
  */

}
