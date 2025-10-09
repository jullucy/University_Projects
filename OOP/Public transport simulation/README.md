# Public Transport Simulation

A comprehensive event-driven simulation of urban public transportation system implemented in Java. This project simulates the daily operations of a tram network, including passenger management, route scheduling, and real-time event processing.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [System Components](#system-components)
- [Input Format](#input-format)
- [Usage](#usage)
- [Statistics & Analytics](#statistics--analytics)
- [Class Structure](#class-structure)
- [Event System](#event-system)
- [Compilation & Execution](#compilation--execution)

## Overview

This simulation models a realistic public transportation system where:
- **Trams** follow predefined routes with scheduled stops
- **Passengers** generate randomly and travel between stops
- **Events** drive the simulation timeline (arrivals, departures, boarding)
- **Statistics** track system performance and passenger satisfaction

The system uses an **event-driven architecture** to simulate real-world scenarios like rush hours, capacity constraints, and passenger wait times.

## Features

### Core Simulation Features
- **Multi-day simulation** support
- **Real-time event processing** with priority queues
- **Dynamic passenger generation** with random destinations
- **Capacity management** for both trams and stops
- **Route optimization** and scheduling
- **Comprehensive statistics** collection

### Advanced Features
- **Visual representation** of simulation state
- **Time conversion** (minutes to HH:MM format)
- **Statistical analysis** of passenger wait times
- **System performance metrics**
- **Extensible event system** for adding new event types

## Architecture

The simulation follows **Object-Oriented Design** principles with clear separation of concerns:

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Simulation    │────│  Event System   │────│  Public Transport│
│   Controller    │    │                 │    │    Network      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Statistics    │    │   Event Queue   │    │ Trams & Routes  │
│   & Analytics   │    │   & Handlers    │    │  & Passengers   │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

## System Components

### Transportation Network
- **Trams**: Vehicles with capacity constraints and route assignments
- **Tram Lines**: Defined routes connecting multiple stops
- **Stops**: Network nodes with capacity limits and passenger queues
- **Routes**: Paths between stops with timing information

### Passenger System
- **Passenger Generation**: Random passenger creation with home stops
- **Destination Assignment**: Dynamic destination selection
- **Boarding Logic**: Capacity-aware passenger boarding
- **Journey Tracking**: Complete trip monitoring and statistics

### Event Management
- **Event Queue**: Priority-based event scheduling
- **Event Types**:
  - `TramOnStop`: Tram arrival at station
  - `GoOnStop`: Passenger arrival at stop
  - Custom events for system extensions

## Input Format

The simulation accepts structured input in the following order:

```
<number_of_days>
<stop_capacity>
<number_of_stops>
<stop_name_1>
<stop_name_2>
...
<stop_name_n>
<number_of_passengers>
<tram_capacity>
<number_of_tram_lines>

For each tram line:
<number_of_trams_on_line>
<route_length>
For each stop on route:
    <stop_name>
    <travel_time_from_previous_stop>
```

### Example Input:
```
2                    // 2 days simulation
50                   // stop capacity: 50 passengers
4                    // 4 stops total
Central_Station      // stop names
University
Shopping_Mall
Airport
100                  // 100 passengers
30                   // tram capacity: 30 passengers
2                    // 2 tram lines

3                    // Line 1: 3 trams
4                    // 4 stops on route
Central_Station      
00:05               // 5 minutes from start
University
00:10               // 10 minutes from Central_Station
Shopping_Mall
00:08               // 8 minutes from University
Airport
00:15               // 15 minutes from Shopping_Mall

2                    // Line 2: 2 trams
3                    // 3 stops on route
Airport
00:12
Shopping_Mall
00:07
Central_Station
00:10
```

## Usage

### Basic Execution
```bash
# Compile all Java files
javac -cp . publicTransport/*.java events/*.java

# Run simulation with input file
java publicTransport.Simulation < input.txt

# Or run interactively
java publicTransport.Simulation
# Then provide input via standard input
```

### Sample Output
```
08:00: tram number 1 (side number 1) arrives at Central_Station
08:00: passenger 15 gets on tram number 1 (side number 1) at Central_Station
08:05: tram number 1 (side number 1) arrives at University
08:05: passenger 23 gets on tram number 1 (side number 1) at University
...
Statistics:
Total rides: 1,234
Average waiting time: 8.5 minutes
Passengers who waited: 89.2%
```

## Statistics & Analytics

The simulation tracks comprehensive metrics:

### Performance Metrics
- **Total Rides**: Number of completed passenger journeys
- **Average Wait Time**: Mean passenger waiting time at stops
- **Wait Percentage**: Percentage of passengers who had to wait
- **System Utilization**: Tram capacity usage statistics

### Real-time Monitoring
- **Live event logging** with timestamps
- **Passenger journey tracking**
- **Tram location and capacity monitoring**
- **Stop occupancy levels**

## Class Structure

### Package: `publicTransport`
| Class | Purpose |
|-------|---------|
| `Simulation` | Main controller, handles I/O and statistics |
| `Tram` | Vehicle implementation with routing logic |
| `TramLine` | Route definition and management |
| `Passenger` | Individual passenger with journey logic |
| `Stop` | Network node with passenger queues |
| `Route` | Path between stops with timing |
| `Vehicle` | Abstract base class for transportation |
| `Draw` | Visualization utilities |

### Package: `events`
| Class | Purpose |
|-------|---------|
| `Event` | Base event class with timing |
| `EventQueue`/`Queue` | Priority-based event scheduling |
| `TramOnStop` | Tram arrival event handler |
| `GoOnStop` | Passenger arrival event handler |
| `SomeEvent` | Event interface for polymorphism |

## ⚡ Event System

The simulation uses an **event-driven architecture**:

### Event Processing Flow
1. **Event Creation**: Events generated for tram movements, passenger arrivals
2. **Queue Management**: Events sorted by timestamp in priority queue  
3. **Event Execution**: Events processed in chronological order
4. **State Updates**: System state updated based on event outcomes
5. **New Event Generation**: Events may trigger additional events

### Event Types
- **TramOnStop**: Handles tram arrivals, passenger boarding/alighting
- **GoOnStop**: Manages passenger arrivals at stops
- **Custom Events**: Extensible system for new event types

## Compilation & Execution

### Prerequisites
- **Java Development Kit (JDK)** 8 or higher
- **Command line** or **IDE** with Java support

### Build Instructions
```bash
# Navigate to project directory
cd "Public transport simulation"

# Compile all source files
javac -cp . publicTransport/*.java events/*.java

# Verify compilation
ls publicTransport/*.class events/*.class
```

### Running the Simulation
```bash
# Method 1: With input file
java publicTransport.Simulation < simulation_input.txt

# Method 2: Interactive mode
java publicTransport.Simulation
# Then enter input line by line

# Method 3: With input redirection
echo "2 50 3 Stop1 Stop2 Stop3 10 25 1 2 3 Stop1 00:05 Stop2 00:10 Stop3 00:08" | java publicTransport.Simulation
```

## Educational Objectives

This project demonstrates key Computer Science concepts:

### Object-Oriented Programming
- **Inheritance**: Vehicle hierarchy (Tram extends Vehicle)
- **Polymorphism**: Event interface implementations
- **Encapsulation**: Private fields with public methods
- **Abstraction**: Abstract base classes and interfaces

### Algorithms & Data Structures
- **Priority Queues**: Event scheduling and management
- **Graph Algorithms**: Route finding and optimization
- **Simulation Algorithms**: Discrete event simulation

### Software Engineering
- **Modular Design**: Package-based organization
- **Documentation**: Comprehensive JavaDoc comments
- **Testing**: Extensible architecture for unit testing
- **Maintainability**: Clean code structure and naming

---

**Course**: Object-Oriented Programming  
**Institution**: University of Warsaw (MIMUW)  
**Language**: Java  
**Paradigm**: Event-Driven Simulation