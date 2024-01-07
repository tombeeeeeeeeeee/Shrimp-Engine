#include "Utilities.h"

bool FileToString(string path, string* file, bool fileToConsole)
{
    fstream new_file;

    new_file.open(path, ios::in);
    // Checking whether the file is open.
    if (new_file.is_open()) {

        *file = "";

        if (fileToConsole)
        {
            ColouredOutput("Reading text from: ", darkCyan, false);
            ColouredOutput(path, green);
        }
        string line;
        // Read data from the file object and put it into a string.
        while (getline(new_file, line)) {
            // Print the data of the string.
            if (fileToConsole) cout << line << "\n";
            *file += line + "\n";
        }

        new_file.close();
        return true;
    }
    else ColouredOutput("Failed to read file: " + path, red);

    return false;
}

 bool FileToStringVector(string path, vector<string>* file, bool fileToConsole)
{
    vector<string> textFromFile;

    fstream new_file;
    
    new_file.open(path, ios::in);

    // Checking whether the file is open.
    if (new_file.is_open()) {

        if (fileToConsole)
        {
            ColouredOutput("Reading text from: ", darkCyan, false);
            ColouredOutput(path, green);
        }

        string line;
        // Read data from the file object and put it into a string.
        while (getline(new_file, line)) {
            // Print the data of the string.
            if(fileToConsole) cout << line << "\n";
            file->push_back(line);
        }

        new_file.close();
        return true;
    }
    else ColouredOutput("Failed to read file: " + path, red);

    return false;
}



vector<string> StringSplit(string toSplit, string splitAt)
{
    vector<string> split;

    //begin iterating through string
    string::iterator it = toSplit.begin();
    string splitted = "";
    while (it != toSplit.end())
    {
        //if the char is on the list of splitters
        if (splitAt.find(*it) != string::npos) 
        {
            if (splitted != "")
            {
                split.push_back(splitted);
                splitted = "";
            }
        }
        else
            splitted.push_back(*it);
        it++;
    }
    split.push_back(splitted);

    return split;
}

bool StringToNumber(string str, int* result)
{
    //Removes non numbers from string
    str = regex_replace(str, regex(R"([^0-9])") , "");
    if (str != "") *result = stoi(str);
    return str != "";
}

bool StringVectorToIntVector(vector<string> str, vector<int>* ints)
{
    for (string toDigitise : str)
    {
        int digit = -1;
        if (StringToNumber(toDigitise, &digit))
            ints->push_back(digit);
    }
    return ints->size() > 0;
}

