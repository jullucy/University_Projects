import pandas as pd
import xml.etree.ElementTree as ET
from data_read import load_drugbank_data


"""
    Extract pathway information for all drugs in the dataset.

    Returns:
    - A DataFrame with the following columns:
      - DrugBank ID
      - Pathway Name
      - Pathway ID
      - Type of Pathway (e.g., signal, metabolic)
    - Total number of unique pathways.
"""
def extract_pathway_data(drugs, namespace):
    pathway_rows = []

    # Iterate through each drug element
    for drug in drugs:
        # Extract the primary DrugBank ID
        drug_id = None

        for drugbank_id in drug.findall("ns:drugbank-id", namespace):
            if drugbank_id.attrib.get("primary") == "true":
                drug_id = drugbank_id.text
                break

        if not drug_id:  # Use the first ID if no primary ID is found
            drug_id = drug.find("ns:drugbank-id", namespace).text

        # Get pathways for the current drug
        pathways = drug.find("ns:pathways", namespace)

        if pathways is None:
            continue

        for pathway in pathways.findall("ns:pathway", namespace):
            pathway_name = pathway.find("ns:name", namespace).text if pathway.find("ns:name", namespace) is not None else None
            pathway_id = pathway.find("ns:smpdb-id", namespace).text if pathway.find("ns:smpdb-id", namespace) is not None else None
            pathway_type = pathway.find("ns:category", namespace).text if pathway.find("ns:category", namespace) is not None else None

            pathway_rows.append({
                "DrugBank ID": drug_id,
                "Pathway Name": pathway_name,
                "Pathway ID": pathway_id,
                "Pathway Type": pathway_type,
            })

    # Convert to DataFrame
    pathway_df = pd.DataFrame(pathway_rows)

    # Calculate the total number of unique pathways
    unique_pathways = pathway_df['Pathway ID'].nunique()

    return pathway_df, unique_pathways

# Main execution for using this function only
if __name__ == "__main__":
    # File path
    file_path = "drugbank_partial.xml"

    # Define the namespace for XML parsing
    namespaces = {'ns': 'http://www.drugbank.ca'}

    # Load the DrugBank data
    drugs = load_drugbank_data(file_path, namespaces)

    pathway_df, unique_pathway_count = extract_pathway_data(drugs, namespaces)

    # Output results
    print(f"Number of unique pathways: {unique_pathway_count}")

    # Save to a CSV file
    pathway_df.to_csv("pathway.csv", index=False)
