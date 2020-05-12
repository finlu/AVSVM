#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

void ConvertSrc2ByteCode(const char *pstrSrcFileName, const char *pstrDstFileName) {
    cout << "Start to read: " << pstrSrcFileName << endl;
    string line;
    ifstream infile(pstrSrcFileName);
    ofstream outfile(pstrDstFileName, ios::binary);
    if (!infile) {
        cout << "open error!" << endl;
        exit(-1);
    }
    int index = 0;
    while (getline(infile, line)) {
        string command;
        string args;
        for (char curChar : line) {
            if (curChar >= 48 && curChar <= 57) {
                args.push_back(curChar);
            } else if (curChar == 32) {
                continue;
            } else if (curChar >= 65 && curChar <= 90) {
                command.push_back(curChar);
            }
        }
        int argsLength = args.length();
        if (command == "POP" || command == "DUP" ||command == "SWAP" ||command == "ADD" ||
            command == "SUB" ||command == "HALT" || command == "NOP") {
            if (argsLength != 0) {
                cout << "指令输入错误" << endl;
                exit(-1);
            }
            outfile << command;
            index += 1;
            if (index % 5 == 0) {
                index = 0;
                outfile << endl;
            } else {
                outfile << " ";
            }
        } else if (command == "PUSH" || command == "IFEQ" ||command == "IFNE") {
            if (argsLength == 0) {
                cout << "指令输入错误" << endl;
                exit(-1);
            }
            outfile << command;
            index += 1;
            if (index % 5 == 0) {
                index = 0;
                outfile << endl;
            } else {
                outfile << " ";
            }
            while  (argsLength < 4) {
                outfile << "0";
                index += 1;
                if (index % 5 == 0) {
                    index = 0;
                    outfile << endl;
                } else {
                    outfile << " ";
                }
                ++argsLength;
            }
            outfile << args;
            index += 1;
            if (index % 5 == 0) {
                index = 0;
                outfile << endl;
            } else {
                outfile << " ";
            }
        }
    }
    infile.close();
    outfile.close();
    cout << "成功生成字节码，字节码内容保存在文件 " << pstrDstFileName << " 中" << endl;
}

int stack[100];
int SP = -1;
int PC = 0;
vector<string> pBuffer;

void SplitString(const string& s, vector<string>& v, const string& c)
{
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}

void loadByteCodes(const char* pstrByteCodesFileName) {
    ifstream infile(pstrByteCodesFileName);
    string line;
    while (getline(infile, line)) {
        vector<string> res;
        SplitString(line, res, " ");
        for (string& r: res) {
            pBuffer.push_back(r);
        }
    }
    infile.close();
}

int getnum() {
    ++PC;
    string res;
    for (int i = 0; i < 4; ++i) {
        res.append(pBuffer[PC]);
        ++PC;
    }
    return stoi(res);
}

void Engine() {
    int num = 0;
    string op;
    while (true) {
        op = pBuffer[PC];
        if (op == "HALT") {
            cout << "程序停机！" << endl;
            break;
        } else if (op == "PUSH") {  // 栈指令
            num = getnum();
            ++SP;
            stack[SP] = num;
        } else if (op == "POP") {
           --SP;
            ++PC;
        } else if (op == "DUP") {
            ++SP;
            stack[SP] = stack[SP-1];
            ++PC;
        } else if (op == "SWAP") {
            stack[SP] = stack[SP] + stack[SP - 1];
            stack[SP - 1] = stack[SP] - stack[SP - 1];
            stack[SP] = stack[SP] - stack[SP - 1];
            ++PC;
        } else if (op == "ADD") {  // 整数运算指令
            int temp = stack[SP - 1] + stack[SP];
            --SP;
            stack[SP] = temp;
            ++PC;
        } else if (op == "SUB") {
            int temp = stack[SP - 1] - stack[SP];
            --SP;
            stack[SP] = temp;
            ++PC;
        } else if (op == "IFEQ") {
            int temp = stack[SP];
            if (temp == 0) {
                PC = PC + getnum();
            }
            --SP;
        } else if (op == "IFNE") {
            --SP;
            int temp = stack[SP];
            if (temp != 0) {
                PC = PC + getnum();
            }
        } else if (op == "NOP") {
            ++PC;
            continue;
        } else {
            cout << "op: " + op + " 操作错误！！" << endl;
            exit(-1);
        }
    }
}

int main() {
    const char *pstrSrcFileName = R"(E:\WorkSpace\CLionProjects\jvm\test.txt)";
    const char *pstrDstFileName = R"(E:\WorkSpace\CLionProjects\jvm\test.code)";
    ConvertSrc2ByteCode(pstrSrcFileName, pstrDstFileName);
    loadByteCodes(pstrDstFileName);
    Engine();
    cout << "运算结果为" << stack[0] << endl;
    return 0;
}
