#include <iostream>
#include <vector>
#include <fstream>
#include "bits-stdc++.h"
#include <sstream>

using namespace std;

string InttoString(int a)
{
    ostringstream temp;
    temp << a;

    return temp.str();
}

vector<string> split(const string& str, const string& delim)
{
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos)
        {
            pos = str.length();
        }
        string token = str.substr(prev, pos - prev);
        if (!token.empty())
        {
            tokens.push_back(token);
        }
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());

    return tokens;
}

template<typename T>
void drop_rows(vector<vector<T>>& tdata, const int& i)
{
    tdata[i].clear();
}

template<typename T>
void shuffle_rows(vector<vector<T>>& tdata, time_t seed)
{
    srand((unsigned)seed);
    vector<T> saved;
    for (int i = 1; i < tdata.size(); i++)
    {
        int r = rand() % tdata.size();
        if (r != i && r != 0)
        {
            for (int j = 0; j < tdata[i].size(); j++)
            {
                saved.push_back(tdata[i][j]);
            }
            drop_rows(tdata, i);
            for (int j = 0; j < saved.size(); j++)
            {
                tdata[i].push_back(tdata[r][j]);
            }
            drop_rows(tdata, r);
            for (int j = 0; j < saved.size(); j++)
            {
                tdata[r].push_back(saved[j]);
            }
            saved.clear();

        }

    }
}

void encode_target_variable(vector<vector<string>>& tdata)
{
    set<string> mp;
    for (int i = 1; i < tdata.size(); i++)
    {
        mp.insert(tdata[i][tdata[i].size() - 1]);
    }
    double y = 1;
    for (auto& x : mp)
    {
        for (int i = 1; i < tdata.size(); i++)
        {
            if (tdata[i][tdata[i].size() - 1] == x)
            {
                tdata[i][tdata[i].size() - 1] = InttoString(y);
            }
        }
        y++;
    }

}

vector<vector<double>> readIrisdata(const char* fname)
{
    vector<string> data;
    vector<vector<string>> tdata;
    ifstream file(fname);
    string line = "";

    while (getline(file, line))
    {
        const std::string s = ", ";
        const std::string t = " ";

        std::string::size_type n = 0;
        while ((n = line.find(s, n)) != std::string::npos)
        {
            line.replace(n, s.size(), t);
            n += t.size();
        }
        if (line[line.length() - 1] == ',')
        {
            line = line + ',';

        }
        const std::string s1 = ",,";
        const std::string t1 = ",nan,";

        std::string::size_type n1 = 0;
        while ((n1 = line.find(s1, n1)) != std::string::npos)
        {
            line.replace(n1, s1.size(), t1);
            n1 += t1.size();
        }

        data.push_back(line);

    }

    file.close();

    tdata.resize(data.size() - 1);

    for (int i = 1; i < data.size(); i++)
    {
        vector<string> str = split(data[i], ",");
        for (int j = 0; j < str.size(); j++)
        {
            tdata[i - 1].push_back(str[j]);
        }
    }

    encode_target_variable(tdata);

    vector<vector<double>> tdataN;
    tdataN.resize(tdata.size());

    for (int i = 0; i < tdata.size(); i++)
    {
        for (int j = 0; j < tdata[i].size(); j++)
        {
            tdataN[i].push_back(atof(tdata[i][j].c_str()));
        }
    }
    shuffle_rows(tdataN, time(0));

    return tdataN;
}

double cal_euclidean_distances(const double x1, const double x2)
{
    return pow((x1 - x2), 2);
}

bool sortCol(const vector<double>& v1, const vector<double>& v2)
{
    return v1[0] < v2[0];
}

vector<double> K_nearest_neighbour(vector<vector<double>>& tdata, vector<vector<double>>& testdata, const int k)
{
    vector<double> labels;
    double distance = 0;
    for (int y = 0; y < testdata.size(); y++)
    {
        vector<vector<double>> distances(tdata.size(), vector<double>(2));
        for (int i = 0; i < tdata.size(); i++)
        {
            distance = 0;
            for (int j = 0; j < tdata[i].size() - 1; j++)
            {
                distance += cal_euclidean_distances(tdata[i][j], testdata[y][j]);
            }

            distances[i][0] = sqrt(distance);
            distances[i][1] = tdata[i][tdata[i].size() - 1];

        }

        sort(distances.begin(), distances.end(), sortCol);
        map<double, int> mp;
        for (int n = 0; n < k; n++)
        {
            mp[distances[n][1]]++;
        }

        vector<pair<int, double>> mpflipped;

        for (auto& x : mp)
        {
            mpflipped.push_back(make_pair(x.second, x.first));

        }
        sort(mpflipped.begin(), mpflipped.end());
        labels.push_back((*next(mpflipped.begin(), mpflipped.size() - 1)).second);

    }

    return labels;
}

int main()
{
    vector<vector<double>> tdata = readIrisdata("iris.data");

    vector<vector<double>> testdata = { {6.0,3.3,2.3,0.7,-1},
                                       {6.0,0.3,1.2,2,-1} };

    vector<double> labels = K_nearest_neighbour(tdata, testdata, 4);

    for (int i = 0; i < labels.size(); i++)
    {
        cout << labels[i] << endl;
    }

    return 0;
}
