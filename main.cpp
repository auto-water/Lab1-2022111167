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

// ���ߺ�����ͳһСд + ȥ������ĸ�ַ�
std::string cleanText(const std::string& text);

// �ִʺ���
std::vector<std::string> tokenize(const std::string& line);


// ��������ͼ��
class Graph {
private:
    unordered_map<string, unordered_map<string, int>> adjList;
    unordered_map<string, vector<string>> inEdges; // �洢ÿ���ڵ�����
    vector<string> walkPath;          // �洢����·��
    set<pair<string, string>> walkedEdges; // �洢���߹��ı�

public:
    // ���ӱ� word1 -> word2
    void addEdge(const string& word1, const string& word2) {
        adjList[word1][word2]++;
        // ȷ��word2��Ϊ�ڵ����
        if (adjList.find(word2) == adjList.end()) {
            adjList[word2] = unordered_map<string, int>();
        }
        // ά�������ڽӱ�
        inEdges[word2].push_back(word1);
    }

    // ��ӡͼ�ṹ����ʽ������
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


    // ��ȡͼ�ӿڣ���������ѯʹ��
    const unordered_map<string, unordered_map<string, int>>& getGraph() const {
        return adjList;
    }

    // �ж�ͼ���Ƿ����ĳ������
    bool contains(const string& word) const {
        return adjList.find(word) != adjList.end();
    }

    // ��ѯ bridge words��word1 -> word3 -> word2
    string queryBridgeWords(const string& word1, const string& word2) const {
        // 1. �ж� word1 �� word2 �Ƿ���ͼ��
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

        // 2. �������� word1 -> mid -> word2 �����
        vector<string> bridges;

        const unordered_map<string, int>& neighbors = adjList.at(word1);
        for (const auto& pair : neighbors) {
            const string& mid = pair.first;
            // ��� mid ��һ����ָ�� word2�����ǺϷ��ŽӴ�
            if (adjList.find(mid) != adjList.end()) {
                if (adjList.at(mid).find(word2) != adjList.at(mid).end()) {
                    bridges.push_back(mid);
                }
            }
        }

        // 3. ������
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
        string cleaned = cleanText(inputText); // ʹ��֮ǰд�Ĺ��ߺ���
        vector<string> words = tokenize(cleaned);

        if (words.size() < 2) return inputText;

        string result;
        for (size_t i = 0; i + 1 < words.size(); ++i) {
            string word1 = words[i];
            string word2 = words[i + 1];

            // ���ӵ�һ����
            result += word1;

            // ��ѯ�ŽӴ��б�
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

            // ������ŽӴʣ����ѡ��һ������
            if (!bridges.empty()) {
                int idx = rand() % bridges.size();
                result += " " + bridges[idx];
            }

            // ���ӵڶ�����ǰ�Ŀո�
            result += " ";
        }

        // �������һ����
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

        // ��ʼ����̾������ǰ����
        unordered_map<string, int> dist;
        unordered_map<string, string> prev;
        set<string> visited;

        // ��ʼ�������нڵ�������
        for (const auto& pair : adjList) {
            dist[pair.first] = INT_MAX;
        }
        dist[start] = 0;

        while (true) {
            // �ҵ���ǰδ���ʡ�������С�Ľڵ�
            string u;
            int minDist = INT_MAX;
            for (const auto& pair : dist) {
                if (!visited.count(pair.first) && pair.second < minDist) {
                    u = pair.first;
                    minDist = pair.second;
                }
            }

            // û�ҵ����Žڵ㣬˵������
            if (minDist == INT_MAX) break;

            visited.insert(u);

            // ���������ھӣ����¾���
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

        // �޷�����Ŀ��ڵ�
        if (dist[end] == INT_MAX) {
            return "No path from \"" + start + "\" to \"" + end + "\"!";
        }

        // ����·��
        vector<string> path;
        string current = end;
        while (current != start) {
            path.push_back(current);
            current = prev[current];
        }
        path.push_back(start);
        reverse(path.begin(), path.end());

        // �������
        stringstream ss;
        ss << "Shortest path from \"" << start << "\" to \"" << end << "\":\n";
        for (size_t i = 0; i < path.size(); ++i) {
            ss << path[i];
            if (i + 1 < path.size()) ss << " -> ";
        }
        ss << "\nTotal weight: " << dist[end];

        return ss.str();
    }


    double calPageRank_Chatgpt(const string& word) const {
        const double d = 0.85;           // ����ϵ��
        const int max_iter = 100;        // ����������
        const double epsilon = 1e-6;     // ������ֵ

        // ���нڵ��ʼ�� PR ֵΪ 1.0
        unordered_map<string, double> pr;
        for (const auto& pair : adjList) {
            pr[pair.first] = 1.0;
            // ��Ҫע�⣺��ָ��û���ߵĽڵ�ҲҪ����
            for (const auto& sub : pair.second) {
                if (pr.find(sub.first) == pr.end()) {
                    pr[sub.first] = 1.0;
                }
            }
        }

        // Ԥ��������ͼ�������Ϣ
        unordered_map<string, vector<string>> incoming;
        for (const auto& from_pair : adjList) {
            const string& u = from_pair.first;
            for (const auto& to_pair : from_pair.second) {
                const string& v = to_pair.first;
                incoming[v].push_back(u);
            }
        }

        // ����
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
            if (diff < epsilon) break; // ��ǰ����
        }

        // ����Ŀ�굥�ʵ� PR ֵ��������ڣ�
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

        // ʹ�ø߾���ʱ�����Ϊ������ӣ�ȷ��ÿ�ε��ò�ͬ��
        unsigned seed = chrono::system_clock::now().time_since_epoch().count();
        static std::mt19937 gen(seed); // ��̬���棬��ÿ�ε��������趨����

        // ���ѡ����ʼ�ڵ�
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

            // ÿ���������ɷֲ����󣨱������״̬��
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
            cerr << "�޷�д���ļ�: " << filename << endl;
            return;
        }
        for (const auto& word : walkPath) {
            outfile << word << " ";
        }
        outfile << endl;
    }
};

// ���ߺ�����ͳһСд + ȥ������ĸ�ַ�
string cleanText(const string& text) {
    string result;
    for (char c : text) {
        if (isalpha(c)) {
            result += tolower(c);
        } else if (isspace(c)) {
            result += ' ';
        } else {
            result += ' '; // �����תΪ�ո�
        }
    }
    return result;
}

// �ִʺ���
vector<string> tokenize(const string& line) {
    vector<string> tokens;
    stringstream ss(line);
    string word;
    while (ss >> word) {
        tokens.push_back(word);
    }
    return tokens;
}

// ���ļ���ȡ������ͼ�ṹ
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
    string filepath = "d:/2025Junior/SoftwareEngineering/lab1/"; // Ĭ���ļ�·��
    cout << "�������ı��ļ�����";
    string filename;
    cin >> filename;
    string path = filepath + filename;
    Graph g;
    loadTextAndBuildGraph(path, g);

    cout << "\n=== չʾ���ɵ�����ͼ ===\n";
    g.showGraph();

    string w1, w2;
    cout << "\n����������Ӣ�ĵ����Բ�ѯ�ŽӴʣ�";
    cin >> w1 >> w2;

    string bridgeInfo = g.queryBridgeWords(w1, w2);
    cout << bridgeInfo << endl;

    srand(time(0)); // ��ʼ��������ӣ��������һ��

    cin.ignore(); // ���ǰһ�� cin ����
    string userInput;
    cout << "\n������һ��Ӣ���ı����Զ��������ı���\n";
    getline(cin, userInput);

    string newText = g.generateNewText(userInput);
    cout << "\n���ɵ����ı����£�\n" << newText << endl;

    string src, dst;
    cout << "\n�����������յ㵥���Բ�ѯ���·����";
    cin >> src >> dst;

    string pathInfo = g.calcShortestPath(src, dst);
    cout << pathInfo << endl;

    string queryWord;
    cout << "\n������Ҫ��ѯ PageRank �ĵ��ʣ�";
    cin >> queryWord;

    double prVal = g.calPageRank_Chatgpt(queryWord);
    if (prVal < 0) {
        cout << "���� \"" << queryWord << "\" ����ͼ�У�" << endl;
    } else {
        cout << "Chatgpt���ɴ�������PageRankֵΪ��" << prVal << endl;
    }

    // ������߹���
    string walkResult = g.randomWalk();
    cout << "\n�������·����\n" << walkResult << endl;

    // ���浽�ļ�����ѡ��
    g.saveWalkToFile("random_walk.txt");

    return 114514;
}
