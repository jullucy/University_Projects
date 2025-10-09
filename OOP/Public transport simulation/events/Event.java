package events;

/**
 * Event class holds information about type of event which has happened and time
 * of this event.
 */
public class Event {
    private final int time;
    private final SomeEvent event;

    public Event(SomeEvent event, int time) {
        this.time = time;
        this.event = event;
    }

    public int getTime() {
        return time;
    }

    public SomeEvent getEvent() {
        return event;
    }

    @Override
    public String toString() {
        return convertTime(time)+ ": " + event.toString();
    }

    /**
     * Converts time to String.
     * @param time time given in number of minutes.
     * @return String representing time in popular form of: hours:minutes
     */
    public static String convertTime(int time) {
        String answer = time / 60 + ":";

        if (time % 60 < 10) {
            answer += "0" + time % 60;
        }
        else {
            answer += (time % 60);
        }

        return answer;
    }

    public String time() {
        return convertTime(time);
    }
}