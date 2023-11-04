import tkinter as tk
from tkinter import ttk  # Import ttk from tkinter for Treeview
import graphviz
from graphviz import Digraph
from test import connection_to_db, get_qep_image

#SELECT * FROM customer WHERE c_custkey = 1 OR c_custkey = 2
#test code to try to output image of QEP
def execute_sql_query():
    query = sql_entry.get()
    try:
        connection, cursor = connection_to_db()
        cursor.execute(query)

        # Fetch the QEP image
        qep_json = get_qep_image(cursor, query)

        # Add the nodes to the graph
        qep_dot = graphviz.Digraph(comment="Query Execution Plan")
        qep_dot.add_nodes(qep_json["Plan"])

        cursor.close()
        connection.close()

        if qep_dot:
            qep_window = tk.Toplevel(window)
            qep_window.title("Query Execution Plan (QEP)")

            # Render the QEP tree
            qep_dot.render("qep_tree", view=True)
            img_label = tk.Label(qep_window, text="Query Execution Plan (QEP)")
            img_label.pack()

        else:
            result_label.config(text="QEP not available for this query")

    except Exception as e:
        result_label.config(text=f"Error: {str(e)}")

window = tk.Tk()
window.title("SQL Query Executor")

# Create an entry field for the SQL query
sql_entry = tk.Entry(window, width=50)
sql_entry.pack(padx=10, pady=10)

# Create a button to execute the SQL query
execute_button = tk.Button(window, text="Execute Query", command=execute_sql_query)
execute_button.pack(padx=10, pady=10)

# Create a label to display the result
result_label = tk.Label(window, text="")
result_label.pack(padx=10, pady=10)

window.mainloop()