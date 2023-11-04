import psycopg2
import graphviz
from graphviz import Graph

def connection_to_db():    
    try:
        # Define the connection parameters
        dbname = "TPC-H"
        user = "postgres"
        password = "voidbeast1"
        host = "127.0.0.1"
        port = "5432"  # Default PostgreSQL port is 5432

        # Create a connection to the database
        connection = psycopg2.connect(
            dbname=dbname,
            user=user,
            password=password,
            host=host,
            port=port
        )

        # Create a cursor
        cursor = connection.cursor()

        # Now, you have a working database connection and a cursor for executing SQL queries.

        return connection, cursor

    except psycopg2.Error as e:
        print(f"Error connecting to the database: {e}")

def get_qep_image(cursor, query):
    cursor.execute(f"EXPLAIN (FORMAT JSON) {query}")
    qep_json = cursor.fetchone()[0][0]
    print(qep_json)

    # Check if the QEP image is available in the JSON
    if "Plan" in qep_json:
        dot = Graph(comment="Query Execution Plan")
        add_nodes(dot, qep_json["Plan"])
        return dot
    else:
        return None

def add_nodes(dot, plan):
    if "Node Type" in plan:
        # Construct a label with the desired information
        node_label = f"{plan['Node Type']}\nStartup Cost: {plan.get('Startup Cost', 'N/A')}\nTotal Cost: {plan.get('Total Cost', 'N/A')}"

        # Add the node to the graph
        dot.node(node_label, label=node_label)

        if "Plans" in plan:
            for subplan in plan["Plans"]:
                # Recursively add the subplan to the graph
                add_nodes(dot, subplan)

                # Connect the node to its subplan
                dot.edge(node_label, f"{subplan['Node Type']}\nStartup Cost: {subplan.get('Startup Cost', 'N/A')}\nTotal Cost: {subplan.get('Total Cost', 'N/A')}")


