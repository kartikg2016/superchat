#include <stdlib.h>
#include <iostream>

#include "model.h"

using namespace std;

//----------FUNCTION PROTOTYPES----------
void printResult(int test_num, bool passed);
void runTests();
void testIsUserNew();
void testFindUserIndex();
void testCalculateNumUsersInChatRoom();

//----------GLOBAL VARIABLES----------
Model m;
bool fail_exit = false;

/* Template for model after populateForTesting(-1)
0)  Name: me       ~ UUID: 0  ~ CRI: 0
1)  Name: Joe      ~ UUID: 1  ~ CRI: 0
2)  Name: Robert   ~ UUID: 2  ~ CRI: 0 
3)  Name: Rivka    ~ UUID: 3  ~ CRI: 0
4)  Name: Ramon    ~ UUID: 4  ~ CRI: 1
5)  Name: Stephani ~ UUID: 5  ~ CRI: 1
6)  Name: Jewel    ~ UUID: 6  ~ CRI: 1
7)  Name: Isaias   ~ UUID: 7  ~ CRI: 2
8)  Name: Murray   ~ UUID: 8  ~ CRI: 2
9)  Name: Darell   ~ UUID: 9  ~ CRI: 2
10) Name: Alyce    ~ UUID: 10 ~ CRI: 3
11) Name: Carylon  ~ UUID: 11 ~ CRI: 3
12) Name: Dona     ~ UUID: 12 ~ CRI: 3
13) Name: George   ~ UUID: 13 ~ CRI: 4 
14) Name: Doug     ~ UUID: 14 ~ CRI: 4
15) Name: Hannah   ~ UUID: 15 ~ CRI: 4 
*/

int main()
{
	m.populateForTesting(-1);
	runTests();
	if (fail_exit) exit(EXIT_FAILURE);
	return 0;
}

void printResult(int test_num, bool passed)
{
	printf("\tTest #%d: ", test_num);
	if(passed) printf("PASSED");
	else 
	{
		printf("******FAILED******");
		fail_exit = true;
	}
	printf("\n");
}

void runTests()
{
	testIsUserNew();
	testFindUserIndex();
	testCalculateNumUsersInChatRoom();
}

void testIsUserNew()
{
	printf("Testing isUserNew()\n");
	if(m.isUserNew(0) == false) printResult(1,1);
	else printResult(1,0); 
	if(m.isUserNew(16) == true) printResult(2,1);
	else printResult(2,0); 
	if(m.isUserNew(5) == false) printResult(3,1);
	else printResult(3,0); 
	if(m.isUserNew(3) == false) printResult(4,1);
	else printResult(4,0); 
}

void testFindUserIndex()
{
	printf("Testing findUserIndex()\n");
	if(m.findUserIndex(0) == 0) printResult(1,1);
	else printResult(1,0); 
	if(m.findUserIndex(15) == 15) printResult(2,1);
	else printResult(2,0); 
	if(m.findUserIndex(16) == -1) printResult(3,1);
	else printResult(3,0); 
	if(m.findUserIndex(50) == -1) printResult(4,1);
	else printResult(4,0); 
}

void testCalculateNumUsersInChatRoom()
{
	printf("Testing calculateNumUsersInChatRoom()\n");
	if(m.calculateNumUsersInChatRoom(0) == 4) printResult(1,1);
	else printResult(1,0); 
	if(m.calculateNumUsersInChatRoom(1) == 3) printResult(2,1);
	else printResult(2,0); 
	if(m.calculateNumUsersInChatRoom(5) == 0) printResult(3,1);
	else printResult(3,0); 
	if(m.calculateNumUsersInChatRoom(10) == 0) printResult(4,1);
	else printResult(4,0); 
}