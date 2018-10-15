package net.jeedup.nlp.interesting;

import net.jeedup.nlp.Phrase;
import org.junit.jupiter.api.Test;

import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

class ImprobablePhraseDetectionAlgorithmTest {

    @Test
    void testImprobablePhrases() {

        ImprobablePhraseDetectionAlgorithm algo =
                new ImprobablePhraseDetectionAlgorithm(1, 100, 3, 2);

        List<Phrase> phrases = Arrays.asList(
                new Phrase("The quick brown fox"),
                new Phrase("The quick brown fox"),
                new Phrase("There was a dark brown fox"),
                new Phrase("There was a dark brown fox"),
                new Phrase("The quick brown fox"),
                new Phrase("The quick brown fox"),
                new Phrase("The quick brown fox"),
                new Phrase("The quick red brown fox"),
                new Phrase("jumped into the fort"),
                new Phrase("There was a dark brown fox")
        );

        phrases.forEach(algo::addBackgroundPhrase);

        List<Phrase> newPrases = Arrays.asList(
                new Phrase("The quick brown fox"),
                new Phrase("There was a dark blue fox"),
                new Phrase("The quick brown fox"),
                new Phrase("The quick brown fox"),
                new Phrase("The quick red brown fox"),
                new Phrase("Someone went cuckooo"),
                new Phrase("That Someone was a real loser"),
                new Phrase("jumped into the fort"),
                new Phrase("There was a dark brown fox")
        );

        newPrases.forEach(algo::addPhrase);

        List<Phrase> improbable = algo.getImprobablePhrases(3, true);
        System.out.println(improbable);
    }


    @Test
    void testSmallSet() throws Exception {
        ImprobablePhraseDetectionAlgorithm algo =
            new ImprobablePhraseDetectionAlgorithm(3, 100, 3, 2);

        train(algo);
        tst(algo);

        List<Phrase> improbable = algo.getImprobablePhrases(0, true);

        assertContains("Supreme Court", improbable);
        assertContains("Sheikh Sabah", improbable);
        assertContains("Chaim Weizmann", improbable);
        assertContains("Bosnian Relief", improbable);
    }

    void assertContains(String subject, List<Phrase> phrases) {
        assertTrue(contains(subject, phrases));
    }

    boolean contains(String subject, List<Phrase> phrases) {
        for (Phrase phrase : phrases)
            if (subject.equals(phrase.text))
                return true;
        return false;
    }

    void train(ImprobablePhraseDetectionAlgorithm algo) throws Exception {
        Path path = Paths.get(ImprobablePhraseDetectionAlgorithm.class
                .getResource("/talk.politics.mideast/train").getFile());


        Files.list(path).forEach( p -> {
            try {
                byte[] bytes = Files.readAllBytes(p);
                String text = new String(bytes, StandardCharsets.ISO_8859_1);
                algo.addBackgroundPhrase(new Phrase(text));
            } catch (Exception e) {
                e.printStackTrace();
            }
        });
    }

    void tst(ImprobablePhraseDetectionAlgorithm algo) throws Exception {
        Path path = Paths.get(ImprobablePhraseDetectionAlgorithm.class
                .getResource("/talk.politics.mideast/test").getFile());

        Files.list(path).forEach( p -> {
            try {
                byte[] bytes = Files.readAllBytes(p);
                String text = new String(bytes, StandardCharsets.ISO_8859_1);
                algo.addPhrase(new Phrase(text));
            } catch (Exception e) {
                e.printStackTrace();
            }
        });
    }
}