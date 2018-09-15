package net.jeedup.nlp.cluster;

import net.jeedup.nlp.Phrase;
import org.junit.jupiter.api.Test;

import java.util.Arrays;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

class CosineDistancePhraseClusterAlgorithmTest {
    @Test
    void testClusterSimple() {

        PhraseClusterAlgorithm algo = new CosineDistancePhraseClusterAlgorithm();

        List<Phrase> phrases = Arrays.asList(
                new Phrase("The quick brown fox"),
                new Phrase("jumped into the fort"),
                new Phrase("There was a dark brown fox")
        );

        List<Phrase> clusers = algo.cluserPhrases(phrases);

        assertNotNull(clusers);
        assertEquals(2, clusers.size());
        for (Phrase phrase : clusers) {
            if (phrase.text.contains("fox")) {
                assertEquals("There was a dark brown fox", phrase.text);
                assertEquals("The quick brown fox", phrase.relatedPhrases.iterator().next().text);
            } else {
                assertEquals("jumped into the fort", phrase.text);
            }
        }
    }
}