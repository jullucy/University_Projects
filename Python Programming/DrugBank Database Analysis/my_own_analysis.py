"""
    This script extracts, queries, and analyzes drug-target data from DrugBank.

    It performs the following operations:
    1. Extracts UniProt IDs for targets associated with a specific DrugBank ID.
    2. Queries the UniProt database for detailed information about each UniProt ID.
    3. Creates a star network graph with the DrugBank ID at the center and taxonomies connected to it.
    4. Saves the extracted data and UniProt details to a CSV file.
    5. Visualizes the network graph using Matplotlib.

    Functions:
    - extract_uniprot_ids: Extracts UniProt IDs for targets of a given DrugBank ID.
    - query_uniprot: Queries the UniProt database for information related to a UniProt ID.
    - create_taxonomy_star_network: Constructs a star network graph with DrugBank ID at the center.
    - plot_taxonomy_star_network: Plots the network graph for taxonomy data.
    - analyse: The main analysis function that ties everything together and handles user input.

    Usage:
    Run the script, provide a DrugBank ID when prompted, and the script will:
    - Extract the targets associated with the DrugBank ID.
    - Query the UniProt database for details about the UniProt IDs.
    - Plot a network graph based on taxonomy data and save the results to a CSV file.

    Dependencies:
    - requests: For querying the UniProt database.
    - pandas: For data manipulation.
    - matplotlib: For plotting the network graph.
    - networkx: For constructing and handling network graphs.
"""

import requests
import pandas as pd
import matplotlib.pyplot as plt
import networkx as nx

from data_read import load_drugbank_data


"""
    Extracts UniProt IDs for the targets of a specific DrugBank ID.

    Parameters:
    - root: The root element of the parsed DrugBank XML.
    - drugbank_id: The specific DrugBank ID to search for.

    Returns:
    - A DataFrame with the following columns:
      - DrugBank ID
      - UniProt ID
"""
def extract_uniprot_ids(drugs, namespace, drugbank_id):
    uniprot_data = []

    for drug in drugs:
        # Find the primary DrugBank ID
        primary_id = None
        drugbank_ids = drug.findall("ns:drugbank-id", namespace)
        for db_id in drugbank_ids:
            if db_id.attrib.get("primary") == "true":
                primary_id = db_id.text
                break

        if not primary_id or primary_id != drugbank_id:
            continue

        # Extract UniProt IDs from targets
        targets = drug.find("ns:targets", namespace)
        if targets is None:
            continue

        for target in targets.findall("ns:target", namespace):
            polypeptide = target.find("ns:polypeptide", namespace)
            if polypeptide is not None:
                uniprot_id = polypeptide.attrib.get("id", "No information given")
                uniprot_data.append({
                    "DrugBank ID": primary_id,
                    "UniProt ID": uniprot_id
                })

    return pd.DataFrame(uniprot_data)


"""
    Queries the UniProt database for a given UniProt ID and extracts relevant fields like description.

    Parameters:
    - uniprot_id: A single UniProt ID string.

    Returns:
    - A dictionary with detailed UniProt information, including:
      - Description
      - Synonyms
      - Organism
      - Taxonomy
"""
def query_uniprot(uniprot_id):
    base_url = f"https://www.uniprot.org/uniprot/{uniprot_id}.txt"
    try:
        response = requests.get(base_url)
        response.raise_for_status()
        uniprot_data = response.text

        # Parse the UniProt data for relevant fields
        description = None
        synonyms = []
        organism = None
        taxonomy = []

        for line in uniprot_data.splitlines():
            if line.startswith("DE   RecName: Full="):
                description = line.split("=")[-1].strip(";")
            elif line.startswith("DE   AltName: Full="):
                synonyms.append(line.split("=")[-1].strip(";"))
            elif line.startswith("OS   "):
                organism = line[5:].strip()
            elif line.startswith("GN   Name="):
                taxonomy.append(line.split("=")[-1].split("{")[0].strip(";"))

        return {
            "Description": description or "No description found",
            "Synonyms": ", ".join(synonyms) if synonyms else "No synonyms found",
            "Organism": organism or "No organism information found",
            "Taxonomy": ", ".join(taxonomy) if taxonomy else "No taxonomy information found"
        }

    except requests.exceptions.RequestException as e:
        print(f"Error querying UniProt for {uniprot_id}: {e}")
        return {
            "Description": "Error retrieving data",
            "Synonyms": "Error retrieving data",
            "Organism": "Error retrieving data",
            "Taxonomy": "Error retrieving data"
        }


"""
    Creates a star network graph with the DrugBank ID at the center and taxonomies connected to it.

    Parameters:
    - drugbank_id: The specific DrugBank ID to be the central node in the network.
    - taxonomy_data: A list of taxonomies associated with the drug.

    Returns:
    - A NetworkX graph representing the star network.
"""
def create_taxonomy_star_network(drugbank_id, taxonomy_data):
    G = nx.Graph()

    # Add the DrugBank ID as the central node
    G.add_node(drugbank_id)

    # Add taxonomy nodes and connect them to the DrugBank ID
    for taxonomies in taxonomy_data:
        taxonomy_list = taxonomies.split(", ") if isinstance(taxonomies, str) else []

        for taxonomy in taxonomy_list:
            G.add_node(taxonomy)
            G.add_edge(drugbank_id, taxonomy)  # Connect the DrugBank ID to each taxonomy

    return G


"""
    Plots the star-like network graph with DrugBank ID at the center.

    Parameters:
    - G: A NetworkX graph object to plot.
"""
def plot_taxonomy_star_network(G):
    plt.figure(figsize=(12, 12))
    pos = nx.spring_layout(G, seed=42)  # Layout for better visual appearance
    nx.draw(G, pos, with_labels=True, node_size=5000, node_color='skyblue', font_size=10, font_weight='bold',
            edge_color='gray')
    plt.title("Taxonomy Network of Targets (Star Layout)")
    plt.show()

"""
    Main function to drive the analysis of DrugBank data, extract UniProt IDs, query for their details,
    and plot the network of taxonomies.

    Parameters:
    - drugs: A list of parsed drug entries from DrugBank.
    - namespaces: A dictionary containing the XML namespaces for parsing.

    Returns:
    - None (prints results and generates output files and plots).
"""
def analyse(drugs, namespaces):
    # Input: DrugBank ID to search
    drugbank_id = input("Enter the DrugBank ID to search for: ")

    # Extract UniProt data for the specified DrugBank ID
    uniprot_data = extract_uniprot_ids(drugs, namespaces, drugbank_id)

    if uniprot_data.empty:
        print(f"No targets or UniProt IDs found for DrugBank ID: {drugbank_id}")
        return

    # Display results
    print("\nExtracted Data:")
    print(uniprot_data)

    # Optional: Query UniProt for detailed information
    uniprot_details = uniprot_data['UniProt ID'].apply(query_uniprot)

    # Convert the list of dictionaries into a DataFrame and join it with the original DataFrame
    uniprot_details_df = pd.DataFrame(uniprot_details.tolist())
    final_data = pd.concat([uniprot_data, uniprot_details_df], axis=1)

    print("\nFinal Data with UniProt Details:")
    print(final_data)

    # Save results to CSV
    output_file = f"{drugbank_id}_targets_uniprot.csv"
    final_data.to_csv(output_file, index=False)
    print(f"\nResults saved to {output_file}")

    # Create the taxonomy star network
    taxonomy_data = final_data['Taxonomy'].dropna()
    G = create_taxonomy_star_network(drugbank_id, taxonomy_data)

    # Plot the star network graph
    plot_taxonomy_star_network(G)


# Main function to load data, search, and display results
if __name__ == "__main__":
    file_path = "drugbank_partial.xml"

    # Define the namespace for XML parsing
    namespaces = {'ns': 'http://www.drugbank.ca'}

    # Load the DrugBank data
    drugs = load_drugbank_data(file_path, namespaces)

    analyse(drugs, namespaces)
