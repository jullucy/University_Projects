package investors;

import java.util.LinkedList;

/**
 * Class used by the SMA investors in order to count SMA and make decisions about
 * the market.
 */
public class Statistics {
    private final LinkedList<Integer> sma5;
    private final LinkedList<Integer> sma10;
    private int shouldInvest; // 2 for neutral, 1 for sale, 0 for pursue

    public Statistics() {
        sma5 = new LinkedList<>();
        sma10 = new LinkedList<>();
        shouldInvest = 0;
    }

    public void setSma(int data) {
        if (sma5.size() == 5) {
            sma5.remove();
        }

        sma5.add(data);

        if (sma10.size() == 10) {
            sma10.remove();
            sma10.add(data);
            checkIfShouldInvest();
        }
        else {
            sma10.add(data);
        }

    }

    /**
     * Counts SMA for 5 most recent turns and for 10 most recent turns and
     * decide on actions that investors should undertake.
     */
    private void checkIfShouldInvest() {
        double meanForSma5 = sma5.stream()
                .mapToDouble(d -> d)
                .average()
                .orElse(0.0);
        double meanForSma10 = sma10.stream()
                .mapToDouble(d -> d)
                .average()
                .orElse(0.0);

        // If investor should buy shares.
        if (meanForSma5 > meanForSma10) {
            shouldInvest = 0;
        }
        // If investor should sell shares.
        else if (meanForSma5 < meanForSma10) {
            shouldInvest = 1;
        }
        // If investor shouldn't do anything.
        else {
            shouldInvest = 2;
        }
    }

    public int getShouldInvest() {
        return shouldInvest;
    }

}
