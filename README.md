# University Projects

This repository contains a collection of projects and assignments from my Computer Science studies at the **University of Warsaw (MIMUW)**. Each project demonstrates different programming concepts, algorithms, and technologies learned throughout the curriculum.

## Courses & Projects Overview

### Computer Architecture and Operating Systems
- **Division (`Division/`)**
  - Assembly language implementation for division operations
  - Files: `mdiv.asm`

- **NAND Gates Simulation (`NAND_Gates_Simulation/`)**
  - Dynamic library for combinational Boolean circuits using NAND gates
  - Implements circuit creation, connection, and evaluation functions
  - Files: `nand.c`, `makefile`
  - Features: Gate creation/deletion, signal connections, critical path calculation

### Computer Networks
- **P2P Clock Synchronization (`P2P_clock_synchronization/`)**
  - Implementation of peer-to-peer clock synchronization protocols
  - Demonstrates distributed systems concepts and network programming

### Concurrent Programming
- **Computational Verification of Combinatorial Hypothesis (`Computational_Verification_of_a_Combinatorial_Hypothesis/`)**
  - Multi-threaded approach to computational verification problems
  - Includes both non-recursive and parallel implementations
  - Built with CMake for cross-platform compilation
  - Files: `report.pdf`, `main.c` implementations for different approaches

### Introductory Programming
Classic programming exercises covering fundamental concepts:
- **L-system (`L_system.c`)**
  - Implementation of Lindenmayer systems for generating fractal patterns
- **Looping exercises (`looping.c`)**
  - Basic loop structures and control flow
- **Reversi game (`reversi.c`)**
  - Console-based implementation of the classic board game

### Object-Oriented Programming (Java)

#### Public Transport Simulation
Event-driven simulation of urban public transportation system:
- **Core Features:**
  - Tram and passenger management
  - Route planning and scheduling
  - Real-time event processing
  - Statistics and visualization
- **Key Classes:** `Simulation`, `Tram`, `Passenger`, `Route`, `Stop`
- **Event System:** Queue-based event handling for realistic simulation

#### Stock Market Simulation
Comprehensive stock trading simulation with multiple investor strategies:
- **Investor Types:**
  - Random investors
  - SMA (Simple Moving Average) based investors
  - Statistical analysis tools
- **Market Features:**
  - Order book management
  - Share trading mechanics
  - Real-time market simulation
- **Visualization:** Graphical representation of market data and trends

### Python Programming

#### DrugBank Database Analysis
Advanced data processing and analysis of pharmaceutical database:
- **Core Features:**
  - XML data parsing and processing
  - Drug interaction analysis
  - Pathway visualization and statistics
  - RESTful API with FastAPI
  - Comprehensive testing suite
- **Key Modules:**
  - `server.py` - FastAPI web server
  - `main.py` - Main analysis pipeline
  - `interactions.py` - Drug interaction processing
  - `pathways_*.py` - Pathway analysis and visualization
- **API Functionality:** Query drug interactions and pathways via HTTP endpoints

### WWW Applications (Django)

#### Route Management System
Full-stack web application for managing geographical routes:
- **Backend Features:**
  - Django REST API with token authentication
  - Route and route point management
  - User-specific data isolation
  - SQLite database integration
- **Frontend Features:**
  - Interactive route visualization
  - Background image support for different Warsaw districts
  - Responsive design with modern UI
- **API Endpoints:**
  - Route CRUD operations
  - Point management within routes
  - User authentication and authorization
- **Media Support:** Custom backgrounds for different Warsaw neighborhoods

## Technologies Used

- **Languages:** C, Assembly, Java, Python, JavaScript, HTML/CSS
- **Frameworks:** Django, FastAPI, CMake
- **Databases:** SQLite
- **Tools:** Make, Git, REST APIs
- **Concepts:** 
  - Concurrent programming and threading
  - Network programming and protocols
  - Object-oriented design patterns
  - Web development (full-stack)
  - Data analysis and visualization
  - Algorithm implementation and optimization

## Learning Outcomes

These projects demonstrate proficiency in:
- Low-level programming (Assembly, C)
- System programming and concurrent execution
- Object-oriented design and implementation
- Web application development
- Database design and management
- Data processing and analysis
- API design and documentation
- Testing and quality assurance

