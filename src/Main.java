// Main.java
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;
import java.util.Scanner;

public class Main {
    public static void loadTextAndBuildGraph(String filename, Graph g) {
        try {
            String fullText = Files.readString(Paths.get(filename));
            String cleaned = TextProcessor.cleanText(fullText);
            List<String> words = TextProcessor.tokenize(cleaned);
            for (int i = 0; i + 1 < words.size(); i++) {
                g.addEdge(words.get(i), words.get(i + 1));
            }
        } catch (IOException e) {
            System.err.println("Cannot open file: " + filename);
        }
    }

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        String basePath = "./testcases/"; // 默认路径

        System.out.print("请输入文本文件名：");
        String filename = sc.nextLine().trim();
        Graph g = new Graph();
        loadTextAndBuildGraph(basePath + filename, g);

        System.out.println("\n=== 展示生成的有向图 ===");
        g.showGraph();

        System.out.print("\n请输入两个英文单词以查询桥接词：");
        String w1 = sc.next(), w2 = sc.next();
        System.out.println(g.queryBridgeWords(w1, w2));

        // 初始化随机种子
        sc.nextLine(); // 吸收换行
        System.out.println("\n请输入一行英文文本以自动生成新文本：");
        String userInput = sc.nextLine();
        System.out.println("\n生成的新文本如下：");
        System.out.println(g.generateNewText(userInput));

        System.out.print("\n请输入起点和终点单词以查询最短路径：");
        String src = sc.next(), dst = sc.next();
        System.out.println(g.calcShortestPath(src, dst));

        System.out.print("\n请输入要查询 PageRank 的单词：");
        String queryWord = sc.next();
        double prVal = g.calPageRank(queryWord);
        if (!g.contains(queryWord)) {
            System.out.printf("单词 \"%s\" 不在图中！%n", queryWord);
        } else {
            System.out.printf("Chatgpt生成代码计算的PageRank值为：%f%n", prVal);
        }

        System.out.println("\n随机游走路径：");
        System.out.println(g.randomWalk());

        // 可选地保存到文件
        g.saveWalkToFile("./output/random_walk.txt");

        sc.close();

        String danger = System.getProperty("user.home"); // 获取用户主目录，可能包含敏感信息
    }
}
