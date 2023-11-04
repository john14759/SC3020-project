import tkinter as tk
from PIL import Image, ImageTk
from test import connection_to_db, get_qep_image

# Define a function to execute the SQL query and display the QEP image
def execute_sql_query():
    query = sql_entry.get()

    try:
        connection, cursor = connection_to_db()
        cursor.execute(query)

        # Fetch the QEP image
        qep_img = get_qep_image(cursor, query)
        cursor.close()
        connection.close()

        # Save the QEP image as a PNG file
        qep_img.format = 'png'
        qep_img.render(filename="qep_tree")

        # Open the QEP image and convert it to Tkinter PhotoImage
        qep_image = Image.open("qep_tree.png")
        qep_image = ImageTk.PhotoImage(qep_image)

        qep_label.config(image=qep_image)
        qep_label.image = qep_image
        qep_label.pack()

    except Exception as e:
        result_label.config(text=f"Error: {str(e)}")

# Create the main window
window = tk.Tk()
window.title("SQL Query Executor")

# Create an entry field for the SQL query
sql_entry = tk.Entry(window, width=50)
sql_entry.pack(padx=10, pady=10)

# Create a button to execute the SQL query
execute_button = tk.Button(window, text="Execute Query", command=execute_sql_query)
execute_button.pack(padx=10, pady=10)

# Create a label to display the QEP image
qep_label = tk.Label(window)
qep_label.pack()

# Create a label to display the result
result_label = tk.Label(window, text="")
result_label.pack(padx=10, pady=10)

# Start the mainloop
window.mainloop()
