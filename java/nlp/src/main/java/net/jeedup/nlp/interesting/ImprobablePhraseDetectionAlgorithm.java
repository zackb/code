package net.jeedup.nlp.interesting;

import com.aliasi.lm.TokenizedLM;
import com.aliasi.util.ScoredObject;
import net.jeedup.nlp.Phrase;

import java.util.List;
import java.util.SortedSet;

import static net.jeedup.nlp.interesting.InterestingPhraseDetectionAlgorithm.toPhrases;

/**
 * Find "statistically improbably phrases" by comparing a background or "historical" corpus against a new one.
 * Rising trends in phrases for instance.
 *
 * Created by zack on 10/15/18.
 */
public class ImprobablePhraseDetectionAlgorithm {

    // the background phrase model
    private final InterestingPhraseDetectionAlgorithm background;

    // the newer phrases to find the improbably ones in
    private final InterestingPhraseDetectionAlgorithm foreground;

    public ImprobablePhraseDetectionAlgorithm() {
        background = new InterestingPhraseDetectionAlgorithm();
        foreground = new InterestingPhraseDetectionAlgorithm();
    }

    public ImprobablePhraseDetectionAlgorithm(int minCount, int maxCount, int ngram, int ngramReportingLength) {
        background = new InterestingPhraseDetectionAlgorithm(minCount, maxCount, ngram, ngramReportingLength);
        foreground = new InterestingPhraseDetectionAlgorithm(minCount, maxCount, ngram, ngramReportingLength);
    }

    public void addBackgroundPhrase(Phrase phrase) {
        background.addPhrase(phrase);
    }

    public void addPhrase(Phrase phrase) {
        foreground.addPhrase(phrase);
    }

    public List<Phrase> getImprobablePhrases() {
        return getImprobablePhrases(0, false);
    }

    public List<Phrase> getImprobablePhrases(int minCount, boolean capitalizedOnly) {
        TokenizedLM bmodel = background.getModel();
        TokenizedLM fmodel = foreground.getModel();

        bmodel.sequenceCounter().prune(minCount);
        fmodel.sequenceCounter().prune(minCount);

        SortedSet<ScoredObject<String[]>> newTerms = fmodel.newTermSet(foreground.getNgramReportingLength(),
                foreground.getMinCount(),
                foreground.getMaxCount(),
                bmodel);

        return toPhrases(newTerms, capitalizedOnly);
    }
}
