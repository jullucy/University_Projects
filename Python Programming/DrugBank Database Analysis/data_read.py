import xml.etree.ElementTree as ET

"""
    Load and parse a DrugBank XML file, returning a list of drug elements.

    This function reads a DrugBank XML file from the specified file path and
    parses it using Python's `xml.etree.ElementTree` module. It retrieves all
    `drug` elements within the XML document based on the provided namespace.
"""
def load_drugbank_data(file_path, namespaces):
    tree = ET.parse(file_path)
    root = tree.getroot()

    # Parse the XML file incrementally
    drugs = root.findall("ns:drug", namespaces)

    return drugs