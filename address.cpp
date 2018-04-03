#include <iostream>

/*
* @authors Alexander Nowlin and Noah Sperling
* @version 04/03/2018
*/

using namespace std;

int main(int argc, char *argv[]) {
  if(argc != 2) {
    cout << "\nInvalid Arguments\n" << endl;
    return 0;
  }

  // casts address from string to int
  int address = atoi(argv[1]);

  // makes a copy
  int a_copy = address;

  // to store the binary version of address
  char bin_addr[] = "00000000000000000000000000000000";

  //converts address to binary
  int i;
  for(i = 31; i > -1; i--) {
    if (a_copy % 2) {
      bin_addr[i] = '1';
    }
    a_copy /= 2;
  }

  // to store offset and page number
  char offset[] = "000000000000";
  char page[] = "00000000000000000000";

  // copies binary offset and page number
  for(i = 31; i > -1; i--) {
    if(i >= 20) {
      offset[i - 20] = bin_addr[i];
    } else {
      page[i] = bin_addr[i];
    }
  }

  // converts offset and page number to ints
  int p_int = std::stoi(page, nullptr, 2);
  int o_int = std::stoi(offset, nullptr, 2);

  // prints results
  cout << "The address " << address << " contains: " << endl;
  cout << "Page Number: " << p_int << endl;
  cout << "Offset: " << o_int << endl;
}
