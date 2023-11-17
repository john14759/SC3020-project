import psycopg2
import graphviz
from graphviz import Digraph

def connect_to_db():    
    try:
        # Define the connection parameters
        dbname = "TPC-H"
        user = "postgres"
        password = "Anyaforger1!"
        host = "127.0.0.1"
        port = "5432"  # Default PostgreSQL port is 5432

        # Create a connection to the database
        global connection
        connection = psycopg2.connect(
            dbname=dbname,
            user=user,
            password=password,
            host=host,
            port=port
        )

        # Create a cursor
        global cursor
        cursor = connection.cursor()

        # Now, you have a working database connection and a cursor for executing SQL queries.

    except psycopg2.Error as e:
        print(f"Error connecting to the database: {e}")

def close_db_connection():
    cursor.close()
    connection.close()

def get_qep_image(query):
    try:
        explain_query = f"EXPLAIN (ANALYZE, BUFFERS, FORMAT JSON) {query}"
        cursor.execute(explain_query)
        qep_json = cursor.fetchone()[0][0]
        print(qep_json)
        analyze_qep(qep_json['Plan'])

        # Check if the QEP image is available in the JSON
        if "Plan" in qep_json:
            dot = Digraph(comment="Query Execution Plan")
            dot.graph_attr['bgcolor'] = 'lightyellow'
            add_nodes(dot, qep_json["Plan"])
            return dot
        else:
            return None
        
    except Exception as e:
        # Handle exceptions, and return an informative message
        return [f"Error analyzing the query: {str(e)}"]
    
def get_qep_statements(query):
    try:
        explain_query = f"EXPLAIN (ANALYZE, BUFFERS, FORMAT JSON) {query}"
        cursor.execute(explain_query)
        qep_json = cursor.fetchone()[0][0]
        statements = []

        # Check if the QEP image is available in the JSON
        if "Plan" in qep_json:
            _, statements = analyze_qep(qep_json["Plan"], statements=statements)
            return statements
        else:
            return None
    except Exception as e:
        # Handle exceptions, and return an informative message
        return [f"Error analyzing the query: {str(e)}"]

def get_buffer_size():
    cursor.execute("show shared_buffers")
    return cursor.fetchone()[0]

def get_block_size():
    cursor.execute("show block_size")
    return cursor.fetchone()[0]

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
        'Append': 'Appends the results of multiple scans or subqueries.',
        'Bitmap Heap Scan': 'Reads data from a table using a bitmap and retrieves matching rows.',
        'Bitmap Index Scan': 'Uses a bitmap to find matching rows in an index.',
        'BitmapOr': 'Performs a logical OR operation on the results of multiple bitmap scans.',
        'Custom Scan': 'Performs a custom scan using a custom access method.',
        'Foreign Scan': 'Scans a foreign table using a foreign data wrapper.',
        'Function Scan': 'Generates rows by calling a set-returning function.',
        'Gather': 'Collects rows from multiple worker processes in parallel.',
        'Gather Merge': 'Merges rows collected from multiple worker processes in parallel.',
        'Hash': 'Builds a hash table for hash-based operations.',
        'Hash Join': 'Joins two tables using a hash function to distribute rows across buckets.',
        'Index Only Scan': 'Retrieves data directly from an index without visiting the table.',
        'Index Scan': 'Reads rows from a table using an index.',
        'Limit': 'Limits the number of rows returned by a subplan.',
        'LockRows': 'Acquires a row-level lock on rows returned by a subplan.',
        'Materialize': 'Materializes the results of a subquery.',
        'Merge Append': 'Merges the results of multiple scans or subqueries.',
        'Merge Join': 'Joins two pre-sorted tables by merging their sorted rows.',
        'Nested Loop': 'Joins two tables by nested loop iteration over the outer and inner tables.',
        'Recursive Union': 'Performs a recursive union of multiple scans or subqueries.',
        'Seq Scan': 'Sequentially scans a table, reading all rows in the table.',
        'SetOp': 'Performs a set operation (UNION, INTERSECT, or EXCEPT) on the results of multiple scans or subqueries.',
        'Subquery Scan': 'Executes a subquery and treats its result as a table.',
        'Table Function Scan': 'Generates rows by calling a table function.',
        'Tid Scan': 'Scans a table using tuple ID (TID) values.',
        'Values Scan': 'Generates rows from a set of specified values.',
        'WorkTable Scan': 'Reads rows from a materialized or temporary work table.',
        'Unique': 'Removes duplicate data',
        'Sort': 'Sort and return rows',
        'CTE Scan': 'Runs part of a query and stores the output so that it can be used by other part(s) of the query',
        'Aggregate': 'Perform a calculation on a set of values and return a single result, such as counting, summing, averaging, or finding the maximum or minimum value within a group of rows.',
    }
    return explanations.get(node_type, f'There is no explanation available for this node type: {node_type}.')

def explain_join_type(join_type):
    explanations = {
        'Hash Join': 'Joins two tables using a hash function to distribute rows across buckets.',
        'Hash Semi Join': 'Returns only the rows from the inner table that have matching rows in the outer table.',
        'Hash Anti Join': 'Returns only the rows from the inner table that have no matching rows in the outer table.',
        'Merge Join': 'Joins two pre-sorted tables by merging their sorted rows.',
        'Merge Semi Join': 'Returns only the rows from the inner table that have matching rows in the outer table.',
        'Merge Anti Join': 'Returns only the rows from the inner table that have no matching rows in the outer table.',
        'Nested Loop': 'Joins two tables by nested loop iteration over the outer and inner tables.',
        'Nested Loop Semi Join': 'Returns only the rows from the inner table that have matching rows in the outer table.',
        'Nested Loop Anti Join': 'Returns only the rows from the outer table that have no matching rows in the inner table.',
        'Bitmap Index Scan': 'Uses a bitmap to find matching rows in an index.',
        'Bitmap Heap Scan': 'Reads data from a table using a bitmap and retrieves matching rows.',
        'BitmapOr': 'Performs a logical OR operation on the results of multiple bitmap scans.',
        'Tid Scan': 'Scans a table using tuple ID (TID) values.',
        'Inner': 'Returns only the rows that have matching rows in both the inner and outer tables.',
        'Right': 'Retrieves all records from the right table and matching records from the left table.',
        'Left': 'Retrieves all records from the left table and the matching records from the right table.',
        'Full': 'Retrieves all records when there is a match in either the left or the right table.',
        'Semi': 'Returns only the rows from the left table for which there is a match in the right table, it does not actually return the columns from the right table.'
    }
    return explanations.get(join_type, f'There is no explanation available for this join type: {join_type}.')

def extract_relations_for_join(plans):
    """
    Extracts the relation names from child plans.

    Args:
    - plans (list): The child plans of the join.

    Returns:
    Tuple[str, str]: A tuple containing the names of the left and right relation.
    """
    left_relation = None
    right_relation = None
    for child_plan in plans:
        if 'Relation Name' in child_plan:
            if left_relation is None:
                left_relation = child_plan['Relation Name']
            else:
                right_relation = child_plan['Relation Name']
                break  # Stop after finding two relations
    return left_relation, right_relation

def analyze_qep(qep, indent=0, first_line_indent=0, step=1, statements=None):
    """
    Analyzes the Query Execution Plan (QEP) and prints a step-by-step analysis.

    Args:
    - qep (dict): The Query Execution Plan in JSON format.
    - indent (int): The current indentation level for formatting.
    - first_line_indent (int): Additional indentation for the first line.
    - step (int): The current step number.
    - statements (list): A list to store the printf statements.

    Returns:
    None
    """
    indent_str = " " * first_line_indent

    # Initialize the statements list if not provided
    if statements is None:
        statements = []

    # Recursively analyze child nodes if they exist
    plans = qep.get('Plans', [])
    for i, plan in enumerate(reversed(plans), start=1):
        # Additional indentation for child nodes
        step, statements = analyze_qep(plan, indent + 2, first_line_indent, step, statements)

    # Append details of the current node to the statements list
    node_type = qep.get('Node Type', 'NULL')
    join_type = qep.get('Join Type', 'NULL')
    relation_name = qep.get('Relation Name', 'NULL')
    index_name = qep.get('Index Name', 'NULL')
    hash_condition = qep.get('Hash Cond', 'NULL')

    statement = (
        f"{indent_str}Step {step}:\n"
    )

    # Add explanation for relation and index usage
    if relation_name != 'NULL':
        statement += f"{indent_str}  A sequential scan is performed on the relation {relation_name}.\n"
    if index_name != 'NULL':
        statement += f"{indent_str}  An index scan is performed using the index {index_name}.\n"

    # Add explanation for node type
    statement += f"{indent_str}  ({node_type}) {explain_node_type(node_type)}\n"

    # Add explanation for joins if applicable
    if join_type != 'NULL':
        statement += f"{indent_str}  ({join_type}) {explain_join_type(join_type)}\n"
        left_relation, right_relation = extract_relations_for_join(plans)
        if left_relation and right_relation:
            statement += f"{indent_str}  {left_relation} is {join_type.lower().replace('_', ' ')} with {right_relation}.\n"

    # Add more detailed explanation for hash and hash join
    if 'Hash' in node_type:
        if 'Plans' in qep:
            for child_plan in reversed(qep['Plans']):
                if 'Seq Scan' in child_plan.get('Node Type', ''):
                    child_relation = child_plan.get('Relation Name', 'NULL')
                    statement += f"{indent_str}  Hash the results of sequential scan on relation {child_relation}.\n"

        if 'Hash Join' in node_type:
            if 'Plans' in qep:
                for child_plan in reversed(qep['Plans']):
                    if 'Seq Scan' in child_plan.get('Node Type', ''):
                        child_relation = child_plan.get('Relation Name', 'NULL')
                        statement += f"{indent_str}  Join hashed results with sequential scan on relation {child_relation}.\n"

    if 'Nested Loop' in node_type or 'Merge Join' in node_type:
        left_relation, right_relation = extract_relations_for_join(plans)
        if left_relation and right_relation:
            # Construct a statement that describes the join operation
            join_method = "merge join" if 'Merge Join' in node_type else "nested loop join"
            statement += f"{indent_str}  {left_relation} is joined with {right_relation} using {join_method}.\n"

    # Add a newline for better readability
    statement += "\n"

    # Append the statement to the list
    statements.append(statement)

    return step + 1, statements




