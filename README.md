# Project_DS

```markdown
# Keyword-Based Large-Scale Document Search

This C++ program performs keyword-based large-scale document search using TF-IDF (Term Frequency-Inverse Document Frequency) scoring.

## Description

The program takes as input a list of keywords and returns the top matching documents from a collection of text documents. It calculates the relevance score for each document based on the TF-IDF algorithm and the input keywords.

## Usage

Compile the program using a C++ compiler. For example:

```bash
g++ -o search main.cpp
```

Then, run the executable with the following command:

```bash
./search NUM K1 K2 ... Km
```

Where:
- `NUM` is the number of search results to be returned.
- `K1`, `K2`, ..., `Km` are the keywords to search for.

## Input Files

The program expects the following input files to be present in the same directory:

- `article.txt`: Text file containing the collection of documents. Each document should be separated by a page break character (\f), and each document should start with a unique identifier.
- `dictionary.txt`: Text file containing the dictionary of words.
- `stopwords.txt`: Text file containing the list of stopwords.

## Output

The program outputs the top matching documents to the console and saves the results to a file named `results.txt`.

## Example

Assuming the compiled executable is named `search`, you can run the program as follows:

```bash
./search 100 edu news article
```

