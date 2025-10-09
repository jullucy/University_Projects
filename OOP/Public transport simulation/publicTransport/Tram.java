package publicTransport;

import events.Event;
import events.Queue;
import events.TramOnStop;

/**
 * Class representing a tram.
 * It has special toString method which allows it to print it as a tram in
 * our simulation.
 */
public class Tram extends Vehicle {
    public Tram(int sideNumber, TramLine tramLine, int capacity) {
        super(sideNumber, tramLine, capacity);
    }

    @Override
    public String toString() {
        return ("tram number " + super.getLine().getNumber() + " (side number " +
                super.getSideNumber() + ") ");
    }

    /**
     * Allows to put new event in event queue - tram arrival at next stop.
     * @param events the event queue to which we will add new element.
     */
    public void next(Queue events) {
        nextStop();
        Stop stop = super.getCurrentStop();

        TramOnStop event = new TramOnStop(this, stop);
        Event temp = new Event(event, getTime());
        events.put((temp));
    }
}