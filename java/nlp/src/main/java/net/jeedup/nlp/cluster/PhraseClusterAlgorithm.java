package net.jeedup.nlp.cluster;

import net.jeedup.nlp.Phrase;

import java.util.List;

/**
 * TODO
 * Created by zack on 9/14/18.
 */
public interface PhraseClusterAlgorithm {
    List<Phrase> cluserPhrases(List<Phrase> phrases);
}
