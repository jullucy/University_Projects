""" drugbank_api.py

    This FastAPI application provides an API for interacting with DrugBank data.
    It allows users to query the number of pathways that interact with a specific drug
    based on its DrugBank ID.

    Endpoints:
        1. GET / - A simple welcome message to confirm that the API is running.
        2. POST /drug-pathways - Accepts a DrugBank ID and returns the number of pathways
        that the drug interacts with, based on the DrugBank XML data.

     The application processes the DrugBank data stored in an XML file, and users can
     query the data by providing a drug ID. The data is parsed using ElementTree to
     extract information on pathways associated with each drug.
     http://127.0.0.1:8000/docs You have to open this link to send the data
"""
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import xml.etree.ElementTree as ET

# Initialize the FastAPI app
app = FastAPI()

@app.get("/")
async def root():
    return {"message": "Welcome to the DrugBank API"}

# Class for the request body
class DrugRequest(BaseModel):
    drug_id: str


# Function to load the DrugBank data
def load_drugbank_data(file_path):
    tree = ET.parse(file_path)
    root = tree.getroot()
    return root


"""
    Counts the number of pathways that interact with a given drug.

    Parameters:
    - root: XML element of the DrugBank database.
    - drugbank_id: ID of the drug.

    Returns:
    - The number of pathways that interact with the drug.
    """
def count_drug_interactions(root, drugbank_id):
    namespace = {"ns": "http://www.drugbank.ca"}
    drugs = root.findall("ns:drug", namespace)

    for drug in drugs:
        # Check if this is the correct drug
        drugbank_ids = drug.findall("ns:drugbank-id", namespace)
        primary_id = next((db_id.text for db_id in drugbank_ids if db_id.attrib.get("primary") == "true"), None)

        if primary_id == drugbank_id:
            # Find the pathways
            pathways = drug.find("ns:pathways", namespace)
            if pathways is not None:
                return len(pathways.findall("ns:pathway", namespace))

    # If the drug was not found
    raise HTTPException(status_code=404, detail=f"Drug with ID {drugbank_id} not found")


"""
    Returns the number of pathways for a given drug based on its ID.

    Parameters:
    - data: JSON with the field drug_id.

    Returns:
    - The number of pathways interacting with the drug.
"""
@app.post("/drug-pathways")
def get_drug_pathways(data: DrugRequest):
    # Path to the DrugBank XML data file
    file_path = "drugbank_partial.xml"
    root = load_drugbank_data(file_path)

    # Calculate the number of pathways for the given drug
    count = count_drug_interactions(root, data.drug_id)

    return {"drug_id": data.drug_id, "pathway_count": count}
