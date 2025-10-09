package events;

import publicTransport.Passenger;
import publicTransport.Stop;

/**
 * Class representing event of passenger going to the stop.
 * Comparing to other events connected with passengers, this has to be added to queue,
 * because it is not connected with any tram event.
 */
public class GoOnStop extends SomeEvent{
    private final Passenger passenger;

    public GoOnStop(Passenger passenger) {
        this.passenger = passenger;
        super.setVehicleEvent(false);
    }

    @Override
    public String toString() {
        return "Passenger " + passenger.getNumber() + " decided to cancel their tour," +
                " due to the lack of space on the stop";
    }

    @Override
    public void goOnStop(int stopCapacity, int day, Event event) {
        Stop home = passenger.getHomeStop();

        if (home.getNumberOfPassengers() < stopCapacity) {
            home.newPassenger(passenger);
            System.out.print("day " + day + ", " + event.time() + ": ");
            System.out.println("Passenger " + passenger.getNumber() + " went to their home stop " +
                    home + ".");
        }
        else  {
            System.out.println("day " + day + ", " + event);
        }
    }

}
