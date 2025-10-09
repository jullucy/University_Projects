package publicTransport;

import events.*;

import java.util.Scanner;
import java.util.Objects;

import static events.Event.convertTime;
import static publicTransport.Draw.draw;

/**
 * Simulation is the main class, it is used to control whole simulation.
 * It reads data form the standard input and prints it.
 * It also gives the statistics of the simulation. It can be edited,
 * ex. new methods for statistics can be added.
 */
public class Simulation {
    private final int numberOfDays;
    private final int stopCapacity;
    private final int numberOfStops;
    private final Stop[] stops;
    private final int numberOfPassengers;
    private final int tramCapacity;
    private final int numberOfLines;
    private final TramLine[] tramLines;
    private Tram[] trams;
    private Passenger[] passengers;
    private final Queue events;
    private int numberOfRides;
    private int timeOfWaiting;
    private int howManyWaited;

    /**
     * Simulation method allows to read data form the standard input and create
     * a new simulation object based on it.
     * On the input user should put (in given order):
     *   numer of days of simulation (int);
     *   stop capacity (int);
     *   number of stops (int);
     *   for every stop:
     *       name of the stop (String), names should differ, otherwise it will
     *       lead to errors;
     *   number of passengers (int);
     *   tram capacity (int);
     *   number of tram lines (int);
     *   for every line:
     *       number of trams of this line (int),
     *       length of the route (int),
     *       for every stop on the route:
     *           name of the stop (String),
     *           time of getting on this stop from previous stop (String)
     *           (the last given time is the time of waiting on the loop);
     */
    public Simulation() {
        Scanner reader = new Scanner(System.in);
        this.numberOfDays = reader.nextInt();
        this.stopCapacity = reader.nextInt();
        this.numberOfStops = reader.nextInt();
        this.stops = new Stop[numberOfStops];

        for (int i = 0; i < numberOfStops; i++) {
            stops[i] = new Stop(reader.next(), stopCapacity);
        }

        this.numberOfPassengers = reader.nextInt();
        this.tramCapacity = reader.nextInt();
        this.numberOfLines = reader.nextInt();
        this.tramLines = new TramLine[numberOfLines];

        for (int i = 0; i < numberOfLines; i++) {
            int a = reader.nextInt();
            int b = reader.nextInt();
            tramLines[i] = new TramLine(i, a, b);
            int[] temp1 = new int[tramLines[i].getLengthOfRoute()];
            Stop[] temp2 = new Stop[tramLines[i].getLengthOfRoute()];

            for (int j = 0; j < tramLines[i].getLengthOfRoute(); j++) {
                String name = reader.next();
                int index = 0;
                while (!Objects.equals(stops[index].toString(), name)) index++;
                temp2[j] = stops[index]; // We assume that given stop is in the
                // array of stops
                temp1[j] = reader.nextInt();
            }

            tramLines[i].setRoute(temp2, temp1);
        }

        this.events = new Queue();
        this.numberOfRides = 0;
        this.timeOfWaiting = 0;
        this.howManyWaited = 0;
    }

    /**
     * Allows to print read data (in order it was given,
     * without additional structures that was created)
     */
    public void write() {
        System.out.println(numberOfDays);
        System.out.println(stopCapacity);
        System.out.println(numberOfStops);

        for (int i = 0; i < numberOfStops; i++) {
            System.out.println(stops[i].toString());
        }

        System.out.println(numberOfPassengers);
        System.out.println(tramCapacity);
        System.out.println(numberOfLines);

        for (int i = 0; i < numberOfLines; i++) {
            System.out.println(tramLines[i].toString());
        }
    }

    public void addWaitingTime(int time) {
        this.timeOfWaiting += time;
    }

    public void addHowManyWaited(int howMany) {
        this.howManyWaited += howMany;
    }

    public void setNumberOfRides(int numberOfRides) {
        this.numberOfRides += numberOfRides;
    }

    public void setTimeOfWaiting(int timeOfWaiting) {
        this.timeOfWaiting += timeOfWaiting;
    }

    public Queue getEvents() {
        return events;
    }

    /**
     * Creates trams that will be used in the simulation.
     * Gives them number, tram line and capacity that won't be changed during
     * the simulation.
     */
    public void createTrams() {
        int size = numberOfLines;
        this.trams = new Tram[size];
        int number = 0;

        for (int i = 0; i < numberOfLines; i++) {
            for (int j = 0; j < tramLines[i].getNumberOfVehicles(); j++) {
                trams[number] = new Tram(number, tramLines[i], tramCapacity);
                number++;

                if (number == size) {
                    size *= 2;
                    Tram[] temp = new Tram[size];
                    System.arraycopy(trams, 0, temp, 0, number);
                    this.trams = temp;
                }

            }
        }
    }

    /**
     * Starts the trams at the beginning of every simulation day.
     * Half of the trams starts from first and half from the second side. If the
     * number of trams is odd one more tram starts from the first side.
     * Trams start in gaps counted by the time of riding to both sides (including
     * waiting on the loop) divided by the number of trams ont the route.
     * Can be changed for example in order to change the way trams are starting
     * their route.
     */
    public void startTrams() {
        int index = 0;
        for (int i = 0; i < numberOfLines; i++) {
            int gaps = tramLines[i].countGaps();
            int n = tramLines[i].getNumberOfVehicles();

            for (int j = 0; j < n; j++) {
                if (j < (n / 2)) {
                    trams[index].start(360 + j * gaps, true);
                }
                else {
                    trams[index].start(360 + (j - (n / 2)) * gaps, false);
                }

                Stop stop = trams[index].getCurrentStop();
                TramOnStop event = new TramOnStop(trams[index], stop);
                Event temp = new Event(event, trams[index].getTime());
                this.events.put(temp);
                index++;
            }
        }
    }

    /**
     * Creates passengers at the beginning of simulation.
     * Gives them number (characteristic for every passenger) and home stop
     * that was generated pseudo-randomly.
     */
    public void initializePassengers() {
        passengers = new Passenger[numberOfPassengers];
        Stop stop;

        for (int i = 0; i < numberOfPassengers; i++) {
            stop = stops[draw(0, numberOfStops - 1)];
            passengers[i] = new Passenger(i, stop);
        }
    }

    /**
     * Method used at the beginning of the day to send the passengers to their
     * home stops
     * on pseudo-random hour between 6 and 12 am.
     */
    public void goOnStop() {
        int time;

        for (int i = 0; i < numberOfPassengers; i++) {
            time = draw(360, 720);
            passengers[i].goOnStop(events, time);
        }
    }

    /**
     * Simulation controller, which is simulating the flow of a single day.
     * Manages the usage of the queue, according to type of event that is
     * currently happening.
     * @param day the day of the simulation (counting from 0).
     */
    public void simulate(int day) {
        Stop stop;
        boolean signal = false;

        while (!events.empty()) {
            Event event = events.take();

            // Loop last until the time of the event exceed 23
            // (so in minutes - it is 1380th minute of the day).
            while (!signal) {
                if (event.getTime() > 1380) {
                    signal = true;
                }
                else {
                    if (event.getEvent().isVehicleEvent()) {
                        event.getEvent().onStop(this, day, event);
                    }
                    // If the event is not connected with vehicles.
                    else {
                       event.getEvent().goOnStop(stopCapacity, day, event);
                    }

                    // Take next event from event queue.
                    event = events.take();
                }
            }

            // After exiting the loop, to end last route of the trams.
            Tram last = event.getEvent().getTram();

            // Check if the tram ended its last loop, if not continue the route.
            if (!last.isBeginning()) {
                event.getEvent().onStop(this, day, event);
            }
            else {
                System.out.println("day " + day + ", " + event);
                stop = last.getCurrentStop();
                numberOfRides += last.canGetOut(stop, day, event.getTime());
            }
        }
    }

    /**
     * Used at th end of the day to make all passengers "go home".
     */
    public void goHome() {
        for (int i = 0; i < numberOfStops; i++) {
            stops[i].endOfDay(this);
        }

        for (int i = 0; i < trams.length; i++) {
            if (trams[i] != null) {
                numberOfRides += trams[i].endOfDay();
            }
            else {
                i = trams.length;
            }
        }
    }

    /**
     * Main controller, creates simulation and manages all methods needed to
     * perform the simulation.
     */
    public static void main(String[] args) {
        int helper = 0;
        int timeHelper = 0;

        Simulation currentSimulation = new Simulation(); // Read simulation data
        // from the input and create simulation.
        currentSimulation.write(); // Write read data.
        currentSimulation.createTrams();
        currentSimulation.initializePassengers();

        // Simulate all days. After each day write statistics for this day.
        for (int i = 0; i < currentSimulation.numberOfDays; i++) {
            currentSimulation.startTrams();
            currentSimulation.goOnStop();
            currentSimulation.simulate(i);
            currentSimulation.goHome();

            int temp = currentSimulation.timeOfWaiting - timeHelper;
            System.out.println("DAILY SUMMARY:\nNumber of rides today: "
                    + (currentSimulation.numberOfRides - helper) );
            System.out.println("They waited " + temp + " minutes, that is " +
                    convertTime(temp) + " hours");

            helper = currentSimulation.numberOfRides;
            timeHelper = currentSimulation.timeOfWaiting;
        }

        helper = currentSimulation.numberOfRides;
        timeHelper = currentSimulation.timeOfWaiting;
        currentSimulation.addHowManyWaited(helper);
        helper = currentSimulation.howManyWaited;

        // Write summary statistics.
        System.out.println("SUMMARY:\nNumber of rides completed by passengers "
                + currentSimulation.numberOfRides);
        if (helper != 0) {
            System.out.println("Average awaiting time was equal to: "
                    + (timeHelper/ helper) + " minutes");
        }
        else {
            System.out.println("There was no passengers on the stops," +
                    " so awaiting time is equal to 0");
        }


    }

}
