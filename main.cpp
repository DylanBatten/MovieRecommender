#include "./RunGraph.h"

int main() {
    std::cout << "Enter 1 for graph: ";
    char choice;
    std::cin >> choice;
    if (choice == '1') {
        runGraph();
    }

    return 0;
}