package net.jeedup.nlp.interesting;

import net.jeedup.nlp.Phrase;
import org.junit.jupiter.api.Test;

import java.util.Arrays;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

class InterestingPhraseDetectionAlgorithmTest {

    @Test
    void testSimple() {
        InterestingPhraseDetectionAlgorithm algo = new InterestingPhraseDetectionAlgorithm();

        List<Phrase> phrases = Arrays.asList(
                new Phrase("The quick brown fox"),
                new Phrase("The quick brown fox"),
                new Phrase("The quick brown fox"),
                new Phrase("There was a dark brown fox"),
                new Phrase("There was a dark brown fox"),
                new Phrase("There was a dark brown fox"),
                new Phrase("The quick brown fox"),
                new Phrase("The quick brown fox"),
                new Phrase("The quick brown fox"),
                new Phrase("The quick brown fox"),
                new Phrase("The quick brown fox"),
                new Phrase("The quick red brown fox"),
                new Phrase("jumped into the fort"),
                new Phrase("There was a dark brown fox"),
                new Phrase("There was a dark brown fox")
        );

        phrases.forEach(algo::addPhrase);

        List<Phrase> sig = algo.getSignificantPhrases(3, false);

        Phrase brownFox = sig.stream()
                .filter(p -> p.text.equals("brown fox"))
                .findFirst()
                .orElse(null);

        assertNotNull(brownFox);
        assertEquals(81.9999, brownFox.score, 0.001);
    }
}