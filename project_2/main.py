import tkinter as tk
from PIL import Image, ImageTk
from test import connect_to_db, get_block_size, get_buffer_size, get_qep_image, close_db_connection, get_qep_statements

# Create the main window
window = tk.Tk()
window.title("SQL Query Executor")

# Set the window size to cover the entire screen
window.geometry("1500x1000")

# Create a top canvas for title, entry field, and button
top_canvas = tk.Canvas(window)
top_canvas.pack(side=tk.TOP, padx=10, pady=10)

# Create a title label with a big font in the top canvas
title_label = tk.Label(top_canvas, text="Enter SQL Query", font=("Helvetica", 15))
title_label.pack()

# Create an entry field for the SQL query in the top canvas
sql_entry = tk.Entry(top_canvas, width=50, font=("Helvetica", 10))
sql_entry.pack(pady=10)

# Function to execute the SQL query
def execute_sql_query():
    global legend_canvas  # Declare legend_canvas as a global variable
    query = sql_entry.get()

    try:
        connect_to_db()

        # Fetch the QEP image
        qep_digraph = get_qep_image(query)
        statements = get_qep_statements(query)
        buffer_size = get_buffer_size()
        blk_size = get_block_size()
        close_db_connection()

        # Save the QEP digraph as a PNG file
        qep_digraph.format = 'png'
        qep_digraph.render(filename="qep_tree")

        # Open the QEP image and convert it to Tkinter PhotoImage
        qep_image = Image.open("qep_tree.png")

        # Resize the QEP image to your desired dimensions
        resized_qep_img = qep_image.resize((600, 600))  # Adjust the dimensions as needed
        resized_qep_img.save("resized_qep_tree.png")

        qep_image = ImageTk.PhotoImage(resized_qep_img)

        qep_label.config(image=qep_image)
        qep_label.image = qep_image

        left_canvas.create_text(250, 620, anchor=tk.W, text=f"Buffer Size: {buffer_size}")
        left_canvas.create_text(250, 640, anchor=tk.W, text=f"Block Size: {blk_size} bytes")

        # Update the statements in the right canvas
        analysis_output_label.config(text='\n'.join(statements))

        # Create a legend canvas
        legend_canvas_height = len(legend_items) * 40 + 10  # Adjust the height based on the number of items
        legend_canvas = tk.Canvas(left_canvas, width=700, height=legend_canvas_height)
        legend_canvas.pack(pady=10)

        # Create legend items with font
        for i, item in enumerate(legend_items):
            y_offset = i * 40  # Adjust Y-coordinate to separate items
            legend_canvas.create_oval(5, 5 + y_offset, 15, 15 + y_offset, fill="black")
            legend_canvas.create_text(20, 10 + y_offset, text=item["text"], anchor="w", font=("Helvetica", 10))

    except Exception as e:
        result_label.config(text=f"Error: {str(e)}")

# Create a button to execute the SQL query in the top canvas
execute_button = tk.Button(top_canvas, text="Execute Query", command=execute_sql_query, font=("Helvetica", 10))
execute_button.pack(pady=10)

# Create a left canvas for QEP image and legend items
left_canvas = tk.Canvas(window)
left_canvas.pack(side=tk.LEFT, padx=10, pady=10)

# Create a right canvas for QEP analysis output
right_canvas = tk.Canvas(window)
right_canvas.pack(side=tk.RIGHT, padx=10, pady=10, fill=tk.Y)

# Create a label to display the QEP analysis output in the right canvas
analysis_output_label = tk.Label(right_canvas, text="", font=("Helvetica", 12), justify=tk.LEFT)
analysis_output_label.pack()

# Legend items
legend_items = [
    {"text": "Start-up cost is the estimated Postgres computational units to start up a node to start process the query."},
    {"text": "Total cost is the estimated Postgres computational units to finish process the query and return results"},
    {"text": "Shared Hit Blocks: number of shared blocks read into cache"},
    {"text": "Local Hit Blocks: number of local blocks read into cache"},
]

# Create a label to display the QEP image in the left canvas
qep_label = tk.Label(left_canvas, font=("Helvetica", 12))
qep_label.pack()

# Create a label to display the result in the left canvas
result_label = tk.Label(left_canvas, text="", font=("Helvetica", 12))
result_label.pack(pady=10)

# Start the mainloop
window.mainloop()
