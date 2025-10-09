package events;

/**
 * Queue allows programme to store information about happening events.
 * New events can be added by using 'put' method.
 * The earliest event can be accessed by using 'take' method.
 * To check if queue is empty use 'empty' method.
 */
public class Queue implements EventQueue {
    private int queueLength;
    private Event[] events;

    public Queue() {
        this.queueLength = 0;
        this.events = new Event[1];
    }

    /**
     * We assume that current queue is sorted from the latest to the earliest
     * events, which is imposed by the way 'put' method is operating.
     */
    @Override
    public void put(Event event) {
        if (queueLength >= events.length) {
            Event[] temp = new Event[2 * events.length];
            System.arraycopy(events, 0, temp, 0, queueLength);
            events = temp;
        }

        int middle = binSearch(event);

        for (int i = queueLength; i > middle; i--) {
            events[i] = events[i - 1];
        }

        events[middle] = event;
        queueLength++;

    }

    @Override
    public Event take() {
        assert !empty(): "The events Queue is empty";
        queueLength--;
        return events[queueLength];
    }

    @Override
    public boolean empty(){
        return queueLength == 0;
    }

    /**
     * Helper method, used to find proper place in events array.
     * @param event new event which will be put in the queue.
     * @return index of place where event should be put.
     */
    private int binSearch(Event event) {
        int left = 0;
        int right = queueLength - 1;
        int middle;

        while (left <= right) {
            middle = (left + right) / 2;

            if (events[middle].getTime() < event.getTime()) {
                right = middle - 1;
            } else if (events[middle].getTime() > event.getTime()) {
                left = middle + 1;
            } else {
                return middle;
            }
        }

        return left;
    }

}