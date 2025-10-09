package orders;

import investors.Investor;

/**
 * Represents an immediate order, valid only for the current turn.
 */
public class ImmediateOrder extends OrderValidUntilGivenTurn {
    private final int turn;

    /**
     * Constructs an immediate order.
     * @param investor the investor placing the order.
     * @param typeOfOrder the type of order (0 for purchase, 1 for sale).
     * @param stockIdentifier the identifier of the stock.
     * @param howManyShares the number of shares.
     * @param priceLimit the price limit.
     * @param turn the current turn.
     */
    public ImmediateOrder(Investor investor, int typeOfOrder, int stockIdentifier, int howManyShares, int priceLimit, int turn) {
        super(investor, typeOfOrder, stockIdentifier, howManyShares, priceLimit, turn);
        this.setType(0); // Set the order type to immediate
        this.turn = turn;
    }

    /**
     * Gets the turn for this order.
     * @return the turn.
     */
    @Override
    public int getTurn() {
        return turn;
    }

    /**
     * Constructs an immediate order by copying another immediate order.
     * @param order the order to copy.
     */
    public ImmediateOrder(ImmediateOrder order) {
        super(order);
        this.turn = order.getTurn();
    }
}