//
//  main.cpp
//
//
//  Created by Aristomenis Zazanis on 7/2/24.
//

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cctype>
#include <limits>
#include <algorithm>

// English letter frequencies as a percentage of text, used for frequency analysis
const std::vector<double> englishLetterFreqs = {
    8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015, 6.094, 6.966,
    0.153, 0.772, 4.025, 2.406, 6.749, 7.507, 1.929, 0.095, 5.987,
    6.327, 9.056, 2.758, 0.978, 2.360, 0.150, 1.974, 0.074
};

// Calculates the Index of Coincidence (IoC) for a given text segment.
double indexOfCoincidence(const std::string& text) {
    std::vector<int> freq(26, 0); // Frequency of each letter in the segment
    for (char c : text) {
        if (isalpha(c)) {
            freq[toupper(c) - 'A']++; // Increment frequency for the letter
        }
    }
    double sum = 0.0;
    size_t N = text.length(); // Total number of letters in the segment
    for (int count : freq) {
        sum += count * (count - 1); // Sum of the products of each count and count - 1
    }
    return N > 1 ? sum / (N * (N - 1)) : 0; // Calculate and return the IoC
}

// Estimates the key length by sampling the ciphertext at intervals and analyzing the IoC for each sample.
int estimateKeyLength(const std::string& ciphertext) {
    const double threshold = 0.065; // Target IoC value close to that of English text
    int keyLength = 1; // Initial key length guess
    double maxIoC = 0.0; // Maximum IoC found

    for (int n = 1; n <= 20; ++n) { // Test key lengths up to 20
        std::string sample;
        for (size_t i = 0; i < ciphertext.length(); i += n) {
            sample += ciphertext[i]; // Take every nth character as a sample
        }
        double currentIoC = indexOfCoincidence(sample); // Calculate IoC for the sample
        // Update key length if the current IoC is closer to the threshold than the previous best
        if (std::abs(currentIoC - threshold) < std::abs(maxIoC - threshold)) {
            maxIoC = currentIoC;
            keyLength = n;
        }
        // Break early if IoC is significantly high, suggesting a likely key length
        if (currentIoC > 0.06) {
            break;
        }
    }
    return keyLength; // Return the estimated key length
}

// Finds the best shift (key character) for a given frequency distribution of a ciphertext segment.
char findBestShift(const std::vector<int>& freqs, int totalChars) {
    double minChiSquared = std::numeric_limits<double>::max();
    char bestShift = 'A'; // Initialize best shift
    for (int shift = 0; shift < 26; ++shift) { // Test all possible shifts (key characters)
        double chiSquared = 0.0; // Chi-squared statistic for this shift
        for (size_t i = 0; i < 26; ++i) {
            double observed = freqs[(i + shift) % 26]; // Observed frequency after applying shift
            double expected = totalChars * (englishLetterFreqs[i] / 100.0); // Expected frequency
            chiSquared += std::pow(observed - expected, 2) / expected; // Calculate chi-squared component
        }
        // Update best shift if this shift results in a lower chi-squared value
        if (chiSquared < minChiSquared) {
            minChiSquared = chiSquared;
            bestShift = shift;
        }
    }
    return 'A' + bestShift; // Return the character corresponding to the best shift
}

// Estimates the Vigenère cipher key by analyzing the frequency distribution of letters in each segment.
std::string estimateKey(const std::string& ciphertext, int keyLength) {
    std::string key; // String to store the estimated key
    for (int i = 0; i < keyLength; ++i) { // Iterate over each position in the key
        std::vector<int> freqs(26, 0); // Frequency of each letter for this key position
        int totalChars = 0; // Total characters considered for this key position
        for (size_t j = i; j < ciphertext.length(); j += keyLength) {
            if (isalpha(ciphertext[j])) {
                freqs[toupper(ciphertext[j]) - 'A']++; // Increment frequency for the letter
                totalChars++;
            }
        }
        char bestShift = findBestShift(freqs, totalChars); // Find the best shift (key character) for this position
        key += bestShift; // Append the best shift to the key
    }
    return key; // Return the estimated key
}

// Main function to drive the cipher-breaking process.
int main() {
    std::string ciphertext = "ciphertext"; // Placeholder for actual ciphertext
    std::string processedText; // Processed text for analysis
    for (char c : ciphertext) {
        if (isalpha(c)) {
            processedText += toupper(c); // Convert to uppercase and filter non-alphabetic characters
        }
    }

    int estimatedKeyLength = estimateKeyLength(processedText); // Estimate the key length
    std::cout << "Estimated Key Length: " << estimatedKeyLength << std::endl;

    std::string estimatedKey = estimateKey(processedText, estimatedKeyLength); // Estimate the key
    std::cout << "Estimated Key: " << estimatedKey << std::endl;

    return 0;
}
