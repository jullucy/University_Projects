import pandas as pd
from data_read import load_drugbank_data

"""
   Extracts target information for all drugs in the dataset.

   Returns:
   - A DataFrame with the following columns:
     - DrugBank ID
     - Target DrugBank ID
     - Source
     - External ID
     - Polypeptide Name
     - Gene Name
     - GenAtlas ID
     - Chromosome
     - Cellular Location
"""
def extract_target_data(drugs, namespace):
    target_rows = []

    for drug in drugs:
        drug_id = None
        for drugbank_id in drug.findall("ns:drugbank-id", namespace):
            if drugbank_id.attrib.get("primary") == "true":
                drug_id = drugbank_id.text
                break
        if not drug_id:
            drug_id = drug.find("ns:drugbank-id", namespace).text

        targets = drug.find("ns:targets", namespace)
        if targets is None:
            continue

        for target in targets.findall("ns:target", namespace):
            target_id = target.findtext("ns:id", default="No information given", namespaces=namespace)
            polypeptide = target.find("ns:polypeptide", namespace)

            if polypeptide is not None:
                source = polypeptide.attrib.get("source", "No information given")
                external_id = polypeptide.attrib.get("id", "No information given")
                polypeptide_name = polypeptide.findtext("ns:name", default="No information given", namespaces=namespace)
                gene_name = polypeptide.findtext("ns:gene-name", default="No information given", namespaces=namespace)
                genatlas_id = "No information given"
                chromosome = polypeptide.findtext("ns:chromosome-location", default="No information given", namespaces=namespace)
                cellular_location = polypeptide.findtext("ns:cellular-location", default="No information given", namespaces=namespace)

                external_identifiers = polypeptide.find("ns:external-identifiers", namespace)
                if external_identifiers is not None:
                    for identifier in external_identifiers.findall("ns:external-identifier", namespace):
                        if identifier.findtext("ns:resource", namespaces=namespace) == "GenAtlas":
                            genatlas_id = identifier.findtext("ns:identifier", namespaces=namespace)
                            break

                target_rows.append({
                    'DrugBank ID': drug_id,
                    'Target DrugBank ID': target_id,
                    'Source': source,
                    'External ID': external_id,
                    'Polypeptide Name': polypeptide_name,
                    'Gene Name': gene_name,
                    'GenAtlas ID': genatlas_id,
                    'Chromosome': chromosome,
                    'Cellular Location': cellular_location,
                })

    target_df = pd.DataFrame(target_rows)
    return target_df

# Main function for usage of this function only
if __name__ == '__main__':
    file_path = "drugbank_partial.xml"

    # Define the namespace for XML parsing
    namespaces = {'ns': 'http://www.drugbank.ca'}

    # Load the DrugBank data
    drugs = load_drugbank_data(file_path, namespaces)

    target_data = extract_target_data(drugs, namespaces)
    target_data.to_csv("targets.csv", index=False)