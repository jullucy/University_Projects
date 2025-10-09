package investors;

import market.MarketSystem;

import static investors.Draw.draw;

/**
 * Represents an investor that uses the random strategy.
 */
public class RandomInvestor extends Investor {
    public RandomInvestor(int howMany, int type) {
        super(howMany, type);
    }

    public RandomInvestor(Investor investor) {
        super(investor);
    }

    @Override
    public void makeDecision(MarketSystem system) {
        // Draw the company anf type of order (sale or purchase).
        int index = draw(0, system.getSharesLength() - 1);
        int typeOfOrder = draw(0, 1);
        super.decide(system, typeOfOrder, index);
    }
}
