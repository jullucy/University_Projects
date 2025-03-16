import pandas as pd
import networkx as nx
import matplotlib.pyplot as plt

from data_read import load_drugbank_data


"""
  Extract pathway information from DrugBank data.

  Returns:
      A DataFrame with columns:
      - Pathway Name
      - Pathway ID
      - Pathway Type
      - Drug Name
      - DrugBank ID
"""
def extract_pathway_interactions(drugs, namespace):
    pathway_rows = []

    for drug in drugs:
        drug_id = None
        for drugbank_id in drug.findall("ns:drugbank-id", namespace):
            if drugbank_id.attrib.get("primary") == "true":
                drug_id = drugbank_id.text
                break
        if not drug_id:
            drug_id = drug.find("ns:drugbank-id", namespace).text

        pathways = drug.find("ns:pathways", namespace)
        if pathways is None:
            continue

        for pathway in pathways.findall("ns:pathway", namespace):
            pathway_name = pathway.findtext("ns:name", namespaces=namespace)
            pathway_id = pathway.findtext("ns:smpdb-id", namespaces=namespace)
            pathway_type = pathway.findtext("ns:category", namespaces=namespace)
            drugs = pathway.find("ns:drugs", namespace)
            if drugs is None:
                continue

            for related_drug in drugs.findall("ns:drug", namespace):
                drug_name = related_drug.findtext("ns:name", namespaces=namespace)
                related_drugbank_id = related_drug.findtext("ns:drugbank-id", namespaces=namespace)

                pathway_rows.append({
                    'Pathway Name': pathway_name,
                    'Pathway ID': pathway_id,
                    'Pathway Type': pathway_type,
                    'Drug Name': drug_name,
                    'DrugBank ID': related_drugbank_id,
                })

    pathway_df = pd.DataFrame(pathway_rows)
    return pathway_df

"""
    Create a bipartite graph from pathway data.

    Returns:
        A NetworkX graph object.
"""
def create_bipartite_graph(pathway_df):
    B = nx.Graph()

    # Add nodes for drugs and pathways
    drugs = pathway_df['DrugBank ID'].dropna().unique()
    pathways = pathway_df['Pathway Name'].dropna().unique()

    B.add_nodes_from(drugs, bipartite=0)  # Drug nodes
    B.add_nodes_from(pathways, bipartite=1)  # Pathway nodes

    # Add edges between drugs and pathways
    for _, row in pathway_df.iterrows():
        if pd.notna(row['DrugBank ID']) and pd.notna(row['Pathway Name']):
            B.add_edge(row['DrugBank ID'], row['Pathway Name'])

    return B

"""
    Plot a bipartite graph with distinct colors for drugs and pathways.
    """
def plot_bipartite_graph(B):
    # Separate nodes by bipartite group
    drug_nodes = {n for n, d in B.nodes(data=True) if d['bipartite'] == 0}
    pathway_nodes = set(B) - drug_nodes

    # Create a layout for the bipartite graph
    pos = nx.drawing.layout.bipartite_layout(B, nodes=drug_nodes)

    # Draw nodes and edges
    plt.figure(figsize=(15, 8))
    nx.draw_networkx_nodes(B, pos, nodelist=drug_nodes, node_color='lightblue', label='Drugs', node_size=3000)
    nx.draw_networkx_nodes(B, pos, nodelist=pathway_nodes, node_color='lightgreen', label='Pathways', node_size=3000)
    nx.draw_networkx_edges(B, pos, edge_color='gray', width=1)
    nx.draw_networkx_labels(B, pos, font_size=10, font_weight='bold')

    plt.title('Drug-Pathway Bipartite Graph', fontsize=20)
    plt.tight_layout()
    plt.show()

# Main for usage of this function only
if __name__ == '__main__':
    file_path = "drugbank_partial.xml"

    # Define the namespace for XML parsing
    namespaces = {'ns': 'http://www.drugbank.ca'}

    # Load the DrugBank data
    drugs = load_drugbank_data(file_path, namespaces)

    pathway_data = extract_pathway_interactions(drugs, namespaces)

    # Save pathway data to a CSV file
    pathway_data.to_csv("pathway_interactions.csv", index=False)

    # Create and plot the bipartite graph
    B = create_bipartite_graph(pathway_data)
    plot_bipartite_graph(B)