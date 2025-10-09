package market;

import orders.Order;

/**
 * Represents a share in the market.
 */
public class Share {
    private final String identifier; // Identifier of the share.
    private final int index; // Index of the share.
    private int lastPrice; // Last traded price of the share.
    private final OrderBook orders; // Order book for managing orders related to this share.

    /**
     * Constructor to initialize a Share object.
     * @param identifier is the identifier of the share.
     * @param lastPrice is the last traded price of the share.
     * @param index is the index of the share.
     */
    public Share(String identifier, int lastPrice, int index) {
        this.identifier = identifier;
        this.lastPrice = lastPrice;
        this.index = index;
        this.orders = new OrderBook(); // Initialize the order book for this share.
    }

    public String getIdentifier() {
        return identifier;
    }


    public OrderBook getOrders() {
        return orders;
    }

    public int getIndex() {
        return index;
    }

    public int getLastPrice() {
        return lastPrice;
    }

    public void setLastPrice(int lastPrice) {
        this.lastPrice = lastPrice;
    }

    /**
     * Adds an order related to this share to the order book.
     * @param order The order to be added.
     */
    public void addOrder(Order order) {
        orders.addOrder(order);
    }

    /**
     * Sorts the orders related to this share in the order book.
     */
    public void sort() {
        orders.sortOrders();
    }

    /**
     * Completes orders related to this share in the order book.
     * @param turn The current turn of the simulation.
     */
    public void complete(int turn) {
        orders.sortOrders(); // First, sort the orders.
        orders.complete(turn, this); // Then, complete the orders.
    }
}