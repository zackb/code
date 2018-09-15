package net.jeedup.nlp.interesting;

import com.aliasi.lm.TokenizedLM;
import com.aliasi.tokenizer.IndoEuropeanTokenizerFactory;
import com.aliasi.tokenizer.TokenizerFactory;
import com.aliasi.util.ScoredObject;
import net.jeedup.nlp.Phrase;

import java.util.ArrayList;
import java.util.List;
import java.util.SortedSet;

import static net.jeedup.common.util.StringUtil.isCapitalized;

/**
 * TODO
 * Created by zack on 9/14/18.
 */
public class InterestingPhraseDetectionAlgorithm {

    private static final TokenizerFactory TOKENIZER_FACTORY = IndoEuropeanTokenizerFactory.INSTANCE;

    private static int MIN_COUNT = 5;
    private static int NGRAM_REPORTING_LENGTH = 2;
    private static int MAX_COUNT = 100;
    private static int NGRAM = 3;

    private final int minCount;
    private final int ngramReportingLength;
    private final int maxCount;
    private final int ngram;

    private final TokenizedLM model;

    public InterestingPhraseDetectionAlgorithm() {
        this(MIN_COUNT, MAX_COUNT, NGRAM, NGRAM_REPORTING_LENGTH);
    }

    public InterestingPhraseDetectionAlgorithm(int minCount, int maxCount, int ngram, int ngramReportingLength) {
        this.minCount = minCount;
        this.maxCount = maxCount;
        this.ngram = ngram;
        this.ngramReportingLength = ngramReportingLength;
        this.model = new TokenizedLM(TOKENIZER_FACTORY, ngram);
    }

    public void addPhrase(Phrase phrase) {
        model.handle(phrase.text);
    }

    public List<Phrase> getSignificantPhrases() {
        return getSignificantPhrases(0, false);
    }

    // collocated significant terms
    public List<Phrase> getSignificantPhrases(int minCount, boolean capitalizedOnly) {

        SortedSet<ScoredObject<String[]>> coll =
                model.collocationSet(ngramReportingLength, this.minCount, maxCount);

        if (minCount > 0) {
            model.sequenceCounter().prune(minCount);
        }

        List<Phrase> phrases = new ArrayList<>();

        for (ScoredObject<String[]> col : coll) {
            double score = col.score();
            String[] toks = col.getObject();

            boolean add = true;
            if (capitalizedOnly) {
                for (int i = 0; i < toks.length; i++) {
                    if (isCapitalized(toks[i])) {
                        add = false;
                        break;
                    }
                }
            }


            if (add) {
                String text = String.join(" ", toks);
                phrases.add(new Phrase(text, score));
            }
        }

        return phrases;
    }
}
