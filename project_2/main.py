import tkinter as tk
from test import connection_to_db

def execute_sql_query():
    query = sql_entry.get()
    try:
        connection, cursor = connection_to_db()
        cursor.execute(query)
        
        # Fetch the results
        rows = cursor.fetchall()

        for row in rows:
            print(row)


        # Close the cursor and the database connection when done
        cursor.close()
        connection.close()
        result_label.config(text="Query executed successfully")
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




