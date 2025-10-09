package orders;

import investors.Investor;
import market.Share;

import java.util.ArrayList;

/**
 * Represents an order without a deadline.
 */
public class OrderWithoutDeadline extends Order {

    /**
     * Constructs a new OrderWithoutDeadline.
     * @param investor the investor who placed the order.
     * @param typeOfOrder the type of order (0 for purchase, 1 for sale).
     * @param stockIdentifier the identifier of the stock.
     * @param howManyShares the number of shares.
     * @param priceLimit the price limit.
     */
    public OrderWithoutDeadline(Investor investor, int typeOfOrder, int stockIdentifier, int howManyShares, int priceLimit) {
        super(investor, typeOfOrder, stockIdentifier, howManyShares, priceLimit, 1);
    }

    /**
     * Constructs a new OrderWithoutDeadline by copying an existing order.
     * @param order the order to copy.
     */
    public OrderWithoutDeadline(Order order) {
        super(order);
    }

    /**
     * Gets the turn for this order, which is always Integer.MAX_VALUE as it has no deadline.
     * @return the turn, always Integer.MAX_VALUE.
     */
    public int getTurn() {
        return Integer.MAX_VALUE;
    }

    /**
     * Realizes a sale for this order.
     * @param turn the current turn.
     * @param saleOrders the list of sale orders.
     * @param purchaseOrders the list of purchase orders.
     * @param indexP the index of the purchase order.
     * @param share the share involved in the transaction.
     */
    @Override
    public void realizeSale(int turn, ArrayList<Order> saleOrders, ArrayList<Order> purchaseOrders, int indexP, Share share) {
        if (getHowManyShares() > getInvestor().getNumberOfShares(getStockIdentifier())) {
            saleOrders.remove(this);
        }
        else if (this.getHowManyShares() == 0) {
            saleOrders.remove(this);
        }
        else {
            purchaseOrders.get(indexP).realizePursue(turn, purchaseOrders, saleOrders, saleOrders.indexOf(this), share);

            if (this.getHowManyShares() == 0) {
                saleOrders.remove(this);
            }
        }
    }

    /**
     * Realizes a purchase for this order.
     * @param turn the current turn.
     * @param purchaseOrders the list of purchase orders.
     * @param saleOrders the list of sale orders.
     * @param indexS the index of the sale order.
     * @param share the share involved in the transaction.
     */
    @Override
    public void realizePursue(int turn, ArrayList<Order> purchaseOrders, ArrayList<Order> saleOrders, int indexS, Share share) {
        if (getPriceLimit() * getHowManyShares() > getInvestor().getCash()) {
            purchaseOrders.remove(this);
        }
        else if (this.getHowManyShares() == 0) {
            purchaseOrders.remove(this);
        }
        else {
            share.setLastPrice(saleOrders.get(indexS).getPriceLimit());
            realize(purchaseOrders, saleOrders, indexS);
        }
    }
}
