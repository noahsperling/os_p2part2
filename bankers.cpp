#include <iostream>
#include <stdlib.h>
#include <pthread.h>

/*
* @authors Noah Sperling and Alexander Nowlin
* @version 04/19/2018
*/

using namespace std;

#define NUMBER_OF_CUSTOMERS 2
#define NUMBER_OF_RESOURCES 2

int available[NUMBER_OF_RESOURCES];

int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {{2,2},{2,2}};

int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {{0,0},{0,0}};

int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {{2,2},{2,2}};

bool safety()
{
  int work[NUMBER_OF_RESOURCES];
  bool finish[NUMBER_OF_CUSTOMERS];

  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    work[i] = available[i];
  }

  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    finish[i] = false;
  }

  cout << "finish array : ";
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    if(i != NUMBER_OF_CUSTOMERS - 1) {
      cout << finish[i] << ", ";
    } else {
      cout << finish[i] << endl;
    }
  }

  // whether an index has been found s.t. need <= work
  bool temp1 = true;

  // loops until an no index exists such that need <= work
  while (temp1) {

    // no index has been found yet this iteration
    temp1 = false;

    // loops through indices to find one where need <= work
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
      bool temp = false;

      // if this one has already been assessed, continue
      if (finish[i])
      {
        continue;
      }

      // checks an index to see if need <= work
      for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
      {
        if (need[i][j] <= work[j])
        {
          temp = true;
        }
        else
        {
          temp = false;
          cout << "need is not less than work at this index!" << endl;
          break;
        }
      }

      // at this point, it has been determined that need <= work
      if (temp)
      {
        cout << "updating work and finish!" << endl;
        // updates work
        for (int k = 0; k < NUMBER_OF_RESOURCES; k++) {
          work[k] = work[k] + allocation[i][k];
        }
        // update finish
        finish[i] = true;

        // we found an index
        cout << "We found an index where need <= work, so iterate again!" << endl;
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

  int[] old_available = available;
  int[][] old_allocation = allocation;
  int[][] old_need = need;


  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    available[i] = available[i] - request[i];
    allocation[customer][i] = allocation[customer][i] + request[i];
    need[customer][i] = need[customer][i] - request[i];
  }

  bool is_safe = safety();
  if (!is_safe) {
    available = old_available;
    need = old_need;
    allocation = old_allocation;
    return 0;
  }

  return 1;
}

int main(int argc, char* argv[])
{
  if (argc != NUMBER_OF_RESOURCES + 1)
  {
    cout << "Invalid number of arguments" << endl;
    return 0;
  }

  for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
  {
    available[i] = atoi(argv[i+1]);
  }

  cout << "System is Safe: " << safety() << endl;
  //pthread_t customers[NUMBER_OF_CUSTOMERS];
  return 0;
}
