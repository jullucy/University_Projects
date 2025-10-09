# DrugBank Database Analysis

A comprehensive Python application for analyzing pharmaceutical data from the DrugBank database. This project provides XML data processing, statistical analysis, visualization tools, and a RESTful API for querying drug information and interactions.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Data Processing Pipeline](#data-processing-pipeline)
- [API Endpoints](#api-endpoints)
- [Visualization Tools](#visualization-tools)
- [Installation](#installation)
- [Usage](#usage)
- [Module Documentation](#module-documentation)
- [Testing](#testing)
- [Data Sources](#data-sources)

## Overview

This project processes and analyzes DrugBank XML data to extract meaningful insights about:
- **Drug properties** and classification
- **Pathway interactions** and biological processes
- **Target proteins** and cellular locations
- **Gene-drug relationships** and networks
- **Drug interactions** and safety profiles

The system combines data processing, statistical analysis, and web API functionality to provide a complete pharmaceutical informatics solution.

## Features

### Core Data Processing
- **XML parsing** of large DrugBank datasets
- **Data extraction** into structured CSV formats
- **Multi-module architecture** for different data types
- **Efficient memory management** for large datasets

### Analysis Capabilities
- **Drug status analysis** with statistical summaries
- **Pathway interaction mapping** and counting
- **Target protein cellular location analysis**
- **Gene-drug relationship networks**
- **Synonym and product information extraction**

### Visualization Tools
- **Interactive network graphs** using NetworkX and Matplotlib
- **Pie charts** for drug status distribution
- **Histograms** for pathway analysis
- **Bipartite graphs** for drug-pathway relationships
- **Star networks** for taxonomy relationships

### Web API
- **FastAPI-powered** RESTful endpoints
- **Drug pathway queries** by DrugBank ID
- **JSON response format** for easy integration
- **Interactive API documentation** via Swagger UI

## Architecture

The system follows a modular design with clear separation of concerns:

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Data Layer    │────│ Processing Layer│────│  Analysis Layer │
│  (XML Parser)   │    │  (Extractors)   │    │ (Statistics &   │
└─────────────────┘    └─────────────────┘    │  Visualization) │
         │                       │             └─────────────────┘
         ▼                       ▼                       │
┌─────────────────┐    ┌─────────────────┐              ▼
│   File I/O      │    │   CSV Export    │    ┌─────────────────┐
│   Management    │    │   & Storage     │    │   Web API       │
└─────────────────┘    └─────────────────┘    │  (FastAPI)      │
                                              └─────────────────┘
```

## Data Processing Pipeline

### 1. Data Loading (`data_read.py`)
- Loads DrugBank XML files with proper namespace handling
- Extracts drug elements for further processing
- Manages memory efficiently for large datasets

### 2. Primary Data Extraction (`file_read.py`)
- Extracts basic drug information (ID, name, type, groups)
- Creates structured DataFrames for analysis
- Exports to CSV for persistent storage

### 3. Specialized Extractors
- **Synonyms** (`synonyms_searcher.py`): Alternative drug names
- **Products** (`products.py`): Commercial drug products
- **Pathways** (`pathways.py`): Biological pathway data
- **Targets** (`targets.py`): Protein target information
- **Interactions** (`interactions.py`): Drug-drug interactions
- **Genes** (`genes_interactions.py`): Gene-drug relationships

## API Endpoints

### Base URL: `http://127.0.0.1:8000`

#### GET /
**Description**: Welcome message and API status check
**Response**: 
```json
{
  "message": "Welcome to the DrugBank API"
}
```

#### POST /drug-pathways
**Description**: Query pathway interactions for a specific drug
**Request Body**:
```json
{
  "drug_id": "DB00001"
}
```
**Response**:
```json
{
  "drug_id": "DB00001",
  "pathway_count": 15,
  "pathways": ["pathway1", "pathway2", ...]
}
```

### API Documentation
Interactive API documentation available at: `http://127.0.0.1:8000/docs`

## Visualization Tools

### Network Graphs
- **Drug-Pathway Networks**: Bipartite graphs showing drug-pathway relationships
- **Gene-Drug Networks**: Star networks centered on specific genes
- **Taxonomy Networks**: Hierarchical visualization of drug classifications

### Statistical Charts
- **Drug Status Distribution**: Pie charts showing approved vs experimental drugs
- **Pathway Histograms**: Distribution of pathway interactions
- **Cellular Location Charts**: Target protein location analysis

### Graph Examples
```python
# Generate synonym graph for a specific drug
draw_synonym_graph("DB00001", synonym_df)

# Create pathway interaction network
create_bipartite_graph(pathway_df)

# Plot cellular locations
plot_cellular_locations("targets.csv")
```

## Installation

### Prerequisites
- Python 3.8 or higher
- pip package manager

### Dependencies
```bash
pip install fastapi uvicorn pandas matplotlib networkx requests xml
```

### Project Setup
```bash
# Clone or download the project
cd "DrugBank Database Analysis"

# Install required packages
pip install -r requirements.txt

# Ensure DrugBank XML file is present
# Download drugbank_partial.xml from DrugBank website
```

## Usage

### Running the Complete Analysis
```bash
# Execute main analysis pipeline
python main.py

# This will:
# 1. Parse the XML data
# 2. Extract all data types
# 3. Generate visualizations
# 4. Save results to CSV files
```

### Starting the API Server
```bash
# Start FastAPI server
uvicorn server:app --reload

# Access API documentation
# Open http://127.0.0.1:8000/docs in browser
```

### Running Individual Modules
```bash
# Extract specific data types
python file_read.py          # Primary drug data
python synonyms_searcher.py  # Drug synonyms
python pathways_plot.py      # Pathway networks
python targets_plot.py       # Target analysis
```

### Testing
```bash
# Run comprehensive tests
python test_drug.py

# Test with generated data
python test_for_generated_base.py
```

## Module Documentation

### Core Modules

| Module | Purpose | Key Functions |
|--------|---------|---------------|
| `data_read.py` | XML parsing and data loading | `load_drugbank_data()` |
| `file_read.py` | Primary drug data extraction | `extract_primary_drug_data()` |
| `server.py` | FastAPI web server | `count_drug_interactions()` |
| `main.py` | Main execution pipeline | Orchestrates all modules |

### Data Extraction Modules

| Module | Data Type | Output |
|--------|-----------|--------|
| `synonyms_searcher.py` | Drug synonyms | `synonyms.csv` |
| `products.py` | Commercial products | `products.csv` |
| `pathways.py` | Biological pathways | `pathways.csv` |
| `targets.py` | Protein targets | `targets.csv` |
| `interactions.py` | Drug interactions | `interactions.csv` |
| `drug_info.py` | Drug status/classification | Statistical summaries |
| `genes_interactions.py` | Gene-drug relationships | Network graphs |
| `my_own_analysis.py` | Custom taxonomy analysis | UniProt integration |

### Visualization Modules

| Module | Visualization Type | Purpose |
|--------|--------------------|---------|
| `pathways_plot.py` | Bipartite graphs | Drug-pathway networks |
| `pathways_histogram.py` | Histograms | Pathway distribution |
| `targets_plot.py` | Pie charts | Cellular location analysis |
| `genes_interactions.py` | Network graphs | Gene-drug relationships |

## Testing

### Test Coverage
- **Unit Tests** (`test_drug.py`): Core functionality validation
- **Integration Tests** (`test_for_generated_base.py`): Full pipeline testing
- **Data Validation**: XML parsing and extraction verification
- **API Testing**: Endpoint functionality and response validation

### Running Tests
```bash
# Run all tests
python -m pytest test_drug.py -v

# Test with sample data
python test_for_generated_base.py

# API endpoint testing
curl -X POST "http://127.0.0.1:8000/drug-pathways" \
     -H "Content-Type: application/json" \
     -d '{"drug_id": "DB00001"}'
```

## Data Sources

### Input Files
- **Primary**: `drugbank_partial.xml` - DrugBank database export
- **Generated**: `drugbank_partial_and_generated.xml` - Enhanced dataset
- **Configuration**: Various CSV files for analysis parameters

### Output Files
- **drug_data_summary.csv**: Primary drug information
- **synonyms.csv**: Alternative drug names
- **products.csv**: Commercial drug products  
- **pathways.csv**: Biological pathway data
- **targets.csv**: Protein target information
- **interactions.csv**: Drug interaction data

### External APIs
- **UniProt API**: Protein information and taxonomy data
- **DrugBank API**: Additional drug information (if available)

## Performance Considerations

### Memory Optimization
- Streaming XML parsing for large files
- Efficient DataFrame operations
- Garbage collection for large datasets

### Processing Speed
- Vectorized operations using pandas
- Optimized XML traversal
- Caching for repeated queries

## Educational Objectives

This project demonstrates key concepts in:

### Data Science & Analytics
- **XML Processing**: Large-scale data parsing and extraction
- **Data Pipeline Design**: ETL processes and workflow management
- **Statistical Analysis**: Descriptive statistics and data summarization
- **Data Visualization**: Network analysis and statistical plotting

### Software Engineering
- **Modular Architecture**: Separation of concerns and reusable components
- **API Development**: RESTful services with FastAPI
- **Testing Strategy**: Unit and integration testing approaches
- **Documentation**: Comprehensive code and API documentation

### Bioinformatics
- **Pharmaceutical Databases**: DrugBank structure and content
- **Drug Discovery**: Pathway analysis and target identification
- **Network Biology**: Gene-drug interaction networks
- **Regulatory Science**: Drug approval status and classification

---

**Course**: Python Programming  
**Institution**: University of Warsaw (MIMUW)  
**Author**: Julia Łucyn  
**Language**: Python 3.8+  
**Domain**: Bioinformatics & Pharmaceutical Data Analysis