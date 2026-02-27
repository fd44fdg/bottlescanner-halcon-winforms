#include <iostream>
#include <string>

#include "../../BottleScanner.h"

int main(int argc, char* argv[])
{
    if (argc < 2) return 1;
    BottleScanner scanner;
    scanner.SetBaseSLowThreshold(80);        // Match HDevelop baseline
    scanner.SetAdaptiveThresholdEnabled(false); // Disable adaptation for A/B comparison
    scanner.SetPreFilterMinArea(0);         // Disable coarse pre-filter for diagnosis
    scanner.SetMinArea(500);                // Balance under-count and over-count
    scanner.SetMinCircularity(0.10);        // Slightly relax final circularity threshold
    int count = scanner.DetectCaps(argv[1]);
    std::cout << "Detected caps: " << count << std::endl;
    return (count >= 0) ? 0 : 1;
}
