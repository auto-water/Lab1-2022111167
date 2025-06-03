import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;

class BlackBoxTest {

  private Graph createTestGraphFromFile(String filename) {
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
  void testTC1_oneBridgeWord() {
    Graph graph = createTestGraphFromFile("./testcases/EasyTest.txt");
    if (graph == null) fail("Failed to load graph from file.");
    String result = graph.queryBridgeWords("but", "data");
    assertEquals("The bridge words from \"but\" to \"data\" is: \"the\".", result);
  }

  @Test
  void testTC2_multipleBridgeWords() {
    Graph graph = createTestGraphFromFile("./testcases/EasyTest.txt");
    if (graph == null) fail("Failed to load graph from file.");
    String result = graph.queryBridgeWords("scientist", "analyzed");
    assertEquals("The bridge words from \"scientist\" to \"analyzed\" are: \"carefully\", \"carelessly\".", result);
  }

  @Test
  void testTC3_word1NotInGraph() {
    Graph graph = createTestGraphFromFile("./testcases/EasyTest.txt");
    if (graph == null) fail("Failed to load graph from file.");
    String result = graph.queryBridgeWords("apple", "data");
    assertEquals("No \"apple\" in the graph!", result);
  }

  @Test
  void testTC4_word2NotInGraph() {
    Graph graph = createTestGraphFromFile("./testcases/EasyTest.txt");
    if (graph == null) fail("Failed to load graph from file.");
    String result = graph.queryBridgeWords("but", "banana");
    assertEquals("No \"banana\" in the graph!", result);
  }

  @Test
  void testTC5_noBridgeWord() {
    Graph graph = createTestGraphFromFile("./testcases/EasyTest.txt");
    if (graph == null) fail("Failed to load graph from file.");
    String result = graph.queryBridgeWords("wrote", "report");
    assertEquals("No bridge words from \"wrote\" to \"report\"!", result);
  }

  @Test
  void testTC6_invalidWord1() {
    Graph graph = createTestGraphFromFile("./testcases/EasyTest.txt");
    if (graph == null) fail("Failed to load graph from file.");
    String result = graph.queryBridgeWords("data!", "the");
    assertEquals("No \"data!\" in the graph!", result); // cleanText removes the !
  }

  @Test
  void testTC7_invalidWord2() {
    Graph graph = createTestGraphFromFile("./testcases/EasyTest.txt");
    if (graph == null) fail("Failed to load graph from file.");
    String result = graph.queryBridgeWords("but", "report?");
    assertEquals("No \"report?\" in the graph!", result); // cleanText removes the ?
  }
}