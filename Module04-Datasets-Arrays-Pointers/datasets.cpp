#include <iostream>
using namespace std;

int main() {
    string countries[10] = {"Russia", "Canada", "China", "United States", "Brazil", "Australia", "India", "Argentina", "Kazakhstan", "Algeria"};
    int sqKm[10] = {17098246, 9984670, 9596960, 9525067, 8510346, 7741220, 3287263, 2780400, 2724910, 2381714};
    int population[10] = {146028325, 41417056, 1404890000, 341784857, 214211951, 27801023, 142904000, 46466688, 20604819, 47400000};

    int *popPtr = &population[0];

    for (int i = 0; i < 10; i++) {
        cout << countries[i] << " | country size (sqKm): " << sqKm[i] << " | population: " << population[i] << endl;
    }

    cout << "Russia's population, retrieved with a pointer: " << *popPtr << endl;
    return 0;
    
}