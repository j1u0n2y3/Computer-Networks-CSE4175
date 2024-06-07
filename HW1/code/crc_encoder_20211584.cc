#include <fstream>
#include <iostream>
#include <string>
#include <bitset>
#include <sstream>

using namespace std;

string XOR(string a, string b)
{
    string res = "";
    for (int i = 1; i < b.length(); i++)
    {
        ostringstream oss;
        oss << (a[i] ^ b[i]);
        res += oss.str();
    }
    return res;
}

string binarydiv(string dividend, string divisor)
{
    int cur = divisor.length();
    string res = dividend.substr(0, cur);

    for (; cur <= dividend.length(); cur++)
    {
        if (res[0] == '1')
        {
            if (cur == dividend.length())
                res = XOR(divisor, res);
            else
                res = XOR(divisor, res) + dividend[cur];
        }
        else
        {
            if (cur == dividend.length())
                res = XOR(string(cur, '0'), res);
            else
                res = XOR(string(cur, '0'), res) + dividend[cur];
        }
    }
    return res;
}

string ctos(char ch)
{
    string res = "";
    for (int i = 7; i >= 0; --i)
    {
        if ((ch >> i) & 1)
            res += '1';
        else
            res += '0';
    }
    return res;
}

char stoc(string s)
{
    char res = 0;
    for (int i = 0; i < s.length(); ++i)
    {
        res *= 2;
        res += ((int)s[i] - '0');
    }
    return res;
}

string CRC(char c, string gen, int n)
{
    int len = gen.length();
    string tmp = ctos(c);
    string res;

    if (n == 4)
    {
        string t1 = tmp.substr(0, 4);
        string t2 = tmp.substr(4, 4);
        string r1 = binarydiv(t1 + string(len - 1, '0'), gen);
        string r2 = binarydiv(t2 + string(len - 1, '0'), gen);
        res = t1 + r1 + t2 + r2;
    }
    else
    {
        string t = tmp;
        string r = binarydiv(t + string(len - 1, '0'), gen);
        res = t + r;
    }

    return res;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("usage: ./crc_encoder input_file output_file generator dataword_size\n");
        return 1;
    }

    string ifn = argv[1];
    string ofn = argv[2];
    string gen = argv[3];
    string dataword_size = argv[4];

    ifstream input(ifn.data());

    if (!input.is_open())
    {
        printf("input file open error.\n");
        input.close();
        return 1;
    }

    ofstream output(ofn.data());

    if (!output.is_open())
    {
        printf("output file open error.\n");
        output.close();
        return 1;
    }

    int dsize;
    if (dataword_size.compare("4") == 0)
        dsize = 4;
    else if (dataword_size.compare("8") == 0)
        dsize = 8;
    else
    {
        printf("dataword size must be 4 or 8.\n");
        return 1;
    }

    char c;
    string res = "";
    while (input.get(c))
        res += CRC(c, gen, dsize);

    int padsize;
    if (res.length() % 8 != 0)
    {
        padsize = 8 - (res.length() % 8);
        res = ctos(padsize) + res;
        res.insert(0, padsize, '0');
    }
    else
        res = ctos(0) + res;

    int i = 0;
    while (i < res.length())
    {
        output << stoc(res.substr(i, 8));
        i += 8;
    }

    input.close();
    output.close();
    return 0;
}