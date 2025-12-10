#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <random>
#include <locale.h>
#include <chrono>
#include <string>
#include <algorithm>
#include <climits>
#include <iomanip>
#include <cstdlib>

using namespace std;

void printHelp() {
    cout << "Использование программы:" << endl;
    cout << "  program.exe [-type weighted|unweighted] [-orientation directed|undirected] [-n N] [-density D] [-min MIN] [-max MAX] [-help]" << endl;
    cout << endl;
    cout << "Параметры командной строки:" << endl;
    cout << "  -type <тип>          : weighted - взвешенный граф (по умолчанию)" << endl;
    cout << "                        : unweighted - невзвешенный граф" << endl;
    cout << "  -orientation <ориентация> : directed - ориентированный граф" << endl;
    cout << "                        : undirected - неориентированный граф (по умолчанию)" << endl;
    cout << "  -n <число>           : количество вершин графа" << endl;
    cout << "  -density <процент>   : процент рёбер между вершинами (0-100)" << endl;
    cout << "  -min <вес>           : минимальный вес рёбер (только для взвешенного графа)" << endl;
    cout << "  -max <вес>           : максимальный вес рёбер (только для взвешенного графа)" << endl;
    cout << "  -help                : показать эту справку" << endl;
    cout << endl;
    cout << "Примеры:" << endl;
    cout << "  program.exe -type weighted -orientation directed -n 10 -density 50 -min 1 -max 10" << endl;
    cout << "  program.exe -orientation undirected -n 8 -density 30" << endl;
    cout << "  program.exe -type unweighted -n 5 -density 40" << endl;
    cout << endl;
    cout << "Если параметры не указаны, программа запросит их в интерактивном режиме." << endl;
}

int getPositiveInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail() || value <= 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Ошибка! Введите положительное число: ";
        }
        else {
            cin.ignore(10000, '\n');
            return value;
        }
    }
}

int getPercent(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail() || value < 0 || value > 100) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Ошибка! Введите число от 0 до 100: ";
        }
        else {
            cin.ignore(10000, '\n');
            return value;
        }
    }
}

void getWeightRange(int& minWeight, int& maxWeight) {
    while (true) {
        minWeight = getPositiveInt("Введите минимальный вес рёбер: ");
        maxWeight = getPositiveInt("Введите максимальный вес рёбер: ");

        if (minWeight > maxWeight) {
            cout << "Ошибка! Максимальный вес не может быть меньше минимального. Попробуйте снова." << endl;
        }
        else {
            break;
        }
    }
}

vector<vector<int>> generateWeightedAdjacencyMatrix(int n, double density, bool directed, int minWeight = 1, int maxWeight = 10) {
    vector<vector<int>> matrix(n, vector<int>(n, 0));
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> prob_dis(0.0, 1.0);
    uniform_int_distribution<> weight_dis(minWeight, maxWeight);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            if (prob_dis(gen) < density) {
                int weight = weight_dis(gen);
                matrix[i][j] = weight;
                if (!directed) {
                    matrix[j][i] = weight;
                }
            }
        }
    }
    return matrix;
}

vector<vector<int>> generateUnweightedAdjacencyMatrix(int n, double density, bool directed) {
    vector<vector<int>> matrix(n, vector<int>(n, 0));
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> prob_dis(0.0, 1.0);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            if (prob_dis(gen) < density) {
                matrix[i][j] = 1;
                if (!directed) {
                    matrix[j][i] = 1;
                }
            }
        }
    }
    return matrix;
}

vector<vector<int>> convertToAdjacencyList(const vector<vector<int>>& matrix) {
    int n = matrix.size();
    vector<vector<int>> list(n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] > 0) {
                list[i].push_back(j);
            }
        }
    }
    return list;
}

void printMatrix(const vector<vector<int>>& matrix, const string& title = "Матрица смежности:") {
    int n = matrix.size();

    int maxCellWidth = 1;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int numWidth = to_string(matrix[i][j]).length();
            if (numWidth > maxCellWidth) {
                maxCellWidth = numWidth;
            }
        }
    }

    int maxIndexWidth = to_string(n).length();

    cout << title << endl;
    cout << setw(maxIndexWidth + 2) << " ";
    for (int i = 1; i <= n; i++) {
        cout << setw(maxCellWidth + 1) << i;
    }
    cout << endl;

    for (int i = 0; i < n; i++) {
        cout << setw(maxIndexWidth) << (i + 1) << ": ";
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] == INT_MAX) {
                cout << setw(maxCellWidth + 1) << "∞";
            }
            else if (matrix[i][j] == -1) {
                cout << setw(maxCellWidth + 1) << "-";
            }
            else {
                cout << setw(maxCellWidth + 1) << matrix[i][j];
            }
        }
        cout << endl;
    }
}

void printAdjacencyList(const vector<vector<int>>& adjList, const vector<vector<int>>& weights, bool weighted) {
    cout << "Списки смежности:" << endl;
    for (int i = 0; i < adjList.size(); i++) {
        cout << (i + 1) << ": ";
        for (int j = 0; j < adjList[i].size(); j++) {
            int neighbor = adjList[i][j];
            if (weighted) {
                cout << (neighbor + 1) << "(" << weights[i][neighbor] << ") ";
            }
            else {
                cout << (neighbor + 1) << " ";
            }
        }
        cout << endl;
    }
}

void printDistances(const vector<int>& dist) {
    cout << "Вектор расстояний: ";
    for (int i = 0; i < dist.size(); i++) {
        cout << "[" << (i + 1) << "]=";
        if (dist[i] == INT_MAX) cout << "∞ ";
        else if (dist[i] == -1) cout << "- ";
        else cout << dist[i] << " ";
    }
    cout << endl;
}

void printDistancesMatrix(const vector<vector<int>>& allDist, const string& title = "Матрица расстояний:") {
    int n = allDist.size();

    int maxCellWidth = 1;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (allDist[i][j] == INT_MAX) {
                if (1 > maxCellWidth) maxCellWidth = 1;
            }
            else if (allDist[i][j] == -1) {
                if (1 > maxCellWidth) maxCellWidth = 1;
            }
            else {
                int numWidth = to_string(allDist[i][j]).length();
                if (numWidth > maxCellWidth) {
                    maxCellWidth = numWidth;
                }
            }
        }
    }

    int maxIndexWidth = to_string(n).length();

    cout << title << endl;
    cout << setw(maxIndexWidth + 2) << " ";
    for (int i = 1; i <= n; i++) {
        cout << setw(maxCellWidth + 1) << i;
    }
    cout << endl;

    for (int i = 0; i < n; i++) {
        cout << setw(maxIndexWidth) << (i + 1) << ": ";
        for (int j = 0; j < n; j++) {
            if (i == j) {
                cout << setw(maxCellWidth + 1) << "0";
            }
            else if (allDist[i][j] == INT_MAX) {
                cout << setw(maxCellWidth + 1) << "∞";
            }
            else if (allDist[i][j] == -1) {
                cout << setw(maxCellWidth + 1) << "-";
            }
            else {
                cout << setw(maxCellWidth + 1) << allDist[i][j];
            }
        }
        cout << endl;
    }
}

vector<int> BFSD_weighted_matrix(const vector<vector<int>>& G, int v) {
    int n = G.size();
    vector<int> DIST(n, INT_MAX);
    queue<int> Q;

    Q.push(v);
    DIST[v] = 0;

    while (!Q.empty()) {
        int current = Q.front();
        Q.pop();

        for (int i = 0; i < n; i++) {
            if (G[current][i] > 0) {
                int new_dist = DIST[current] + G[current][i];
                if (new_dist < DIST[i]) {
                    DIST[i] = new_dist;
                    Q.push(i);
                }
            }
        }
    }
    return DIST;
}

vector<int> BFSD_unweighted_matrix(const vector<vector<int>>& G, int v) {
    int n = G.size();
    vector<int> DIST(n, -1);
    queue<int> Q;

    Q.push(v);
    DIST[v] = 0;

    while (!Q.empty()) {
        int current = Q.front();
        Q.pop();

        for (int i = 0; i < n; i++) {
            if (G[current][i] > 0 && DIST[i] == -1) {
                DIST[i] = DIST[current] + 1;
                Q.push(i);
            }
        }
    }
    return DIST;
}

vector<int> BFSD_unweighted_list(const vector<vector<int>>& adjList, int v) {
    int n = adjList.size();
    vector<int> DIST(n, -1);
    queue<int> Q;

    Q.push(v);
    DIST[v] = 0;

    while (!Q.empty()) {
        int current = Q.front();
        Q.pop();

        for (int neighbor : adjList[current]) {
            if (DIST[neighbor] == -1) {
                Q.push(neighbor);
                DIST[neighbor] = DIST[current] + 1;
            }
        }
    }
    return DIST;
}

vector<int> DFSD_iterative_matrix(const vector<vector<int>>& G, int v) {
    int n = G.size();
    vector<int> DIST(n, -1);
    stack<int> S;

    S.push(v);
    DIST[v] = 0;

    while (!S.empty()) {
        int current = S.top();
        S.pop();

        for (int i = 0; i < n; i++) {
            if (G[current][i] > 0 && DIST[i] == -1) {
                DIST[i] = DIST[current] + 1;
                S.push(i);
            }
        }
    }
    return DIST;
}

vector<int> DFSD_iterative_list(const vector<vector<int>>& adjList, int v) {
    int n = adjList.size();
    vector<int> DIST(n, -1);
    stack<int> S;

    S.push(v);
    DIST[v] = 0;

    while (!S.empty()) {
        int current = S.top();
        S.pop();

        for (int neighbor : adjList[current]) {
            if (DIST[neighbor] == -1) {
                DIST[neighbor] = DIST[current] + 1;
                S.push(neighbor);
            }
        }
    }
    return DIST;
}

vector<vector<int>> findAllDistances(const vector<vector<int>>& G, bool directed, bool weighted) {
    int n = G.size();
    vector<vector<int>> allDist(n);

    for (int i = 0; i < n; i++) {
        if (weighted) {
            allDist[i] = BFSD_weighted_matrix(G, i);
        }
        else {
            allDist[i] = BFSD_unweighted_matrix(G, i);
        }
    }
    return allDist;
}

void analyzeGraph(const vector<vector<int>>& allDist, const string& graphType, bool weighted) {
    int n = allDist.size();
    vector<int> eccentricity(n, 0);

    cout << "\n=== АНАЛИЗ " << graphType << " ГРАФА ===" << endl;
    cout << "Тип графа: " << (weighted ? "взвешенный" : "невзвешенный") << endl;

    // Сначала выводим матрицу расстояний
    printDistancesMatrix(allDist, "Матрица расстояний (из каждой вершины во все остальные):");

    for (int i = 0; i < n; i++) {
        int max_dist = 0;
        bool allReachable = true;

        for (int j = 0; j < n; j++) {
            if (i != j) {
                if (allDist[i][j] == INT_MAX || allDist[i][j] == -1) {
                    allReachable = false;
                }
                else {
                    max_dist = max(max_dist, allDist[i][j]);
                }
            }
        }

        if (!allReachable) {
            eccentricity[i] = INT_MAX;
        }
        else {
            eccentricity[i] = max_dist;
        }
    }

    int diameter = 0;
    int radius = INT_MAX;

    for (int i = 0; i < n; i++) {
        if (eccentricity[i] != INT_MAX) {
            diameter = max(diameter, eccentricity[i]);
            radius = min(radius, eccentricity[i]);
        }
    }

    cout << "\nЭксцентриситеты вершин: " << endl;
    for (int i = 0; i < n; i++) {
        cout << "  Вершина " << (i + 1) << ": ";
        if (eccentricity[i] == INT_MAX) {
            cout << "∞ (недостижимы некоторые вершины)";
        }
        else {
            cout << eccentricity[i];
        }
        cout << endl;
    }

    cout << "\nДИАМЕТР графа: " << diameter << endl;
    cout << "РАДИУС графа: " << radius << endl;

    cout << "\nПЕРИФЕРИЙНЫЕ ВЕРШИНЫ (эксцентриситет = диаметру " << diameter << "): ";
    bool hasPeripheral = false;
    for (int i = 0; i < n; i++) {
        if (eccentricity[i] == diameter) {
            cout << (i + 1) << " ";
            hasPeripheral = true;
        }
    }
    if (!hasPeripheral) cout << "отсутствуют";
    cout << endl;

    cout << "ЦЕНТРАЛЬНЫЕ ВЕРШИНЫ (эксцентриситет = радиусу " << radius << "): ";
    bool hasCentral = false;
    for (int i = 0; i < n; i++) {
        if (eccentricity[i] == radius) {
            cout << (i + 1) << " ";
            hasCentral = true;
        }
    }
    if (!hasCentral) cout << "отсутствуют";
    cout << endl;

    cout << "ЦЕНТР графа: { ";
    for (int i = 0; i < n; i++) {
        if (eccentricity[i] == radius) {
            cout << (i + 1) << " ";
        }
    }
    cout << "}" << endl;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    // Параметры по умолчанию
    bool weighted = true;
    bool directed = false;
    int n = -1;          // -1 означает "не задано"
    int densityPercent = -1;
    int minWeight = 1;
    int maxWeight = 10;

    // Обработка аргументов командной строки
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            string arg = argv[i];

            if (arg == "-type") {
                if (i + 1 < argc) {
                    string type = argv[i + 1];
                    if (type == "weighted") weighted = true;
                    else if (type == "unweighted") weighted = false;
                    else {
                        cout << "Ошибка: неизвестный тип графа '" << type << "'" << endl;
                        printHelp();
                        return 1;
                    }
                    i++;
                }
            }
            else if (arg == "-orientation") {
                if (i + 1 < argc) {
                    string orient = argv[i + 1];
                    if (orient == "directed") directed = true;
                    else if (orient == "undirected") directed = false;
                    else {
                        cout << "Ошибка: неизвестная ориентация '" << orient << "'" << endl;
                        printHelp();
                        return 1;
                    }
                    i++;
                }
            }
            else if (arg == "-n") {
                if (i + 1 < argc) {
                    n = atoi(argv[i + 1]);
                    if (n <= 0) {
                        cout << "Ошибка: количество вершин должно быть положительным числом" << endl;
                        printHelp();
                        return 1;
                    }
                    i++;
                }
            }
            else if (arg == "-density") {
                if (i + 1 < argc) {
                    densityPercent = atoi(argv[i + 1]);
                    if (densityPercent < 0 || densityPercent > 100) {
                        cout << "Ошибка: плотность должна быть в диапазоне 0-100" << endl;
                        printHelp();
                        return 1;
                    }
                    i++;
                }
            }
            else if (arg == "-min") {
                if (i + 1 < argc) {
                    minWeight = atoi(argv[i + 1]);
                    if (minWeight <= 0) {
                        cout << "Ошибка: минимальный вес должен быть положительным числом" << endl;
                        printHelp();
                        return 1;
                    }
                    i++;
                }
            }
            else if (arg == "-max") {
                if (i + 1 < argc) {
                    maxWeight = atoi(argv[i + 1]);
                    if (maxWeight <= 0) {
                        cout << "Ошибка: максимальный вес должен быть положительным числом" << endl;
                        printHelp();
                        return 1;
                    }
                    i++;
                }
            }
            else if (arg == "-help") {
                printHelp();
                return 0;
            }
            else {
                cout << "Ошибка: неизвестный параметр '" << arg << "'" << endl;
                printHelp();
                return 1;
            }
        }
    }

    cout << "=== ЛАБОРАТОРНАЯ РАБОТА №10 ===" << endl;
    cout << "=== ПОИСК РАССТОЯНИЙ ВО ВЗВЕШЕННОМ ГРАФЕ ===" << endl;
    cout << endl;
    cout << "Параметры графа:" << endl;
    cout << "  Тип графа: " << (weighted ? "взвешенный" : "невзвешенный") << endl;
    cout << "  Ориентация: " << (directed ? "ориентированный" : "неориентированный") << endl;

    // Если параметры не заданы через командную строку, запрашиваем их
    if (n == -1) {
        n = getPositiveInt("Введите количество вершин графа: ");
    }
    else {
        cout << "  Количество вершин: " << n << " (задано в командной строке)" << endl;
    }

    if (densityPercent == -1) {
        densityPercent = getPercent("Введите процент рёбер между вершинами (0-100%): ");
    }
    else {
        cout << "  Плотность рёбер: " << densityPercent << "% (задано в командной строке)" << endl;
    }

    if (weighted && argc <= 1) { // Если нет параметров командной строки, запрашиваем веса
        getWeightRange(minWeight, maxWeight);
    }
    else if (weighted) {
        cout << "  Минимальный вес: " << minWeight << " (задано в командной строке)" << endl;
        cout << "  Максимальный вес: " << maxWeight << " (задано в командной строке)" << endl;
    }

    // Проверка корректности диапазона весов
    if (weighted && minWeight > maxWeight) {
        cout << "Ошибка: минимальный вес (" << minWeight << ") больше максимального (" << maxWeight << ")" << endl;
        if (argc > 1) {
            cout << "Исправьте параметры командной строки" << endl;
            printHelp();
        }
        return 1;
    }

    cout << endl;

    double density = densityPercent / 100.0;

    cout << "\n=== ЗАДАНИЕ 1 ===" << endl;

    vector<vector<int>> matrix;
    if (weighted) {
        matrix = generateWeightedAdjacencyMatrix(n, density, directed, minWeight, maxWeight);
    }
    else {
        matrix = generateUnweightedAdjacencyMatrix(n, density, directed);
    }

    printMatrix(matrix);

    auto adjList = convertToAdjacencyList(matrix);
    printAdjacencyList(adjList, matrix, weighted);

    cout << "\nПоиск расстояний BFS из вершины 1:" << endl;
    vector<int> distances;
    if (weighted) {
        distances = BFSD_weighted_matrix(matrix, 0);
    }
    else {
        distances = BFSD_unweighted_matrix(matrix, 0);
    }
    printDistances(distances);

    // Также покажем расстояния через списки смежности для невзвешенного графа
    if (!weighted) {
        cout << "\nПоиск расстояний BFS через списки смежности из вершины 1:" << endl;
        auto dist_list = BFSD_unweighted_list(adjList, 0);
        printDistances(dist_list);

        cout << "\nПоиск расстояний DFS (матрица) из вершины 1:" << endl;
        auto dist_dfs_matrix = DFSD_iterative_matrix(matrix, 0);
        printDistances(dist_dfs_matrix);

        cout << "\nПоиск расстояний DFS (списки) из вершины 1:" << endl;
        auto dist_dfs_list = DFSD_iterative_list(adjList, 0);
        printDistances(dist_dfs_list);
    }

    cout << "\n=== ЗАДАНИЕ 2 ===" << endl;

    auto allDistances = findAllDistances(matrix, directed, weighted);
    string graphType = directed ? "ОРИЕНТИРОВАННОГО" : "НЕОРИЕНТИРОВАННОГО";
    analyzeGraph(allDistances, graphType, weighted);

    cout << "\n=== ИНФОРМАЦИЯ О ПАРАМЕТРАХ ===" << endl;
    cout << "Для полного задания параметров через командную строку используйте:" << endl;
    cout << "  " << argv[0] << " -type weighted -orientation directed -n 10 -density 50 -min 1 -max 10" << endl;
    cout << "  " << argv[0] << " -type unweighted -orientation undirected -n 8 -density 30" << endl;
    cout << "  " << argv[0] << " -help" << endl;

    return 0;
}