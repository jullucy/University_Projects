package events;

/**
 * Interface for Event queue.
 */
public interface EventQueue {
    /**
     * Adds new event to the queue according to its time.
     * @param event is new event, which will be put in array.
     */
    void put(Event event);

    /**
     * @return earliest event in the queue.
     */
    Event take();

    /**
     * Checks if the queue is empty.
     * @return true if queue is empty, false if it is not.
     */
    boolean empty();
}
