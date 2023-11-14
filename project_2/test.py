import psycopg2
import graphviz
from graphviz import Digraph

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
    explain_query = f"EXPLAIN (ANALYZE, BUFFERS, FORMAT JSON) {query}"
    cursor.execute(explain_query)
    qep_json = cursor.fetchone()[0][0]
    print(qep_json)
    analyze_qep(qep_json['Plan'])

    # Check if the QEP image is available in the JSON
    if "Plan" in qep_json:
        dot = Digraph(comment="Query Execution Plan")
        add_nodes(dot, qep_json["Plan"])
        return dot
    else:
        return None

def add_nodes(dot, plan, parent_id=None, node_id=0):
    if "Node Type" in plan:
        # Extract information from the JSON data
        node_type = plan["Node Type"]
        startup_cost = plan.get('Startup Cost', 'N/A')
        total_cost = plan.get('Total Cost', 'N/A')
        relation_name = plan.get('Relation Name', 'N/A')
        index_name = plan.get('Index Name', 'N/A')
        shared_hit_blocks = plan.get('Shared Hit Blocks', 'N/A')

        # Construct the label with relevant information
        label = f"{node_type}\nRelation Name: {relation_name}\nIndex Name: {index_name}\nStartup Cost: {startup_cost}\nTotal Cost: {total_cost}\nShared Hit Blocks: {shared_hit_blocks}"
        
        # Define a unique identifier for the node
        node_name = f"node{node_id}"
        dot.node(node_name, label=label)

        # If there's a parent, add an edge from this node to the parent (reverse order)
        if parent_id is not None:
            dot.edge(node_name, parent_id)

        # Recurse for children if they exist
        if "Plans" in plan:
            for subplan in plan["Plans"]:
                node_id += 1
                # Recursively add the subplan to the graph with incremented node ID
                node_id = add_nodes(dot, subplan, parent_id=node_name, node_id=node_id)

    # Return the next available node_id
    return node_id

def explain_node_type(node_type):
    explanations = {
        'Bitmap Heap Scan': 'Reads data from a table using a bitmap and retrieves matching rows.',
        'Bitmap Index Scan': 'Uses a bitmap to find matching rows in an index.',
        'BitmapOr': 'Performs a logical OR operation on the results of multiple bitmap scans.',
        # Add more explanations for other node types as needed
    }
    return explanations.get(node_type, f'There is no join available in this step for explanation.')

def explain_join_type(join_type):
    explanations = {
        'Nested Loop': 'Joins two tables by nested loop iteration over the outer and inner tables.',
        'Hash Join': 'Joins two tables using a hash function to distribute rows across buckets.',
        'Merge Join': 'Joins two pre-sorted tables by merging their sorted rows.',
        # Add more explanations for other join types as needed
    }
    return explanations.get(join_type, f'There is no join available in this step for explanation.')

def analyze_qep(qep, indent=0, first_line_indent=0, step=1):
    """
    Analyzes the Query Execution Plan (QEP) and prints a step-by-step analysis.

    Args:
    - qep (dict): The Query Execution Plan in JSON format.
    - indent (int): The current indentation level for formatting.
    - first_line_indent (int): Additional indentation for the first line.
    - step (int): The current step number.

    Returns:
    None
    """
    indent_str = " " * first_line_indent

    # Recursively analyze child nodes if they exist
    plans = qep.get('Plans', [])
    for i, plan in enumerate(reversed(plans), start=1):
        # Additional indentation for child nodes
        step = analyze_qep(plan, indent + 2, first_line_indent, step)

    # Print details of the current node
    node_type = qep.get('Node Type', 'NULL')
    join_type = qep.get('Join Type', 'NULL')
    relation_name = qep.get('Relation Name', 'NULL')
    index_name = qep.get('Index Name', 'NULL')

    print(f"{indent_str}Step {step}:")
    print(f"{indent_str}  ({node_type}) {explain_node_type(node_type)}")
    print(f"{indent_str}  ({join_type}) {explain_join_type(join_type)}")
    print(f"{indent_str}  Relation Name: {relation_name}")
    print(f"{indent_str}  Index Name: {index_name}")
    print()

    return step + 1



