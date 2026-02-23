#include <Windows.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<vector<long long>> readMatrix(const string& path, int n) {
    ifstream in(path);
    vector<vector<long long>> m(n, vector<long long>(n, 0));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            in >> m[i][j];
        }
    }

    return m;
}

vector<vector<long long>> multiply(const vector<vector<long long>>& A,
                                   const vector<vector<long long>>& B,
                                   int n) {
    vector<vector<long long>> C(n, vector<long long>(n, 0));

    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            for (int j = 0; j < n; j++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return C;
}

void saveMatrix(const string& path, const vector<vector<long long>>& m, int n) {
    ofstream out(path);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            out << m[i][j];
            if (j + 1 < n) out << ' ';
        }
        out << '\n';
    }
}

bool fileExists(const string& path) {
    ifstream f(path);
    return f.is_open();
}

bool parseNFromAFile(const string& name, int& n) {
    // Expected: A_<n>.txt
    if (name.size() < 7) return false;
    if (name[0] != 'A' || name[1] != '_') return false;
    if (name.substr(name.size() - 4) != ".txt") return false;

    string num = name.substr(2, name.size() - 6);
    if (num.empty()) return false;

    for (char c : num) {
        if (c < '0' || c > '9') return false;
    }

    n = stoi(num);
    return true;
}

vector<int> collectSizes(const string& base) {
    vector<int> sizes;

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA((base + "A_*.txt").c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return sizes;
    }

    do {
        int n = 0;
        if (parseNFromAFile(findData.cFileName, n)) {
            sizes.push_back(n);
        }
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);

    sort(sizes.begin(), sizes.end());
    sizes.erase(unique(sizes.begin(), sizes.end()), sizes.end());
    return sizes;
}

string findDataBase() {
    const string bases[] = {"data/", "../data/", "../../data/", "../../../data/"};

    for (const string& b : bases) {
        vector<int> sizes = collectSizes(b);
        if (!sizes.empty()) return b;
    }

    return "";
}

int main() {
    string base = findDataBase();
    if (base.empty()) {
        cout << "Cannot find data folder with A_*.txt files\n";
        return 1;
    }

    vector<int> sizes = collectSizes(base);
    if (sizes.empty()) {
        cout << "No A_*.txt files found in: " << base << "\n";
        return 1;
    }

    cout << "Data folder: " << base << "\n";

    for (int n : sizes) {
        string aPath = base + "A_" + to_string(n) + ".txt";
        string bPath = base + "B_" + to_string(n) + ".txt";
        string cPath = base + "C_" + to_string(n) + ".txt";

        if (!fileExists(aPath) || !fileExists(bPath)) {
            cout << "Skip N=" << n << " (missing A or B file)\n";
            continue;
        }

        vector<vector<long long>> A = readMatrix(aPath, n);
        vector<vector<long long>> B = readMatrix(bPath, n);

        auto start = chrono::high_resolution_clock::now();
        vector<vector<long long>> C = multiply(A, B, n);
        auto end = chrono::high_resolution_clock::now();

        saveMatrix(cPath, C, n);

        auto ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        long long ops = 2LL * n * n * n;

        cout << "N=" << n
             << " | Time: " << ms << " ms"
             << " | Task volume: " << ops << " operations"
             << " | Saved: " << cPath << "\n";
    }

    return 0;
}
