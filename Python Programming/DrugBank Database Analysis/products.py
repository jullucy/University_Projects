import pandas as pd
from data_read import load_drugbank_data

"""
    Extract pharmaceutical product data from the DrugBank XML tree.

    Returns:
        A DataFrame with columns:
        - DrugBank ID
        - Product Name
        - Labeller
        - National Drug Code (NDC)
        - Dosage Form
        - Route of Administration
        - Strength
        - Country
        - Regulatory Agency
    """
def extract_product_data(drugs, namespaces):
    product_rows = []

    for drug in drugs:
        # Extract primary DrugBank ID
        drug_id = None
        drugbank_ids = drug.findall('ns:drugbank-id', namespaces)
        for db_id in drugbank_ids:
            if db_id.attrib.get('primary') == 'true':
                drug_id = db_id.text
                break

        # If no primary ID, fallback to the first ID
        if not drug_id and drugbank_ids:
            drug_id = drugbank_ids[0].text

        # Skip drugs without products
        products = drug.find('ns:products', namespaces)
        if products is None:
            continue

        for product in products.findall('ns:product', namespaces):
            product_name = product.find('ns:name', namespaces).text if product.find('ns:name', namespaces) is not None else None
            manufacturer = product.find('ns:labeller', namespaces).text if product.find('ns:labeller', namespaces) is not None else None
            ndc_code = product.find('ns:ndc-product-code', namespaces).text if product.find('ns:ndc-product-code', namespaces) is not None else None
            dosage_form = product.find('ns:dosage-form', namespaces).text if product.find('ns:dosage-form', namespaces) is not None else None
            route = product.find('ns:route', namespaces).text if product.find('ns:route', namespaces) is not None else None
            strength = product.find('ns:strength', namespaces).text if product.find('ns:strength', namespaces) is not None else None
            country = product.find('ns:country', namespaces).text if product.find('ns:country', namespaces) is not None else None
            agency = product.find('ns:source', namespaces).text if product.find('ns:source', namespaces) is not None else None

            product_rows.append({
                'DrugBank ID': drug_id,
                'Product Name': product_name,
                'Labeller': manufacturer,
                'National Drug Code (NDC)': ndc_code,
                'Dosage Form': dosage_form,
                'Route of Administration': route,
                'Strength': strength,
                'Country': country,
                'Regulatory Agency': agency,
            })

    # Convert to DataFrame
    product_df = pd.DataFrame(product_rows)
    return product_df

# Main for usage of this function only
if __name__ == '__main__':
    file_path = "drugbank_partial.xml"

    # Define the namespace for XML parsing
    namespaces = {'ns': 'http://www.drugbank.ca'}

    # Load the DrugBank data
    drugs = load_drugbank_data(file_path, namespaces)

    product_data = extract_product_data(drugs, namespaces)

    # Save the product data to a CSV file
    product_data.to_csv("products.csv", index=False)
