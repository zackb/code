package net.jeedup.message.broker.impl;

import net.jeedup.message.Message;
import net.jeedup.message.broker.MessageBroker;
import net.jeedup.message.broker.MessageConsumer;

import java.util.Collection;
import java.util.concurrent.*;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * In memory, not persisted broker
 * Created by zack on 9/11/18.
 */
public class InMemoryBroker<T> implements MessageBroker<T> {

    private static final Logger log = Logger.getLogger(InMemoryBroker.class.getName());

    private final BlockingQueue<Message<T>> queue;
    private ExecutorService executor;

    private static final ThreadFactory threadFactory = new ThreadFactory() {
        final ThreadFactory factory = Executors.defaultThreadFactory();
        @Override
        public Thread newThread(final Runnable r) {
            final Thread result = factory.newThread(r);
            result.setName("in-memory-queue-" + result.getName());
            result.setDaemon(true);
            return result;
        }
    };

    public InMemoryBroker() {
        this(Integer.MAX_VALUE);
    }

    public InMemoryBroker(int maxSize) {
        this.queue = new LinkedBlockingQueue<>(maxSize);
    }

    @Override
    public void publish(Message<T> message) {
        queue.offer(message);
    }

    @Override
    public void publish(Collection<Message<T>> messages) throws Exception {
        for (Message<T> message : messages) {
            publish(message);
        }
    }

    @Override
    public void consume(MessageConsumer<T> consumer) {
        executor.submit(() -> {
            while (!executor.isShutdown()) {
                try {
                    // wait for new data and add it to the pending list if its available
                    Message<T> msg = queue.poll(1000, TimeUnit.MILLISECONDS);

                    if (msg != null)
                        consumer.consume(msg);

                } catch (Exception e) {
                    log.log(Level.SEVERE, "Failed consuming data", e);
                }
            }

            log.info("Stopping batching thread");
        });
    }

    @Override
    public void startup() {
        executor = Executors.newSingleThreadExecutor(threadFactory);
    }

    @Override
    public void shutdown() {
        executor.shutdown();
        queue.clear();
    }
}
