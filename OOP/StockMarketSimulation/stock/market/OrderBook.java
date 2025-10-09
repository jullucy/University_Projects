package market;

import orders.Order;

import java.util.ArrayList;
import java.util.Collections;

/**
 * Represents an order book for managing purchase and sale orders of shares.
 */
public class OrderBook {
    private final ArrayList<Order> purchaseOrders; // List to hold purchase orders.
    private final ArrayList<Order> saleOrders; // List to hold sale orders.

    /**
     * Constructor to initialize empty lists for purchase and sale orders.
     */
    public OrderBook() {
        saleOrders = new ArrayList<>();
        purchaseOrders = new ArrayList<>();
    }

    public ArrayList<Order> getPurchaseOrders() {
        return purchaseOrders;
    }

    public ArrayList<Order> getSaleOrders() {
        return saleOrders;
    }

    /**
     * Sorts both purchase and sale orders based on their price limits.
     */
    public void sortOrders() {
        Collections.sort(purchaseOrders); // Sort purchase orders
        Collections.sort(saleOrders); // Sort sale orders
    }

    /**
     * Adds an order to the appropriate list (purchase or sale) based on its type.
     * @param order The order to add.
     */
    public void addOrder(Order order) {
        if (order.getTypeOfOrder() == 0) {
            purchaseOrders.add(order); // Add to purchase orders
        } else {
            saleOrders.add(order); // Add to sale orders
        }
    }

    public void checkIfValid(ArrayList<Order> orders, int turn) {
        for (int i = 0; i < orders.size(); i++) {
            if (orders.get(i).getType() != 1 && orders.get(i).getTurn() < turn) {
                orders.remove(i);
                i--;
            }
        }
    }

    /**
     * Completes orders by matching sale orders with purchase orders.
     * @param turn  Current turn of the simulation.
     * @param share The share for which orders are being completed.
     */
    public void complete(int turn, Share share) {
        int indexSale = 0;
        int indexPurchase = 0;
        boolean notCompleted = true;
        checkIfValid(purchaseOrders, turn);
        checkIfValid(saleOrders, turn);

        while (notCompleted) {
            if (indexSale == saleOrders.size() || indexPurchase == purchaseOrders.size()) {
                notCompleted = false; // Exit loop if no more orders to process.
            }
            else {
                // Realize sale for the current sale order.
                saleOrders.get(indexSale).realizeSale(turn, saleOrders, purchaseOrders, indexPurchase, share);

                if (indexPurchase == purchaseOrders.size() || indexSale == saleOrders.size()) {
                    notCompleted = false; // Exit loop if no more orders to process.
                }
                else if (purchaseOrders.isEmpty() ||
                        saleOrders.get(indexSale).getPriceLimit() > purchaseOrders.get(indexPurchase).getPriceLimit()) {
                    notCompleted = false; // Exit loop if conditions do not match for order completion.
                }
                else if (saleOrders.get(indexSale).getInvestor() == purchaseOrders.get(indexPurchase).getInvestor()) {
                    indexPurchase++; // Move to the next purchase order if investor is the same.
                    if (indexPurchase == purchaseOrders.size()) {
                        notCompleted = false; // Exit loop if no more orders to process.
                    }
                }
            }
        }
    }
}