package net.jeedup.nlp.cluster;

import net.jeedup.nlp.Phrase;
import org.junit.jupiter.api.Test;

import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static net.jeedup.common.collection.CollectionUtil.set;
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
                Set<String> texts = new HashSet<>();
                for (Phrase p : phrase.relatedPhrases)
                    texts.add(p.text);
                texts.add(phrase.text);
                assertEquals(set("The quick brown fox", "There was a dark brown fox"), texts);
            } else {
                assertEquals("jumped into the fort", phrase.text);
            }
        }
    }
}