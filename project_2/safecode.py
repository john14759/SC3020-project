 # code to display queries only
 def execute_sql_query():
    query = sql_entry.get()
    try:
        connection, cursor = connection_to_db()
        cursor.execute(query)
        
        # Fetch the results
        rows = cursor.fetchall()
        
        if len(rows) == 0:
            result_label.config(text="No results found")
        else:
            # Create a new window to display the result
            result_window = tk.Toplevel(window)
            result_window.title("Query Result")
            
            # Create a Treeview widget to display the data
            result_tree = ttk.Treeview(result_window, columns=range(len(rows[0])), show="headings")
            result_tree.pack()
            
            # Add columns to the Treeview if there are results
            for i in range(len(rows[0])):
                result_tree.heading(i, text=f"Column {i}")
            
            # Add data to the Treeview
            for row in rows:
                result_tree.insert("", "end", values=row)
            
            # Close the cursor and the database connection when done
            cursor.close()
            connection.close()
            result_label.config(text="Query executed successfully")
    except Exception as e:
        result_label.config(text=f"Error: {str(e)}")