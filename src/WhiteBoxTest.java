import org.junit.jupiter.api.Test;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static org.junit.jupiter.api.Assertions.*;

class WhiteBoxTest {

  private Graph createTestGraphFromFile() {
    String filename = "./testcases/EasyTest.txt";
    Graph graph = new Graph();
    try {
      String fullText = Files.readString(Paths.get(filename));
      String cleaned = TextProcessor.cleanText(fullText);
      List<String> words = TextProcessor.tokenize(cleaned);
      for (int i = 0; i + 1 < words.size(); i++) {
        graph.addEdge(words.get(i), words.get(i + 1));
      }
    } catch (IOException e) {
      System.err.println("Cannot open file: " + filename);
      return null; // Or throw an exception, depending on your error handling
    }
    return graph;
  }

  private Graph createEmptyGraph() {
    return new Graph();
  }

  @Test
  void calcShortestPath_Path1() {
    Graph graph = createTestGraphFromFile();
    String result = graph.calcShortestPath("invalid_start", "the");
    assertEquals("No \"invalid_start\" in the graph!", result);
  }

  @Test
  void calcShortestPath_Path2() {
    Graph graph = createTestGraphFromFile();
    String result = graph.calcShortestPath("but", "invalid_end");
    assertEquals("No \"invalid_end\" in the graph!", result);
  }

  @Test
  void calcShortestPath_Path3() {
    Graph graph = createTestGraphFromFile();
    String result = graph.calcShortestPath("again", "the");
    assertEquals("No path from \"again\" to \"the\"!", result);
  }

  @Test
  void calcShortestPath_Path4() {
    Graph graph = createTestGraphFromFile();
    String result = graph.calcShortestPath("scientist", "report");
    assertEquals("Shortest path from \"scientist\" to \"report\":" +
        "\nscientist -> analyzed -> the -> report\n" +
        "Total weight: 3", result);
  }
}