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

int error = 0;
char CRC(string t, string gen, int size)
{
    bool chk = false;
    int len = gen.length();

    if (size == 4)
    {
        string t1 = t.substr(0, len + 3);
        string t2 = t.substr(len + 3, len + 3);
        string r1 = binarydiv(t1, gen);
        string r2 = binarydiv(t2, gen);

        for (int i = 0; i < r1.length(); ++i)
        {
            char c = r1[i];
            if (c == '1')
            {
                chk = true;
                break;
            }
        }

        if (chk)
            error++;

        chk = false;

        for (int i = 0; i < r2.length(); ++i)
        {
            char c = r2[i];
            if (c == '1')
            {
                chk = true;
                break;
            }
        }

        if (chk)
            error++;

        return stoc(t1.substr(0, 4) + t2.substr(0, 4));
    }
    else
    {
        string remainder = binarydiv(t, gen);
        for (int i = 0; i < remainder.length(); ++i)
        {
            char c = remainder[i];
            if (c == '1')
            {
                chk = true;
                break;
            }
        }

        if (chk)
            error++;

        return stoc(t.substr(0, 8));
    }
}

int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        printf("usage: ./crc_decoder input_file output_file result_file generator dataword_size\n");
        return 1;
    }

    string ifn = argv[1];
    string ofn = argv[2];
    string rfn = argv[3];
    string gen = argv[4];
    string dataword_size = argv[5];

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

    ofstream result(rfn.data());

    if (!result.is_open())
    {
        printf("result file open error.\n");
        result.close();
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
    input.get(c);
    string padding = ctos(c);

    int codlen;
    int step;
    if (dsize == 4)
        codlen = 2 * gen.length() + 6;
    else
        codlen = gen.length() + 7;
    if (dsize == 4)
        step = 2;
    else
        step = 1;

    string tmp = "";
    while (input.get(c))
        tmp += ctos(c);
    int padsize = tmp.length() - (tmp.length() / codlen * codlen);
    tmp = tmp.substr(padsize, tmp.length() - padsize);

    int cnt = 0;
    int i = 0;
    while (i < tmp.length())
    {
        output << CRC(tmp.substr(i, codlen), gen, dsize);
        cnt += step;
        i += codlen;
    }

    result << cnt << " " << error << "\n";

    input.close();
    output.close();
    result.close();
    return 0;
}