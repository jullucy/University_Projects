package orders;

import investors.Investor;
import market.Share;

import java.util.ArrayList;

public class CompleteOrCancelOrder extends OrderValidUntilGivenTurn {
    private final int turn;

    public CompleteOrCancelOrder(Investor investor, int typeOfOrder, int stockIdentifier, int howManyShares, int priceLimit, int turn) {
        super(investor, typeOfOrder, stockIdentifier, howManyShares, priceLimit, turn);
        this.setType(2);
        this.turn = turn;
    }

    public CompleteOrCancelOrder(CompleteOrCancelOrder completeOrCancelOrder) {
        super(completeOrCancelOrder);
        this.turn = completeOrCancelOrder.getTurn();
    }

    public int getTurn() {
        return turn;
    }

    @Override
    public void realizeSale(int turn, ArrayList<Order> saleOrders, ArrayList<Order> purchaseOrders, int indexP, Share share) {
        ArrayList<Order> purchaseCopy = new ArrayList<>();
        CompleteOrCancelOrder saleCopy = new CompleteOrCancelOrder(this);

        for (Order purchaseOrder : purchaseOrders) {
            Order ord;
            if (purchaseOrder.getType() == 0) {
                ord = new ImmediateOrder((ImmediateOrder) purchaseOrder);
            } else if (purchaseOrder.getType() == 1) {
                ord = new OrderWithoutDeadline(purchaseOrder);
            } else if (purchaseOrder.getType() == 2) {
                ord = new CompleteOrCancelOrder((CompleteOrCancelOrder) purchaseOrder);
            } else {
                ord = new OrderValidUntilGivenTurn((OrderValidUntilGivenTurn) purchaseOrder);
            }
            purchaseCopy.add(ord);
        }

        if (turn != this.turn) {
            saleOrders.remove(this);
        }
        else if (getHowManyShares() > getInvestor().getNumberOfShares(getStockIdentifier())) {
            saleOrders.remove(this);
        }
        else if (this.getHowManyShares() == 0) {
            saleOrders.remove(this);
        }
        else {
            while (this.getHowManyShares() != 0) {
                purchaseOrders.get(indexP).realizePursue(turn, purchaseOrders, saleOrders, saleOrders.indexOf(this), share);
                indexP++;

                if (this.getHowManyShares() != 0 && purchaseOrders.size() <= indexP) {
                    saleOrders.set(saleOrders.indexOf(this), saleCopy);
                    purchaseOrders = purchaseCopy;
                    return;
                }
            }
            saleOrders.remove(this);
        }
    }

    @Override
    public void realizePursue(int turn, ArrayList<Order> purchaseOrders, ArrayList<Order> saleOrders, int indexS, Share share) {
        ArrayList<Order> saleCopy = new ArrayList<>();
        CompleteOrCancelOrder purchaseCopy = new CompleteOrCancelOrder(this);

        for (Order saleOrder : saleOrders) {
            Order ord;
            if (saleOrder.getType() == 0) {
                ord = new ImmediateOrder((ImmediateOrder) saleOrder);
            } else if (saleOrder.getType() == 1) {
                ord = new OrderWithoutDeadline(saleOrder);
            } else if (saleOrder.getType() == 2) {
                ord = new CompleteOrCancelOrder((CompleteOrCancelOrder) saleOrder);
            } else {
                ord = new OrderValidUntilGivenTurn((OrderValidUntilGivenTurn) saleOrder);
            }
            saleCopy.add(ord);
        }

        if (turn != this.turn) {
            purchaseOrders.remove(this);
        }
        else if (getPriceLimit() * getHowManyShares() > getInvestor().getCash()) {
            purchaseOrders.remove(this);
        }
        else if (this.getHowManyShares() == 0) {
            purchaseOrders.remove(this);
        }
        else {
            share.setLastPrice(saleOrders.get(indexS).getPriceLimit());

            if (saleOrders.get(indexS).getHowManyShares() > this.getHowManyShares()) {
                saleOrders.get(indexS).saleShares(this.getHowManyShares(), saleOrders.get(indexS).getPriceLimit());
                this.buyShares(this.getHowManyShares(), saleOrders.get(indexS).getPriceLimit());
            }
            else {
                if (this.getInvestor() != saleOrders.get(indexS).getInvestor()) {
                    this.buyShares(saleOrders.get(indexS).getHowManyShares(), saleOrders.get(indexS).getPriceLimit());
                    saleOrders.get(indexS).saleShares(saleOrders.get(indexS).getHowManyShares(), saleOrders.get(indexS).getPriceLimit());
                }
            }

            if (this.getHowManyShares() != 0 && purchaseOrders.size() <= indexS) {
                purchaseOrders.set(purchaseOrders.indexOf(this), purchaseCopy);
                saleOrders = saleCopy;
                return;
            }

            if (this.getHowManyShares() == 0) {
                purchaseOrders.remove(this);
            }
        }
    }
}