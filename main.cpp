#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <regex>
#include <random>
#include <set>
#include <chrono>

using namespace std;

// 工具函数：统一小写 + 去掉非字母字符
std::string cleanText(const std::string& text);

// 分词函数
std::vector<std::string> tokenize(const std::string& line);


// 定义有向图类
class Graph {
private:
    unordered_map<string, unordered_map<string, int>> adjList;
    unordered_map<string, vector<string>> inEdges; // 存储每个节点的入边
    vector<string> walkPath;          // 存储遍历路径
    set<pair<string, string>> walkedEdges; // 存储已走过的边

public:
    // 添加边 word1 -> word2
    void addEdge(const string& word1, const string& word2) {
        adjList[word1][word2]++;
        // 确保word2作为节点存在
        if (adjList.find(word2) == adjList.end()) {
            adjList[word2] = unordered_map<string, int>();
        }
        // 维护逆向邻接表
        inEdges[word2].push_back(word1);
    }

    // 打印图结构（格式清晰）
    void showGraph() const {
        cout << "Graph Structure:\n";
        for (auto it = adjList.begin(); it != adjList.end(); ++it) {
            const auto& from = it->first;
            const auto& edges = it->second;
            cout << from << " -> ";
            for (auto edgeIt = edges.begin(); edgeIt != edges.end(); ++edgeIt) {
                const auto& to = edgeIt->first;
                const auto& weight = edgeIt->second;
                cout << "(" << to << ", " << weight << ") ";
            }
            cout << endl;
        }
    }


    // 读取图接口，供后续查询使用
    const unordered_map<string, unordered_map<string, int>>& getGraph() const {
        return adjList;
    }

    // 判断图中是否包含某个单词
    bool contains(const string& word) const {
        return adjList.find(word) != adjList.end();
    }

    // 查询 bridge words：word1 -> word3 -> word2
    string queryBridgeWords(const string& word1, const string& word2) const {
        // 1. 判断 word1 和 word2 是否在图中
        bool hasWord1 = (adjList.find(word1) != adjList.end());
        bool hasWord2 = false;

        for (auto it = adjList.begin(); it != adjList.end(); ++it) {
            const auto& key = it->first;
            const auto& targets = it->second;
            if (key == word2 || targets.find(word2) != targets.end()) {
                hasWord2 = true;
                break;
            }
        }


        if (!hasWord1 && !hasWord2)
            return "No \"" + word1 + "\" and \"" + word2 + "\" in the graph!";
        else if (!hasWord1)
            return "No \"" + word1 + "\" in the graph!";
        else if (!hasWord2)
            return "No \"" + word2 + "\" in the graph!";

        // 2. 搜索所有 word1 -> mid -> word2 的情况
        vector<string> bridges;

        const unordered_map<string, int>& neighbors = adjList.at(word1);
        for (const auto& pair : neighbors) {
            const string& mid = pair.first;
            // 如果 mid 有一条边指向 word2，则是合法桥接词
            if (adjList.find(mid) != adjList.end()) {
                if (adjList.at(mid).find(word2) != adjList.at(mid).end()) {
                    bridges.push_back(mid);
                }
            }
        }

        // 3. 输出结果
        if (bridges.empty()) {
            return "No bridge words from \"" + word1 + "\" to \"" + word2 + "\"!";
        } else {
            string result = "The bridge words from \"" + word1 + "\" to \"" + word2 + "\" is: ";
            for (size_t i = 0; i < bridges.size(); ++i) {
                result += "\"" + bridges[i] + "\"";
                if (i + 1 < bridges.size())
                    result += ", ";
            }
            result += ".";
            return result;
        }
    }

    string generateNewText(const string& inputText) const {
        string cleaned = cleanText(inputText); // 使用之前写的工具函数
        vector<string> words = tokenize(cleaned);

        if (words.size() < 2) return inputText;

        string result;
        for (size_t i = 0; i + 1 < words.size(); ++i) {
            string word1 = words[i];
            string word2 = words[i + 1];

            // 添加第一个词
            result += word1;

            // 查询桥接词列表
            vector<string> bridges;

            if (adjList.find(word1) != adjList.end()) {
                const unordered_map<string, int>& midCandidates = adjList.at(word1);
                for (const auto& pair : midCandidates) {
                    const string& mid = pair.first;
                    if (adjList.find(mid) != adjList.end() &&
                        adjList.at(mid).find(word2) != adjList.at(mid).end()) {
                        bridges.push_back(mid);
                    }
                }
            }

            // 如果有桥接词，随机选择一个插入
            if (!bridges.empty()) {
                int idx = rand() % bridges.size();
                result += " " + bridges[idx];
            }

            // 添加第二个词前的空格
            result += " ";
        }

        // 添加最后一个词
        result += words.back();

        return result;
    }

    string calcShortestPath(const string& start, const string& end) const {
        if (adjList.find(start) == adjList.end()) {
            return "No \"" + start + "\" in the graph!";
        }
        if (adjList.find(end) == adjList.end()) {
            return "No \"" + end + "\" in the graph!";
        }

        // 初始化最短距离表，前驱表
        unordered_map<string, int> dist;
        unordered_map<string, string> prev;
        set<string> visited;

        // 初始化：所有节点最大距离
        for (const auto& pair : adjList) {
            dist[pair.first] = INT_MAX;
        }
        dist[start] = 0;

        while (true) {
            // 找到当前未访问、距离最小的节点
            string u;
            int minDist = INT_MAX;
            for (const auto& pair : dist) {
                if (!visited.count(pair.first) && pair.second < minDist) {
                    u = pair.first;
                    minDist = pair.second;
                }
            }

            // 没找到更优节点，说明结束
            if (minDist == INT_MAX) break;

            visited.insert(u);

            // 遍历所有邻居，更新距离
            if (adjList.find(u) != adjList.end()) {
                const unordered_map<string, int>& neighbors = adjList.at(u);
                for (const auto& neighbor : neighbors) {
                    string v = neighbor.first;
                    int weight = neighbor.second;

                    if (dist[u] + weight < dist[v]) {
                        dist[v] = dist[u] + weight;
                        prev[v] = u;
                    }
                }
            }

        }

        // 无法到达目标节点
        if (dist[end] == INT_MAX) {
            return "No path from \"" + start + "\" to \"" + end + "\"!";
        }

        // 回溯路径
        vector<string> path;
        string current = end;
        while (current != start) {
            path.push_back(current);
            current = prev[current];
        }
        path.push_back(start);
        reverse(path.begin(), path.end());

        // 构造输出
        stringstream ss;
        ss << "Shortest path from \"" << start << "\" to \"" << end << "\":\n";
        for (size_t i = 0; i < path.size(); ++i) {
            ss << path[i];
            if (i + 1 < path.size()) ss << " -> ";
        }
        ss << "\nTotal weight: " << dist[end];

        return ss.str();
    }

    double calPageRank_Deepseek(const string& word) const {
        if (adjList.find(word) == adjList.end()) {
            return -1.0; // 单词不存在
        }

        const double d = 0.85;
        const int maxIterations = 1000;
        const double epsilon = 1e-8;

        vector<string> nodes;
        for (const auto& pair : adjList) {
            nodes.push_back(pair.first);
        }
        int N = nodes.size();
        if (N == 0) return 0.0;

        unordered_map<string, double> prCurrent, prNext;
        for (const auto& node : nodes) {
            prCurrent[node] = 1.0 / N;
        }

        for (int iter = 0; iter < maxIterations; ++iter) {
            // 计算所有悬挂节点的PR总和
            double danglingSum = 0.0;
            for (const auto& node : nodes) {
                if (adjList.at(node).empty()) {
                    danglingSum += prCurrent[node];
                }
            }
            danglingSum /= N; // 均分给所有节点

            for (const auto& u : nodes) {
                double sum = 0.0;
                // 处理入边节点的贡献
                if (inEdges.find(u) != inEdges.end()) {
                    for (const string& v : inEdges.at(u)) {
                        int Lv = adjList.at(v).size();
                        if (Lv > 0) {
                            sum += prCurrent[v] / Lv;
                        }
                    }
                }
                // 添加悬挂节点的贡献
                sum += danglingSum;
                prNext[u] = (1.0 - d)/N + d * sum;
            }

            // 检查收敛
            double maxDiff = 0.0;
            for (const auto& node : nodes) {
                maxDiff = max(maxDiff, abs(prNext[node] - prCurrent[node]));
            }
            if (maxDiff < epsilon) break;

            prCurrent = prNext;
        }

        return prCurrent.at(word);
    }

    double calPageRank_Chatgpt(const string& word) const {
        const double d = 0.85;           // 阻尼系数
        const int max_iter = 100;        // 最大迭代次数
        const double epsilon = 1e-6;     // 收敛阈值

        // 所有节点初始化 PR 值为 1.0
        unordered_map<string, double> pr;
        for (const auto& pair : adjList) {
            pr[pair.first] = 1.0;
            // 还要注意：被指向但没出边的节点也要加入
            for (const auto& sub : pair.second) {
                if (pr.find(sub.first) == pr.end()) {
                    pr[sub.first] = 1.0;
                }
            }
        }

        // 预构建反向图：入边信息
        unordered_map<string, vector<string>> incoming;
        for (const auto& from_pair : adjList) {
            const string& u = from_pair.first;
            for (const auto& to_pair : from_pair.second) {
                const string& v = to_pair.first;
                incoming[v].push_back(u);
            }
        }

        // 迭代
        for (int iter = 0; iter < max_iter; ++iter) {
            unordered_map<string, double> new_pr;
            double diff = 0.0;

            for (const auto& node_pair : pr) {
                const string& node = node_pair.first;
                double sum = 0.0;

                if (incoming.find(node) != incoming.end()) {
                    for (const string& inNode : incoming[node]) {
                        int out_deg = adjList.find(inNode) != adjList.end() ? adjList.at(inNode).size() : 0;
                        if (out_deg > 0)
                            sum += pr[inNode] / out_deg;
                    }
                }

                new_pr[node] = (1.0 - d) + d * sum;
                diff += abs(new_pr[node] - pr[node]);
            }

            pr = new_pr;
            if (diff < epsilon) break; // 提前收敛
        }

        // 返回目标单词的 PR 值（如果存在）
        if (pr.find(word) != pr.end()) {
            return pr[word];
        } else {
            cout << "No \"" << word << "\" in the graph!\n";
            return 0.0;
        }
    }


    string randomWalk() {
        walkPath.clear();
        walkedEdges.clear();

        if (adjList.empty()) return "";

        // 使用高精度时间戳作为随机种子（确保每次调用不同）
        unsigned seed = chrono::system_clock::now().time_since_epoch().count();
        static std::mt19937 gen(seed); // 静态引擎，但每次调用重新设定种子

        // 随机选择起始节点
        vector<string> nodes;
        for (const auto& pair : adjList) nodes.push_back(pair.first);
        std::uniform_int_distribution<int> dis(0, nodes.size()-1);
        string current = nodes[dis(gen)];
        walkPath.push_back(current);

        while (true) {
            if (adjList.at(current).empty()) break;

            vector<string> candidates;
            for (const auto& edge : adjList.at(current)) {
                candidates.push_back(edge.first);
            }

            // 每次重新生成分布对象（避免残留状态）
            std::uniform_int_distribution<int> edge_dis(0, candidates.size()-1);
            string next = candidates[edge_dis(gen)];

            pair<string, string> edge(current, next);
            walkPath.push_back(next);
            if (walkedEdges.count(edge)) break;


            walkedEdges.insert(edge);
            current = next;
        }

        stringstream ss;
        for (size_t i = 0; i < walkPath.size(); ++i) {
            ss << walkPath[i];
            if (i != walkPath.size()-1) ss << " ";
        }
        return ss.str();
    }

    void saveWalkToFile(const string& filename) {
        ofstream outfile(filename);
        if (!outfile) {
            cerr << "无法写入文件: " << filename << endl;
            return;
        }
        for (const auto& word : walkPath) {
            outfile << word << " ";
        }
        outfile << endl;
    }
};

// 工具函数：统一小写 + 去掉非字母字符
string cleanText(const string& text) {
    string result;
    for (char c : text) {
        if (isalpha(c)) {
            result += tolower(c);
        } else if (isspace(c)) {
            result += ' ';
        } else {
            result += ' '; // 将标点转为空格
        }
    }
    return result;
}

// 分词函数
vector<string> tokenize(const string& line) {
    vector<string> tokens;
    stringstream ss(line);
    string word;
    while (ss >> word) {
        tokens.push_back(word);
    }
    return tokens;
}

// 从文件读取，生成图结构
void loadTextAndBuildGraph(const string& filename, Graph& g) {
    ifstream infile(filename);
    if (!infile) {
        cerr << "Cannot open file: " << filename << endl;
        return;
    }

    string line, fullText;
    while (getline(infile, line)) {
        fullText += line + ' ';
    }

    string cleaned = cleanText(fullText);
    vector<string> words = tokenize(cleaned);

    for (size_t i = 0; i + 1 < words.size(); ++i) {
        g.addEdge(words[i], words[i + 1]);
    }
}

int main() {
    string filepath = "d:/2025Junior/SoftwareEngineering/lab1/"; // 默认文件路径
    cout << "请输入文本文件名：";
    string filename;
    cin >> filename;
    string path = filepath + filename;
    Graph g;
    loadTextAndBuildGraph(path, g);

    cout << "\n=== 展示生成的有向图 ===\n";
    g.showGraph();

    string w1, w2;
    cout << "\n请输入两个英文单词以查询桥接词：";
    cin >> w1 >> w2;

    string bridgeInfo = g.queryBridgeWords(w1, w2);
    cout << bridgeInfo << endl;

    srand(time(0)); // 初始化随机种子，仅需调用一次

    cin.ignore(); // 清空前一次 cin 残留
    string userInput;
    cout << "\n请输入一行英文文本以自动生成新文本：\n";
    getline(cin, userInput);

    string newText = g.generateNewText(userInput);
    cout << "\n生成的新文本如下：\n" << newText << endl;

    string src, dst;
    cout << "\n请输入起点和终点单词以查询最短路径：";
    cin >> src >> dst;

    string pathInfo = g.calcShortestPath(src, dst);
    cout << pathInfo << endl;

    string queryWord;
    cout << "\n请输入要查询 PageRank 的单词：";
    cin >> queryWord;

    double pr = g.calPageRank_Deepseek(queryWord);
    double prVal = g.calPageRank_Chatgpt(queryWord);
    if (pr < 0) {
        cout << "单词 \"" << queryWord << "\" 不在图中！" << endl;
    } else {
        cout << "Deepseek生成代码计算的PageRank值为：" << pr << endl;
        cout << "Chatgpt生成代码计算的PageRank值为：" << prVal << endl;
    }

    // 随机游走功能
    string walkResult = g.randomWalk();
    cout << "\n随机游走路径：\n" << walkResult << endl;

    // 保存到文件（可选）
    g.saveWalkToFile("random_walk.txt");

    return 0;
}
