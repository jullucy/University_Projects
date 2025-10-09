package orders;

import investors.Investor;
import investors.InvestorSma;
import investors.RandomInvestor;
import market.Share;

import java.util.ArrayList;

/**
 * Abstract class representing an order to pursue or sell shares.
 */
public abstract class Order implements Comparable<Order> {
    private final int typeOfOrder; // 0 for pursue, 1 for sale
    private final int stockIdentifier;
    private int howManyShares;
    private final int priceLimit;
    private Investor investor;
    private int type;

    /**
     * Constructor for creating a new order.
     * @param investor is the investor placing the order.
     * @param typeOfOrder is the type of order (0 for pursue, 1 for sale).
     * @param stockIdentifier is the identifier of the stock.
     * @param howManyShares is the number of shares in the order.
     * @param priceLimit is the price limit for the order.
     * @param type is the type of the order.
     */
    public Order(Investor investor, int typeOfOrder, int stockIdentifier,
                 int howManyShares, int priceLimit, int type) {
        this.investor = investor;
        this.typeOfOrder = typeOfOrder;
        this.stockIdentifier = stockIdentifier;
        this.howManyShares = howManyShares;
        this.priceLimit = priceLimit;
        this.type = type;
    }

    /**
     * Copy constructor for creating a copy of an existing order.
     * @param order The order to copy.
     */
    public Order(Order order) {
        if (order.getInvestor().getInvestorType() == 0) {
            this.investor = new RandomInvestor(order.getInvestor());
        } else if (order.getInvestor().getInvestorType() == 1) {
            this.investor = new InvestorSma(order.getInvestor());
        }
        this.typeOfOrder = order.getTypeOfOrder();
        this.stockIdentifier = order.getStockIdentifier();
        this.howManyShares = order.getHowManyShares();
        this.priceLimit = order.getPriceLimit();
        this.type = order.getType();
    }

    public int getType() {
        return type;
    }

    public abstract int getTurn();

    public void setType(int type) {
        this.type = type;
    }

    public int getPriceLimit() {
        return priceLimit;
    }

    public int getHowManyShares() {
        return howManyShares;
    }

    public Investor getInvestor() {
        return investor;
    }

    public int getStockIdentifier() {
        return stockIdentifier;
    }

    /**
     * Compares this order with another order based on price limit.
     * @param o The order to compare.
     * @return A negative integer, zero, or a positive integer as
     * this order is less than, equal to, or greater than the specified order.
     */
    @Override
    public int compareTo(Order o) {
        // Sort pursue orders from highest to lowest and sale orders from lowest to highest.
        if (typeOfOrder == 0) {
            return this.getPriceLimit() - o.getPriceLimit();
        }
        return o.getPriceLimit() - this.getPriceLimit();
    }

    /**
     * Abstract method to realize a sale based on the order.
     * @param turn is the current turn.
     * @param saleOrders the list of sale orders.
     * @param purchaseOrders the list of purchase orders.
     * @param indexP the index of the purchase order.
     * @param share the share involved in the transaction.
     */
    public abstract void realizeSale(int turn, ArrayList<Order> saleOrders, ArrayList<Order> purchaseOrders, int indexP, Share share);

    /**
     * Abstract method to realize a pursue (buy) based on the order.
     * @param turn is the current turn.
     * @param purchaseOrders the list of purchase orders.
     * @param saleOrders the list of sale orders.
     * @param indexS index of the sale order.
     * @param share the share involved in the transaction.
     */
    public abstract void realizePursue(int turn, ArrayList<Order> purchaseOrders, ArrayList<Order> saleOrders, int indexS, Share share);

    public int getTypeOfOrder() {
        return typeOfOrder;
    }

    /**
     * Method to execute selling shares based on the order.
     * @param howManyShares is the number of shares to sell.
     * @param price the price per share.
     */
    public void saleShares(int howManyShares, int price) {
        investor.sellShares(stockIdentifier, price, howManyShares);
        this.howManyShares -= howManyShares;
    }

    /**
     * Method to execute buying shares based on the order.
     * @param howManyShares is the number of shares to sell.
     * @param price the price per share.
     */
    public void buyShares(int howManyShares, int price) {
        investor.buyShares(stockIdentifier, price, howManyShares);
        this.howManyShares -= howManyShares;
    }

    /**
     * Helper method, used by other classes in orders package.
     * @param purchaseOrders list of purchase orders.
     * @param saleOrders list of sale orders.
     * @param indexS index of this order in the sale orders list.
     */
    protected void realize(ArrayList<Order> purchaseOrders, ArrayList<Order> saleOrders, int indexS) {
        // Check if the investor of this order is not the same as the investor of the sale order
        if (this.getInvestor() != saleOrders.get(indexS).getInvestor()) {
            // Execute the transaction based on which order has more shares
            if (saleOrders.get(indexS).getHowManyShares() > this.getHowManyShares()) {
                saleOrders.get(indexS).saleShares(this.getHowManyShares(), saleOrders.get(indexS).getPriceLimit());
                this.buyShares(this.getHowManyShares(), saleOrders.get(indexS).getPriceLimit());
            } else {
                this.buyShares(saleOrders.get(indexS).getHowManyShares(), saleOrders.get(indexS).getPriceLimit());
                saleOrders.get(indexS).saleShares(saleOrders.get(indexS).getHowManyShares(), saleOrders.get(indexS).getPriceLimit());
            }
        }

        if (this.getHowManyShares() == 0) {
            // Remove the order if it has no more shares to pursue after transaction.
            purchaseOrders.remove(this);
        }
        if (saleOrders.get(indexS).getHowManyShares() == 0) {
            saleOrders.remove(indexS);
        }
    }
}