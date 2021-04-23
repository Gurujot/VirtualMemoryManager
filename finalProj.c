/*
Gurujot Singh
Final Project
April 1st, 2021
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define pageTableSize 256
#define bufferSize 256
#define physicalMemorySize 256

int readDisk(int pageNum, char *PhysicalMemory, int* openFrame);
int findPage(int logicalAddress, char* pageTable, char* PhysicalMemory, int* openFrame, int* pageFaults);


int main (int argc, char* argv[]) {
    // declare vars
	int val;
	int openFrame = 0;
	int pageFaults = 0;
	int inputCount = 0;
	float pageFaultRate;
    FILE *fileDescriptor;

	unsigned char PageTable[pageTableSize];
	memset(PageTable, -1, sizeof(PageTable));	

	char PhyMem[physicalMemorySize][physicalMemorySize]; // 2d array of 256x256

	if (argc < 2 || argc > 2){ // check to see if there are 2 arguments, error if < 2 <, exit if less or more
		printf("Error! Need 2 arguments.\n");
		exit(0);
	}
    
    if ((fileDescriptor = fopen(argv[1], "r")) == NULL) { // if NULL, exit
        printf("Error! File could not be opened.\n");
		exit(0);
    }

	while (fscanf(fileDescriptor, "%d", &val) == 1){ // scan addresses.txt for addresses
		findPage(val, PageTable, (char*)PhyMem, &openFrame, &pageFaults); // call function findPage
		inputCount++; // increment counter
	}

	pageFaultRate = (float)pageFaults / (float)inputCount; // pagefaultrate calculated by dividing pageFaults by inputCount
    printf("Number of Translated Addresses = %d\n", inputCount); // print num of translated addresses
    printf("Page faults = %d\n", pageFaults); // print page faults
	printf("Page Fault Rate = %.3f\n",pageFaultRate); // print page fault rate
	fclose(fileDescriptor);

	return 0;
}

int readDisk (int pageNum, char *PhysicalMemory, int* openFrame) { // called when requested page not in main memory
    // declare vars
	char buffer[bufferSize];
	memset(buffer, 0, sizeof(buffer));
	FILE *backingStore;

	if ((backingStore = fopen("BACKING_STORE.bin", "rb")) == NULL) { // open backingstore.bin
		printf("Error! File could not be opened.\n"); // if cannot be opened print error
		exit(0); // exit
	}
	
	if (fseek(backingStore, pageNum * physicalMemorySize, SEEK_SET) != 0) { 
		printf("error in fseek\n");
	}

	if (fread(buffer, sizeof(char), physicalMemorySize, backingStore) == 0) {
		printf("error in fread\n");
	}

	for (int i=0; i<physicalMemorySize; i++) {
		*((PhysicalMemory+(*openFrame)*physicalMemorySize)+i) = buffer[i];
	}
	
	(*openFrame)++;

	return (*openFrame) - 1;
}


int findPage(int logicalAddress, char* pageTable, char* PhysicalMemory, int* openFrame, int* pageFaults){
    // declare vars
    int frame = 0;
    int newFrame = 0;
    int value;
    
	unsigned char mask = 0xFF; //16-bit mask
	unsigned char offset;
	unsigned char pageNum;
    
	printf("Virtual address: %d\t", logicalAddress); // read logical addresses from addresses.txt

	pageNum = (logicalAddress >> 8) & mask; // get page using bit shifting and masking op
	offset = logicalAddress & mask; // get offset using bit shifting and masking op

	if (pageTable[pageNum] != -1) {  // Check if in PageTable
		frame = pageTable[pageNum];
	} 
	else {  // read from backing_store.bin
		newFrame = readDisk(pageNum, PhysicalMemory, openFrame); // call function readDisk to get address from backing_store.bin
		pageTable[pageNum] = newFrame; // set pageTable[pageNum] to the newFram which was retrieved from backing_store.bin
		frame = pageTable[pageNum]; // set fram to pageTable[pageNum]
		(*pageFaults)++; // increment pageFaults
	}
		
	int index = ((unsigned char)frame*physicalMemorySize)+offset; // index found by multiplying frame by physical memory size and adding offset
	value = *(PhysicalMemory+index);
	printf("Physical Address: %d\t",index); // print physical address
    printf("Value: %d\n", value); // print value
	
	return 0;
}
