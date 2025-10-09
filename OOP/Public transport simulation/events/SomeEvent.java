package events;

import publicTransport.Simulation;
import publicTransport.Tram;

/**
 * Abstract class for types of events.
 * New subclasses can be added in order to add new types of events.
 * Can be edited in order to add new methods which will be used
 * similarly to current methods: added to this class and then override in subclass.
 */
public abstract class SomeEvent {
    private boolean vehicleEvent;

    public abstract String toString(); // Every subclass has to has this method,
    // thus it is abstract.

    public boolean isVehicleEvent() {
        return vehicleEvent;
    } // Checks if the event is connected with vehicle.

    public void setVehicleEvent(boolean vehicleEvent) {
        this.vehicleEvent = vehicleEvent;
    }

    public Tram getTram() {
        return null; // By default, returning null, will be overrode in tram event.
    }

    /**
     * Used to realize event of passenger going to their home stop.
     * Only valid if type of event is GoOnStop (otherwise method does not do anything).
     * @param stopCapacity is tha maximum number of passengers that can be on the stop.
     * @param day is the day of the simulation (counting from 0).
     * @param event is the event that is happening.
     */
    public void goOnStop(int stopCapacity, int day, Event event) {
    }

    /**
     * Used to realize event of tram arriving at the stop.
     * Only valid if type of event is TramOnStop.
     * @param simulation current simulation.
     * @param day the day of the simulation (counting from 0).
     * @param event is the event that is happening.
     */
    public void onStop(Simulation simulation, int day, Event event) {
    }
}
