// Graph.java
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.*;
import java.util.stream.Collectors;

public class Graph {
    private Map<String, Map<String, Integer>> adjList = new HashMap<>();
    private List<String> walkPath = new ArrayList<>();
    private Set<String> walkedEdges = new HashSet<>();

    /** 添加边 word1 -> word2 */
    public void addEdge(String word1, String word2) {
        adjList.computeIfAbsent(word1, k -> new HashMap<>())
                .merge(word2, 1, Integer::sum);
        adjList.putIfAbsent(word2, new HashMap<>());
    }

    /** 打印图结构 */
    public void showGraph() {
        System.out.println("Graph Structure:");
        for (var entry : adjList.entrySet()) {
            String from = entry.getKey();
            String edges = entry.getValue().entrySet().stream()
                    .map(e -> "(" + e.getKey() + ", " + e.getValue() + ")")
                    .collect(Collectors.joining(" "));
            System.out.println(from + " -> " + edges);
        }
    }

    /** 是否包含节点 */
    public boolean contains(String word) {
        return adjList.containsKey(word);
    }

    /** 查询桥接词 */
    public String queryBridgeWords(String w1, String w2) {
        boolean has1 = adjList.containsKey(w1);
        boolean has2 = adjList.containsKey(w2) ||
                adjList.values().stream().anyMatch(m -> m.containsKey(w2));

        if (!has1 && !has2) return String.format("No \"%s\" and \"%s\" in the graph!", w1, w2);
        if (!has1)      return String.format("No \"%s\" in the graph!", w1);
        if (!has2)      return String.format("No \"%s\" in the graph!", w2);

        List<String> bridges = new ArrayList<>();
        for (var midEntry : adjList.get(w1).entrySet()) {
            String mid = midEntry.getKey();
            if (adjList.get(mid).containsKey(w2)) {
                bridges.add(mid);
            }
        }
        if (bridges.isEmpty()) {
            return String.format("No bridge words from \"%s\" to \"%s\"!", w1, w2);
        }
        String joined = bridges.stream()
                .map(s -> "\"" + s + "\"")
                .collect(Collectors.joining(", "));
        return String.format("The bridge words from \"%s\" to \"%s\" is: %s.", w1, w2, joined);
    }

    /** 基于桥接词生成新文本 */
    public String generateNewText(String inputText) {
        String cleaned = TextProcessor.cleanText(inputText);
        List<String> words = TextProcessor.tokenize(cleaned);
        if (words.size() < 2) return inputText;

        StringBuilder sb = new StringBuilder();
        Random rnd = new Random();

        for (int i = 0; i < words.size() - 1; i++) {
            String w1 = words.get(i), w2 = words.get(i + 1);
            sb.append(w1);

            List<String> bridges = new ArrayList<>();
            if (adjList.containsKey(w1)) {
                for (var mid : adjList.get(w1).keySet()) {
                    if (adjList.get(mid).containsKey(w2)) {
                        bridges.add(mid);
                    }
                }
            }
            if (!bridges.isEmpty()) {
                sb.append(" ").append(bridges.get(rnd.nextInt(bridges.size())));
            }
            sb.append(" ");
        }
        sb.append(words.get(words.size() - 1));
        return sb.toString();
    }

    /** 计算最短路径（Dijkstra） */
    public String calcShortestPath(String start, String end) {
        if (!adjList.containsKey(start)) return String.format("No \"%s\" in the graph!", start);
        if (!adjList.containsKey(end))   return String.format("No \"%s\" in the graph!", end);

        Map<String, Integer> dist = new HashMap<>();
        Map<String, String> prev = new HashMap<>();
        Set<String> visited = new HashSet<>();

        for (var node : adjList.keySet()) dist.put(node, Integer.MAX_VALUE);
        dist.put(start, 0);

        while (true) {
            String u = null;
            int minD = Integer.MAX_VALUE;
            for (var e : dist.entrySet()) {
                if (!visited.contains(e.getKey()) && e.getValue() < minD) {
                    u = e.getKey(); minD = e.getValue();
                }
            }
            if (u == null) break;
            visited.add(u);
            for (var nbr : adjList.get(u).entrySet()) {
                String v = nbr.getKey();
                int w = nbr.getValue();
                if (dist.get(u) + w < dist.get(v)) {
                    dist.put(v, dist.get(u) + w);
                    prev.put(v, u);
                }
            }
        }

        if (dist.get(end) == Integer.MAX_VALUE) {
            return String.format("No path from \"%s\" to \"%s\"!", start, end);
        }
        List<String> path = new ArrayList<>();
        for (String cur = end; cur != null; cur = prev.get(cur)) {
            path.add(cur);
            if (cur.equals(start)) break;
        }
        Collections.reverse(path);
        String joined = String.join(" -> ", path);
        return String.format("Shortest path from \"%s\" to \"%s\":%n%s%nTotal weight: %d",
                start, end, joined, dist.get(end));
    }

    /** 计算 PageRank */
    public double calPageRank(String word) {
        final double d = 0.85, eps = 1e-6;
        final int maxIter = 1000;
        // 构造完整节点集
        Map<String, Map<String,Integer>> full = new HashMap<>(adjList);
        for (var e : adjList.values())
            for (String to : e.keySet())
                full.computeIfAbsent(to, k -> new HashMap<>());

        int N = full.size();
        if (N == 0) return 0.0;
        Map<String, Double> pr = new HashMap<>();
        for (String n : full.keySet()) pr.put(n, 1.0 / N);

        Map<String, List<String>> incoming = new HashMap<>();
        for (var from : full.entrySet())
            for (String to : from.getValue().keySet())
                incoming.computeIfAbsent(to, k -> new ArrayList<>()).add(from.getKey());

        for (int iter = 0; iter < maxIter; iter++) {
            Map<String, Double> newPr = new HashMap<>();
            double diff = 0;
            for (String node : full.keySet()) {
                double sum = 0;
                for (String u : incoming.getOrDefault(node, List.of())) {
                    int outdeg = full.get(u).size();
                    if (outdeg > 0) sum += pr.get(u) / outdeg;
                }
                double val = (1 - d) / N + d * sum;
                newPr.put(node, val);
                diff += Math.abs(val - pr.get(node));
            }
            pr = newPr;
            if (diff < eps) break;
        }
        return pr.getOrDefault(word, 0.0);
    }

    /** 随机游走 */
    public String randomWalk() {
        walkPath.clear();
        walkedEdges.clear();
        if (adjList.isEmpty()) return "";

        Random rnd = new Random(System.nanoTime());
        List<String> nodes = new ArrayList<>(adjList.keySet());
        String current = nodes.get(rnd.nextInt(nodes.size()));
        walkPath.add(current);

        while (true) {
            var neighbors = adjList.get(current);
            if (neighbors.isEmpty()) break;
            List<String> cands = new ArrayList<>(neighbors.keySet());
            String next = cands.get(rnd.nextInt(cands.size()));
            String edgeKey = current + "->" + next;
            walkPath.add(next);
            if (walkedEdges.contains(edgeKey)) break;
            walkedEdges.add(edgeKey);
            current = next;
        }
        return String.join(" ", walkPath);
    }

    /** 保存游走路径到文件 */
    public void saveWalkToFile(String filename) {
        try (BufferedWriter out = new BufferedWriter(new FileWriter(filename))) {
            out.write(String.join(" ", walkPath));
        } catch (IOException e) {
            System.err.println("无法写入文件: " + filename);
        }
    }
}
