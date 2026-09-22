#include <iostream>
#include <string>
#include <fstream>
using namespace std;

int main() {

    // open csv file
    fstream MyFile("country_stats_simple.csv");
    
    // line count of file
    int lineCount = 0;
    string line;
    while (getline(MyFile, line)) {
        lineCount++;
    }

    // reset stream
    MyFile.clear();
    MyFile.seekg(0);
    
    // num of records pulled
    const int numOfRecords = lineCount - 1; // header is not a record

    // array declaration
    string countries[numOfRecords];
    int sqKm[numOfRecords];
    int population[numOfRecords];

    // skip header row
    string header;
    getline(MyFile, header);

    // read data in
    for (int i = 0; i < numOfRecords; i++) {
        getline(MyFile, countries[i], ','); // country name, column 1
        MyFile >> sqKm[i]; // country size, column 2 - outputs an int
        MyFile.ignore(); // ignore 1 character, comma 
        MyFile >> population[i]; // population, column 3
        MyFile.ignore(); //ignore 1 character
    }

    // close file after finished importing data
    MyFile.close();

    // display the data
    cout << " - Country data loaded: " << endl;
    
    for (int i = 0; i < numOfRecords; i++) {
        cout << countries[i] << " | country size (sqKm): " << sqKm[i] << " | population: " << population[i] << endl;
    };

    // use pointer to access data value
    string * popPtr = &countries[4]; // create pointer for random value
    cout << " " << endl;
    cout << "The following value came from a pointer: " << *popPtr << endl;

}