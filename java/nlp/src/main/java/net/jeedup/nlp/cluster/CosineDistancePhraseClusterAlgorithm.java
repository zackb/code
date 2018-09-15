package net.jeedup.nlp.cluster;

import com.aliasi.cluster.CompleteLinkClusterer;
import com.aliasi.cluster.Dendrogram;
import com.aliasi.cluster.HierarchicalClusterer;
import com.aliasi.tokenizer.IndoEuropeanTokenizerFactory;
import com.aliasi.tokenizer.Tokenizer;
import com.aliasi.tokenizer.TokenizerFactory;
import com.aliasi.util.Counter;
import com.aliasi.util.Distance;
import com.aliasi.util.ObjectToCounterMap;
import net.jeedup.nlp.Phrase;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;

import static java.util.Collections.emptyList;
import static net.jeedup.common.util.Util.empty;
import static net.jeedup.nlp.util.StringUtil.removeIgnorableWordsFromNews;

/**
 * TODO
 * Created by zack on 9/14/18.
 */
public class CosineDistancePhraseClusterAlgorithm implements PhraseClusterAlgorithm {

    private static final Logger log =
            Logger.getLogger(CosineDistancePhraseClusterAlgorithm.class.getSimpleName());

    static final TokenizerFactory TOKENIZER_FACTORY = tokenizerFactory();

    static TokenizerFactory tokenizerFactory() {
        TokenizerFactory factory = IndoEuropeanTokenizerFactory.INSTANCE;
        // factory  = new LowerCaseTokenizerFactory(factory)
        // factory = new EnglishStopTokenizerFactory(factory)
        // factory = new PorterStemmerTokenizerFactory(factory)
        return factory;
    }

    private static final Distance<Document> COSINE_DISTANCE = (doc1, doc2) -> {
        double oneMinusCosine = 1.0 - doc1.cosine(doc2);
        if (oneMinusCosine > 1.0)
            return 1.0;
        else if (oneMinusCosine < 0.0)
            return 0.0;
        else
            return oneMinusCosine;
    };

    @Override
    public List<Phrase> cluserPhrases(List<Phrase> phrases) {

        Set<Document> docSet = new HashSet<>();

        for (Phrase phrase : phrases) {
            Document doc = new Document(phrase);
            docSet.add(doc);
        }

        if (empty(docSet)) {
            return emptyList();
        }

        HierarchicalClusterer<Document> clusterer = new CompleteLinkClusterer<>(COSINE_DISTANCE);

        Dendrogram<Document> dendrogram = null;
        try {
            dendrogram = clusterer.hierarchicalCluster(docSet);
        } catch(IllegalArgumentException iae) {
            log.log(Level.WARNING, "Failed clustering: ", iae);
            return emptyList();
        }

        Set<Set<Document>> partition = dendrogram.partitionDistance(0.5);

        List<Phrase> result = new ArrayList<>();
        for (Set<Document> set : partition) {
            Phrase main = null;
            for (Document doc : set) {
                if (main == null) {
                    main = doc.phrase;
                    result.add(main);
                } else {
                    main.relatedPhrases.add(doc.phrase);
                }
            }
        }

        return result;
    }

    private static class Document {

        final ObjectToCounterMap<String> tokenCounter = new ObjectToCounterMap<>();
        final double length;
        final Phrase phrase;

        private Document(Phrase phrase) {
            this.phrase = phrase;

            String string = phrase.text;
            string = removeIgnorableWordsFromNews(string);
            Tokenizer tokenizer = TOKENIZER_FACTORY.tokenizer(string.toCharArray(), 0, string.length());
            String token = null;
            while ((token = tokenizer.nextToken()) != null)
                tokenCounter.increment(token.toLowerCase());

            length = length(tokenCounter);
        }

        double cosine(Document doc) {
            return product(doc) / (length * doc.length);
        }

        double product(Document thatDoc) {
            double sum = 0.0;
            for (String token : tokenCounter.keySet()) {
                int count = thatDoc.tokenCounter.getCount(token);
                if (count == 0) {
                    continue;
                }
                sum += Math.sqrt(count * tokenCounter.getCount(token));
            }

            return sum;
        }

        static double length(ObjectToCounterMap<String> otc) {
            double sum = 0.0;
            for (Counter counter : otc.values()) {
                double count = counter.doubleValue();
                sum += count;
            }
            return Math.sqrt(sum);
        }
    }
}
