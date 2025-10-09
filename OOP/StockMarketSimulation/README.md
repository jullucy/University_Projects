# Stock Market Simulation

A comprehensive object-oriented simulation of stock market trading implemented in Java. This project models realistic market dynamics with multiple investor strategies, order book management, and real-time trading simulation.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [System Components](#system-components)
- [Investor Strategies](#investor-strategies)
- [Order Types](#order-types)
- [Input Format](#input-format)
- [Usage](#usage)
- [Market Mechanics](#market-mechanics)
- [Class Structure](#class-structure)
- [Statistics & Analytics](#statistics--analytics)
- [Compilation & Execution](#compilation--execution)

## Overview

This simulation creates a realistic stock market environment where:
- **Investors** execute trading strategies based on market analysis
- **Orders** are processed through a sophisticated order book system
- **Shares** fluctuate in price based on supply and demand
- **Market mechanics** handle order matching and transaction processing

The system demonstrates advanced object-oriented programming concepts while providing insights into financial market behavior and algorithmic trading strategies.

## Features

### Core Trading Features
- **Multi-turn simulation** with configurable duration
- **Order book management** with priority-based matching
- **Multiple investor types** with distinct trading strategies
- **Real-time price discovery** through market transactions
- **Comprehensive order types** with various execution rules
- **Portfolio management** for each investor

### Advanced Market Features
- **Simple Moving Average (SMA)** strategy implementation
- **Random trading** strategy for baseline comparison
- **Order lifecycle management** with expiration handling
- **Market statistics** and performance tracking
- **Configurable market parameters** via input files

## Architecture

The simulation follows **Object-Oriented Design** principles with modular components:

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Market System  │────│  Order Book     │────│   Investors     │
│   Controller    │    │   Management    │    │   & Strategies  │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Share Price   │    │   Order Types   │    │   Statistics    │
│   Management    │    │   & Matching    │    │   & Analytics   │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

## System Components

### Market Infrastructure
- **MarketSystem**: Central controller managing all market operations
- **Share**: Individual stock representation with price tracking
- **OrderBook**: Order matching engine with priority queues
- **Turn-based Execution**: Discrete time simulation model

### Trading Participants
- **Investor**: Abstract base class for all market participants
- **RandomInvestor**: Implements random trading strategy
- **InvestorSma**: Uses Simple Moving Average for decision making
- **Portfolio Management**: Cash and share holdings tracking

### Order Management
- **Order**: Abstract base class for all order types
- **Order Types**: Various execution and expiration rules
- **Order Matching**: Price-time priority matching algorithm
- **Transaction Processing**: Automated trade execution

## Investor Strategies

### Random Investor
- **Strategy**: Makes random buy/sell decisions
- **Purpose**: Provides baseline market behavior
- **Implementation**: Random company and order type selection
- **Characteristics**: No market analysis, pure randomness

### SMA Investor (Simple Moving Average)
- **Strategy**: Technical analysis using moving averages
- **Analysis**: Tracks price trends over time windows
- **Decision Logic**: Buy when price above MA, sell when below
- **Statistics**: Maintains historical price data per stock

### Extensible Framework
- **Abstract Design**: Easy to add new investor strategies
- **Strategy Pattern**: Polymorphic decision making
- **Configuration**: Investor types specified in input files

## Order Types

### Complete or Cancel Order
- **Execution**: Either fills completely or cancels entirely
- **Use Case**: Large orders requiring full execution
- **Risk Management**: Prevents partial fills

### Immediate Order
- **Execution**: Executes immediately at market price
- **Priority**: Highest execution priority
- **Market Impact**: Provides immediate liquidity

### Order Valid Until Given Turn
- **Duration**: Active until specified turn number
- **Expiration**: Automatic cancellation after deadline
- **Planning**: Allows strategic order timing

### Order Without Deadline
- **Duration**: Remains active until filled or manually cancelled
- **Persistence**: Long-term market positions
- **Flexibility**: No time constraints

## Input Format

The simulation accepts structured configuration files:

```
# Configuration file format
<investor_types_line>
<share_identifiers_and_prices>
<initial_cash_amounts>

# Investor types: R for Random, S for SMA
# Example: R R S R S (5 investors: 3 Random, 2 SMA)

# Share format: <identifier>:<initial_price>
# Example: AAPL:150 GOOGL:2800 MSFT:300

# Cash amounts for each investor
# Example: 10000 15000 20000 12000 18000
```

### Example Input File:
```
# Market configuration
R R S R S
AAPL:150 GOOGL:2800 MSFT:300 TSLA:800
10000 15000 20000 12000 18000
```

## Usage

### Basic Execution
```bash
# Compile all Java files
javac -cp . stock/*/*.java

# Run simulation with configuration file
java market.MarketSystem config.txt 100

# Arguments:
# - config.txt: Market configuration file
# - 100: Number of simulation turns
```

### Sample Output
```
Turn 1:
Investor 1 (Random) places BUY order: 10 shares of AAPL at $152
Investor 3 (SMA) places SELL order: 5 shares of GOOGL at $2795
Order matched: AAPL trade at $151, volume: 10

Turn 2:
Current prices: AAPL:$151 GOOGL:$2795 MSFT:$301 TSLA:$805
Investor 2 (Random) places BUY order: 7 shares of MSFT at $302
...

Final Statistics:
Total trades executed: 1,247
Average trade size: 12.3 shares
Market volatility: 2.8%
Most active stock: AAPL (345 trades)
```

## Market Mechanics

### Price Discovery
- **Order Book**: Maintains bid-ask spreads for each stock
- **Market Orders**: Execute at best available prices
- **Limit Orders**: Execute only at specified prices or better
- **Price Impact**: Large orders may move market prices

### Order Matching Algorithm
1. **Price Priority**: Higher bid/lower ask prices matched first
2. **Time Priority**: Earlier orders matched before later ones
3. **Order Type Priority**: Immediate orders processed first
4. **Partial Fills**: Large orders may execute in multiple parts

### Turn-Based Simulation
1. **Investor Decisions**: Each investor makes trading decisions
2. **Order Placement**: New orders added to order book
3. **Order Matching**: Market processes all compatible orders
4. **Order Expiration**: Expired orders removed from book
5. **Statistics Update**: Market data and investor portfolios updated

## Class Structure

### Package: `market`
| Class | Purpose |
|-------|---------|
| `MarketSystem` | Central market controller and simulation engine |
| `Share` | Individual stock with price and trading history |
| `OrderBook` | Order matching and market mechanics |

### Package: `investors`
| Class | Purpose |
|-------|---------|
| `Investor` | Abstract base class for all market participants |
| `RandomInvestor` | Random trading strategy implementation |
| `InvestorSma` | Simple Moving Average strategy implementation |
| `Statistics` | Statistical analysis tools for SMA strategy |
| `Draw` | Utility class for random number generation |

### Package: `orders`
| Class | Purpose |
|-------|---------|
| `Order` | Abstract base class for all order types |
| `CompleteOrCancelOrder` | All-or-nothing order execution |
| `ImmediateOrder` | Market order with immediate execution |
| `OrderValidUntilGivenTurn` | Time-limited order validity |
| `OrderWithoutDeadline` | Persistent order until filled |

### Package: `tests`
| Class | Purpose |
|-------|---------|
| Test classes | Unit tests for market components |

## Statistics & Analytics

### Market Metrics
- **Trading Volume**: Total shares traded per turn
- **Price Volatility**: Statistical measure of price movements
- **Bid-Ask Spreads**: Market liquidity indicators
- **Order Fill Rates**: Execution efficiency metrics

### Investor Performance
- **Portfolio Value**: Real-time valuation of holdings
- **Return on Investment**: Performance relative to initial capital
- **Trading Frequency**: Activity levels by investor type
- **Strategy Effectiveness**: Comparative analysis of approaches

### System Analytics
- **Order Book Depth**: Available liquidity at each price level
- **Market Efficiency**: Price discovery effectiveness
- **Strategy Comparison**: Random vs. SMA performance analysis

## Compilation & Execution

### Prerequisites
- **Java Development Kit (JDK)** 8 or higher
- **Command line** or **IDE** with Java support
- **Configuration file** with market parameters

### Build Instructions
```bash
# Navigate to project directory
cd StockMarketSimulation

# Compile all source files
javac -cp . stock/*/*.java

# Verify compilation
find . -name "*.class" | wc -l
```

### Running the Simulation
```bash
# Method 1: With configuration file
java market.MarketSystem market_config.txt 1000

# Method 2: Custom parameters
java market.MarketSystem custom_setup.txt 500

# Method 3: Debug mode with verbose output
java -Xms512m -Xmx1024m market.MarketSystem config.txt 2000
```

### Configuration File Creation
```bash
# Create sample configuration
cat > sample_config.txt << EOF
R R S R S S R
AAPL:150 GOOGL:2800 MSFT:300 AMZN:3200 TSLA:800
10000 15000 20000 25000 12000 18000 22000
EOF
```

## Educational Objectives

This project demonstrates key Computer Science and Financial Engineering concepts:

### Object-Oriented Programming
- **Inheritance**: Investor hierarchy with strategy specialization
- **Polymorphism**: Multiple order types and investor strategies
- **Encapsulation**: Protected market state and investor portfolios
- **Abstraction**: Abstract classes for extensible design

### Algorithms & Data Structures
- **Priority Queues**: Order book management and matching
- **Time Series Analysis**: Moving average calculations
- **Sorting Algorithms**: Order prioritization and matching
- **Statistical Analysis**: Performance metrics calculation

### Financial Engineering
- **Market Microstructure**: Order book dynamics and price discovery
- **Algorithmic Trading**: Automated strategy implementation
- **Risk Management**: Portfolio optimization and order sizing
- **Market Efficiency**: Price formation and arbitrage opportunities

### Software Engineering
- **Modular Design**: Package-based architecture
- **Configuration Management**: External parameter files
- **Testing Framework**: Comprehensive unit test coverage
- **Performance Optimization**: Efficient order matching algorithms

---

**Course**: Object-Oriented Programming  
**Institution**: University of Warsaw (MIMUW)  
**Language**: Java  
**Domain**: Financial Engineering & Algorithmic Trading
