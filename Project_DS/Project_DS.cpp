#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

// Function to load words from a file into a set
unordered_set<string> loadWords(const string& filename) {
    unordered_set<string> words;
    ifstream file(filename);
    string word;
    while (file >> word) {
        words.insert(word);
    }
    return words;
}

// Function to convert a string to lowercase
string toLower(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Function to tokenize and preprocess a document
vector<string> preprocessDocument(const string& text, const unordered_set<string>& dictionary, const unordered_set<string>& stopwords) {
    vector<string> words;
    istringstream stream(text);
    string word;
    while (stream >> word) {
        // Remove non-letter characters and convert to lowercase
        word.erase(remove_if(word.begin(), word.end(), [](char c) { return !isalpha(c); }), word.end());
        word = toLower(word);
        if (dictionary.find(word) != dictionary.end() && stopwords.find(word) == stopwords.end()) {
            words.push_back(word);
        }
    }
    return words;
}

// Function to calculate term frequency
unordered_map<string, double> calculateTF(const vector<string>& words) {
    unordered_map<string, int> wordCounts;
    for (const auto& word : words) {
        wordCounts[word]++;
    }
    unordered_map<string, double> tf;
    int totalWords = words.size();
    for (const auto& pair : wordCounts) {
        tf[pair.first] = (static_cast<double>(pair.second) / totalWords) * 100;
    }
    return tf;
}

// Function to calculate IDF
unordered_map<string, double> calculateIDF(const vector<unordered_map<string, double>>& tfList, int totalDocuments) {
    unordered_map<string, int> docCounts;
    for (const auto& tf : tfList) {
        for (const auto& pair : tf) {
            docCounts[pair.first]++;
        }
    }
    unordered_map<string, double> idf;
    for (const auto& pair : docCounts) {
        idf[pair.first] = log10(static_cast<double>(totalDocuments) / pair.second);
    }
    return idf;
}

// Function to calculate the relevance score for a document
double calculateRelevance(const unordered_map<string, double>& tf, const unordered_map<string, double>& idf, const vector<string>& keywords) {
    double score = 0.0;
    for (const auto& keyword : keywords) {
        if (tf.find(keyword) != tf.end() && idf.find(keyword) != idf.end()) {
            score += tf.at(keyword) * idf.at(keyword);
        }
    }
    return score;
}

// Function to load documents from a file
vector<pair<string, string>> loadDocuments(const string& filename) {
    vector<pair<string, string>> documents;
    ifstream file(filename);
    string line;
    stringstream buffer;
    string docID;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (line[0] == '\f') {
            if (!docID.empty()) {
                documents.emplace_back(docID, buffer.str());
                buffer.str(string());
                buffer.clear();
            }
            getline(file, docID);
        }
        else {
            buffer << line << " ";
        }
    }
    if (!docID.empty()) {
        documents.emplace_back(docID, buffer.str());
    }
    return documents;
}

// Function to output the top N results
void outputResults(const vector<pair<double, pair<int, string>>>& results, int numResults) {
    ofstream outFile("results.txt");
    for (int i = 0; i < min(numResults, (int)results.size()); ++i) {
        cout << fixed << setprecision(6) << results[i].first << " " << results[i].second.first << " " << results[i].second.second << endl;
        outFile << fixed << setprecision(6) << results[i].first << " " << results[i].second.first << " " << results[i].second.second << endl;
    }
    outFile.close();
}

// Main function
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " NUM K1 K2 ... Km" << endl;
        return 1;
    }

    int numResults = stoi(argv[1]);
    vector<string> keywords;
    for (int i = 2; i < argc; ++i) {
        keywords.push_back(toLower(argv[i]));
    }

    // Load dictionary and stopwords
    unordered_set<string> dictionary = loadWords("dictionary.txt");
    unordered_set<string> stopwords = loadWords("stopwords.txt");

    // Load documents
    vector<pair<string, string>> documents = loadDocuments("article.txt");
    int totalDocuments = documents.size();

    // Process documents
    vector<unordered_map<string, double>> tfList;
    for (const auto& doc : documents) {
        vector<string> words = preprocessDocument(doc.second, dictionary, stopwords);
        tfList.push_back(calculateTF(words));
    }

    // Calculate IDF
    unordered_map<string, double> idf = calculateIDF(tfList, totalDocuments);

    // Calculate relevance scores for each document
    vector<pair<double, pair<int, string>>> results;
    for (int i = 0; i < documents.size(); ++i) {
        double relevance = calculateRelevance(tfList[i], idf, keywords);
        if (relevance > 0) {
            results.emplace_back(relevance, make_pair(i + 1, documents[i].first));
        }
    }

    // Sort results by relevance score in descending order
    sort(results.begin(), results.end(), [](const pair<double, pair<int, string>>& a, const pair<double, pair<int, string>>& b) {
        if (abs(a.first - b.first) > 1e-6) return a.first > b.first;
        return a.second.first < b.second.first;
        });

    // Output results
    outputResults(results, min(numResults, 100));

    return 0;
}
