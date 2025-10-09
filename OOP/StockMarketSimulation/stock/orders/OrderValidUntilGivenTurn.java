package orders;

import investors.Investor;
import market.Share;

import java.util.ArrayList;

/**
 * Represents an order which is valid until turn given by the investor.
 */
public class OrderValidUntilGivenTurn extends Order {
    private final int turn; // Turn until which this order is valid

    /**
     * Constructor for creating an OrderValidUntilGivenTurn object.
     * @param investor the investor placing the order.
     * @param typeOfOrder type of order (0 for pursue, 1 for sale).
     * @param stockIdentifier identifier of the stock for the order.
     * @param howManyShares number of shares for the order.
     * @param priceLimit price limit for the order.
     * @param turn turn until which the order is valid.
     */
    public OrderValidUntilGivenTurn(Investor investor, int typeOfOrder, int stockIdentifier, int howManyShares, int priceLimit, int turn) {
        super(investor, typeOfOrder, stockIdentifier, howManyShares, priceLimit, 3); // Call to superclass constructor
        this.turn = turn; // Initialize turn
    }

    public int getTurn() {
        return turn;
    }

    /**
     * Copy constructor for creating a copy of an OrderValidUntilGivenTurn object.
     * @param order The order to be copied.
     */
    public OrderValidUntilGivenTurn(OrderValidUntilGivenTurn order) {
        super(order); // Call to superclass copy constructor
        this.turn = order.getTurn(); // Copy turn
    }

    /**
     * Method to realize a sale order.
     * @param turn current turn in the market simulation.
     * @param saleOrders list of sale orders.
     * @param purchaseOrders list of purchase orders.
     * @param indexP  index of this order in the purchase orders list.
     * @param share the share related to this order.
     */
    @Override
    public void realizeSale(int turn, ArrayList<Order> saleOrders, ArrayList<Order> purchaseOrders, int indexP, Share share) {
        if (turn > this.turn) {
            saleOrders.remove(this); // Remove the order if the turn has passed
        } else if (getHowManyShares() > getInvestor().getNumberOfShares(getStockIdentifier())) {
            saleOrders.remove(this); // Remove the order if the investor does not have enough shares
        } else if (this.getHowManyShares() == 0) {
            saleOrders.remove(this); // Remove the order if it has no more shares to sell
        } else if (!purchaseOrders.isEmpty()){
            // Attempt to pursue the order in the purchase orders list
            purchaseOrders.get(indexP).realizePursue(turn, purchaseOrders, saleOrders, saleOrders.indexOf(this), share);

            if (this.getHowManyShares() == 0) {
                saleOrders.remove(this); // Remove the order if it has no more shares to sell after pursuing
            }
        }
    }

    /**
     * Method to realize a pursue order.
     * @param turn current turn in the market simulation.
     * @param purchaseOrders list of purchase orders.
     * @param saleOrders list of sale orders.
     * @param indexS index of this order in the sale orders list.
     * @param share the share related to this order.
     */
    @Override
    public void realizePursue(int turn, ArrayList<Order> purchaseOrders, ArrayList<Order> saleOrders, int indexS, Share share) {
        helper(turn, purchaseOrders, saleOrders, indexS, share);
    }

    public void helper(int turn, ArrayList<Order> purchaseOrders, ArrayList<Order> saleOrders, int indexS, Share share) {
        if (turn > this.turn) {
            purchaseOrders.remove(this); // Remove the order if the turn has passed
        }
        else if (getPriceLimit() * getHowManyShares() > getInvestor().getCash()) {
            purchaseOrders.remove(this); // Remove the order if the investor does not have enough cash to pursue
        }
        else if (this.getHowManyShares() == 0) {
            purchaseOrders.remove(this); // Remove the order if it has no more shares to pursue
        }
        else if (!saleOrders.isEmpty()){
            // Set the last price of the share to the price limit of the sale order
            share.setLastPrice(saleOrders.get(indexS).getPriceLimit());
            realize(purchaseOrders, saleOrders, indexS);
        }
    }
}