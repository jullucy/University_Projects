package events;

import publicTransport.Simulation;
import publicTransport.Stop;
import publicTransport.Tram;

/**
 * Class representing event of tram arriving at stop.
 * 'stop' is a stop at which the tram is arriving.
 */
public class TramOnStop extends SomeEvent {
    private final Tram tram;
    private final Stop stop;

    public TramOnStop(Tram tram, Stop stop) {
        this.tram = tram;
        this.stop = stop;
        super.setVehicleEvent(true);
    }

    @Override
    public Tram getTram() {
        return tram;
    }

    @Override
    public String toString() {
        return tram.toString() + "stops at the " + stop.toString() + " stop.";
    }

    public void onStop(Simulation simulation, int day, Event event) {
        System.out.println("day " + day + ", " + event);

        int number = tram.canGetOut(stop, day, event.getTime());
        int time = stop.checkIfGetOnTram(tram, day, event.getTime());
        simulation.setNumberOfRides(number);
        simulation.setTimeOfWaiting(time);

        tram.next(simulation.getEvents());
    }
}