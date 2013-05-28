package text

import  (
    "strings"
    "regexp"
)

// Build a summary of a title and content
func Summarize(title, content string, paras []string) string {
    var sentences_map map[string]int
    if paras == nil {
        paras = paragraphs(content)
    }

    sentences_map = sentencesRanks(strings.Join(paras, "\n\n"))
    summary := []string{title}

    // highest scoring sentence from each paragraph
    for _,p := range paras {
        sentence := strings.TrimSpace(bestSentence(p, sentences_map))
        if sentence != ""   {
            summary = append(summary, sentence)
        }
    }

    return strings.Join(summary, ". ")
}

// Naive method for splitting a text into sentences
func sentences(content string) []string    {
    content = strings.Replace(content, "\n", ". ", -1)
    return strings.Split(content, ". ")
}

// Naive method for splitting a text into paragraphs
func paragraphs(content string) []string {
    return strings.Split(content, "\n\n")
}

// Caculate the intersection between two sentences
func sentencesIntersection(sent1, sent2 string) int {
    // split the sentence into words/tokens
    s1 := strings.Split(sent1, " ") //unique
    s2 := strings.Split(sent2, " ") //unique

    // if no intersection return 0
    if len(s1) + len(s2) == 0 {
        return 0
    }

    // normalize result by average number of words
    return len(wordIntersection(s1, s2)) / ((len(s1) + len(s2)) / 2)
}

func wordIntersection(arr1, arr2 []string) []string {
    res := []string{}
    for _, s1 := range arr1   {
        for _,s2 := range arr2    {
            if s1 == s2 {
                res = append(res, s1)
            }
        }
    }
    return res
}

// Format a sentence, remove all non-alphbetic chars from the sentence
var formatSentenceReg, _ = regexp.Compile("[^A-Za-z0-9]+")
func formatSentence(sentence string) string {
    safe := formatSentenceReg.ReplaceAllString(sentence, "-")
    safe = strings.ToLower(strings.Trim(safe, "-"))
    return safe
}

var alphaNumericReg, _ = regexp.Compile("[^A-Za-z0-9.]+")
func AlphaNumeric(sentence string) string {
    return strings.TrimSpace(alphaNumericReg.ReplaceAllString(sentence, " "))
}

// Convert the content into a map[K]V
// k = The formatted sentence
// V = The rank of the sentence
func sentencesRanks(content string) map[string]int {
    // Split the content into sentences
    sentences := sentences(content)

    // calculate the intersection of every two sentences
    n := len(sentences)
    values := make([][]int, len([]int{n}))
    for i := 0; i < n; i++  {
        values = append(values, []int{n})
        for j := 0; j < n; j++  {
            values[i] = append(values[i], sentencesIntersection(sentences[i], sentences[j]))
        }
    }

    // build the sentences map
    // the score of a sentences is the sum of all its intersection
    sentences_map := map[string]int{}
    for i := 0; i < n; i++ {
        score := 0
        for j := 0; j < n; j++ {
            if i == j {
                continue
            }
            score += values[i][j]
        }
        sentences_map[formatSentence(sentences[i])] = score
    }
    return sentences_map
}

 // Return the highest scoring sentence in a paragraph
func bestSentence(paragraph string, sentences_map map[string]int) string {

    // split the paragraph into sentences
    sentences := sentences(paragraph)

    if len(sentences) < 2 {
        return ""
    }

    // get sentence with highest score according to the sentences map
    best_sentence := ""
    max_value := 0
    for _,s := range sentences {
        strip_s := formatSentence(s)
        if strip_s != "" {
            if sentences_map[strip_s] > max_value {
                max_value = sentences_map[strip_s]
                best_sentence = s
            }
        }
    }
    return best_sentence
}
