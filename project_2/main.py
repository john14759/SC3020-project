import tkinter as tk
from PIL import Image, ImageTk
from test import connect_to_db, get_block_size, get_buffer_size, get_qep_image, close_db_connection, get_qep_statements
from tkinter import Tk, Label
from threading import Thread

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

global create_legend_flag
legend_canvas = None
create_legend_flag = False

def create_legend():
    global create_legend_flag, legend_canvas

    if not create_legend_flag:
        # Check if legend_canvas already exists, if not create it
        if legend_canvas is None:
            legend_canvas_width = 400
            legend_canvas_height = len(legend_items) * 40 + 10
            font_size = 10
            legend_canvas = tk.Canvas(left_frame, width=legend_canvas_width, height=legend_canvas_height)
            legend_canvas.pack(pady=10)

        # Create legend items
        create_legend_items(legend_canvas, legend_items, 20, 10, 40, legend_canvas_width - 30, font_size)
        create_legend_flag = True

# Function to create scrollable canvas
def create_scrollable_canvas(parent, side=tk.LEFT, padx=10, pady=10, min_width=400):
    canvas = tk.Canvas(parent, width=min_width)
    scrollbar = tk.Scrollbar(parent, orient="vertical", command=canvas.yview)
    canvas.configure(yscrollcommand=scrollbar.set)
    canvas.pack(side=side, fill="both", expand=True, padx=padx, pady=pady)
    scrollbar.pack(side="right", fill="y")
    frame = tk.Frame(canvas, width=min_width)

    canvas_frame = canvas.create_window((min_width/2, 0), window=frame, anchor='center')
    frame.bind("<Configure>", lambda e: canvas.configure(scrollregion=canvas.bbox("all")))

    def on_canvas_configure(event):
        canvas.itemconfig(canvas_frame, width=event.width)
        canvas.move(canvas_frame, event.width/2 - canvas.coords(canvas_frame)[0], 0)
        canvas.config(scrollregion=canvas.bbox("all"))

    canvas.bind("<Configure>", on_canvas_configure)
    return canvas, frame

#Function to generate legend items on legend canvas
def create_legend_items(canvas, legend_items, start_x, start_y, line_height, text_width, font_size):

    # Clear existing legend items
    canvas.delete("legend_item")

    bullet_diameter = 4
    bullet_radius = bullet_diameter // 2
    text_vertical_offset = font_size // 4
    for i, item in enumerate(legend_items):
        y_offset = start_y + i * line_height  # Adjust Y-coordinate to separate items
        tag = f"legend_item_{i}"

        if( i < 2):
            canvas.create_oval(start_x - bullet_radius, y_offset + text_vertical_offset - bullet_radius, start_x + bullet_radius, y_offset + text_vertical_offset + bullet_radius, fill="black", tags=tag)
            canvas.create_text(start_x + 10, y_offset + 10, text=item["text"], anchor="w", font=("Helvetica", 10), width=text_width, tags=tag)
        else:
            canvas.create_oval(start_x - bullet_radius, y_offset + 10 - bullet_radius, start_x + bullet_radius, y_offset + 10 + bullet_radius, fill="black", tags=tag)
            canvas.create_text(start_x + 10, y_offset + 10, text=item["text"], anchor="w", font=("Helvetica", 10), width=text_width, tags=tag)

# Function to execute the SQL query
def execute_sql_query():
    # Function to execute the SQL query in a separate thread
    def execute_query_thread():
        try:
            connect_to_db()

            # Fetch the QEP image
            qep_digraph = get_qep_image(query)

            # Check if QEP is None, indicating an invalid query
            if qep_digraph is None:
                result_label.config(text="Error: Invalid query. Please check your SQL syntax.")
                return

            statements = get_qep_statements()
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
            qep_label.pack(side="top", fill="both", expand=True)

            # Update the statements in the right frame
            analysis_output_label.config(text='\n'.join(statements), font=("Helvetica", 10))
            analysis_output_label.pack(side="top", fill="both", expand=True)
            for i, statement in enumerate(statements):
                button = tk.Button(right_frame, text=f"Step {i+1} Details", command=lambda s=statement: view_statement_details(s))
                button.pack()
            create_legend()

        except Exception as e:
            result_label.config(text=f"Error: {str(e)}")

    # Get the query from the entry field
    query = sql_entry.get()

    # Create a thread to execute the query
    query_thread = Thread(target=execute_query_thread)
    query_thread.start()

def view_statement_details(statement):
    new_window = tk.Toplevel(window)
    new_window.title("Details")
    label = tk.Label(new_window, text=statement, font=("Helvetica", 12))
    label.pack(padx=20, pady=20)

# Create a button to execute the SQL query in the top canvas
execute_button = tk.Button(top_canvas, text="Execute Query", command=execute_sql_query, font=("Helvetica", 10))
execute_button.pack(pady=10)

# Create scrollable left and right canvases
left_canvas, left_frame = create_scrollable_canvas(window, side=tk.LEFT, min_width=400)
right_canvas, right_frame = create_scrollable_canvas(window, side=tk.RIGHT, min_width=400)

# Create a label to display the QEP analysis output in the right canvas
analysis_output_label = tk.Label(right_frame, text="", font=("Helvetica", 12), justify=tk.LEFT)
analysis_output_label.place()

# Legend items
legend_items = [
    {"text": "Start-up cost: Estimated Postgres computational units to start up a node to start process the query."},
    {"text": "Total cost: Estimated Postgres computational units to finish process the query and return results"},
    {"text": "Shared Hit Blocks: Number of shared blocks read into cache"},
    {"text": "Local Hit Blocks: Number of local blocks read into cache"},
]


# Create a label to display the QEP image in the left canvas
qep_label = tk.Label(left_frame, font=("Helvetica", 12))
qep_label.place()

# Create a label to display the result in the left canvas
result_label = tk.Label(left_frame, text="", font=("Helvetica", 12))
result_label.place()

# Start the mainloop
window.mainloop()
