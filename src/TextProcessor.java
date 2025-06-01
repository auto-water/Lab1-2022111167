// TextProcessor.java
import java.util.ArrayList;
import java.util.List;

public class TextProcessor {
    /** 工具函数：统一小写 + 去掉非字母字符 */
    public static String cleanText(String text) {
        StringBuilder sb = new StringBuilder();
        for (char c : text.toCharArray()) {
            if (Character.isLetter(c)) {
                sb.append(Character.toLowerCase(c));
            } else if (Character.isWhitespace(c)) {
                sb.append(' ');
            } else {
                sb.append(' '); // 标点转空格
            }
        }
        return sb.toString();
    }

    /** 分词函数 */
    public static List<String> tokenize(String line) {
        List<String> tokens = new ArrayList<>();
        for (String w : line.trim().split("\\s+")) {
            if (!w.isEmpty()) tokens.add(w);
        }
        return tokens;
    }
}
