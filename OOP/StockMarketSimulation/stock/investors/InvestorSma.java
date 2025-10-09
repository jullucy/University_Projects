package investors;

import market.MarketSystem;

import java.util.ArrayList;


/**
 * Represents an investor that uses the Simple Moving Average (SMA) strategy.
 */
public class InvestorSma extends Investor {
    private ArrayList<Statistics> stockStatistics; // Statistics for every company

    public InvestorSma(int howMany, int type) {
        super(howMany, type);
        stockStatistics = new ArrayList<>();
    }

    public InvestorSma(Investor investor) {
        super(investor);
    }

    /**
     * Makes a decision based on the SMA strategy.
     * @param system the market system
     */
    @Override
    public void makeDecision(MarketSystem system) {
        for (int i = 0; i < system.getSharesLength(); i++) {
            if (i == stockStatistics.size()) {
                stockStatistics.add(new Statistics());
            }

            stockStatistics.get(i).setSma(system.getPrice(i));

            int signal = stockStatistics.get(i).getShouldInvest();

            // Signal not equal to 2 means the actions has to be undertaken.
            if (signal < 2 ) {
                super.decide(system, signal, i); // Depending on the type of
                // signal that was given investor buy or sell shares.
                i = system.getSharesLength();
            }
        }
    }
}