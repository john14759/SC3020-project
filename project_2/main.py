import tkinter as tk
from PIL import Image, ImageTk
from test import connection_to_db, get_qep_image

# Create the main window
window = tk.Tk()
window.title("SQL Query Executor")

# Set the window size to cover the entire screen
window.geometry(f"{window.winfo_screenwidth()}x{window.winfo_screenheight()}+0+0")

# Create a title label with a big font
title_label = tk.Label(window, text="Enter SQL Query", font=("Helvetica", 15))
title_label.pack(pady=10)

# Create an entry field for the SQL query
sql_entry = tk.Entry(window, width=50, font=("Helvetica", 10))
sql_entry.pack(padx=10, pady=10)

# Legend items
legend_items = [
    {"text": "Start-up cost is the estimated Postgres computational units to start up a node to start process the query"},
    {"text": "Total cost is the estimated Postgres computational units to finish process the query and return results"},
    {"text": "Shared Hit Blocks: number of shared blocks read into cache"},
    {"text": "Local Hit Blocks: number of local blocks read into cache"},
]

# Create a button to execute the SQL query
def execute_sql_query():
    global legend_canvas  # Declare legend_canvas as a global variable
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

        # Create a legend canvas
        legend_canvas_height = len(legend_items) * 40 + 10  # Adjust the height based on the number of items
        legend_canvas = tk.Canvas(window, width=700, height=legend_canvas_height)
        legend_canvas.pack(padx=10, pady=10)

        # Create legend items with font
        for i, item in enumerate(legend_items):
            y_offset = i * 40  # Adjust Y-coordinate to separate items
            legend_canvas.create_oval(5, 5 + y_offset, 15, 15 + y_offset, fill="black")
            legend_canvas.create_text(20, 10 + y_offset, text=item["text"], anchor="w", font=("Helvetica", 10))

    except Exception as e:
        result_label.config(text=f"Error: {str(e)}")

# Create a button to execute the SQL query
execute_button = tk.Button(window, text="Execute Query", command=execute_sql_query, font=("Helvetica", 10))
execute_button.pack(padx=10, pady=10)

# Create a label to display the QEP image
qep_label = tk.Label(window, font=("Helvetica", 12))
qep_label.pack()

# Create a label to display the result
result_label = tk.Label(window, text="", font=("Helvetica", 12))
result_label.pack(padx=10, pady=10)

# Start the mainloop
window.mainloop()

